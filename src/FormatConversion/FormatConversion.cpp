#include "../Pch.hpp"

#include "FormatConversion.hpp"

namespace Velyra::Image {

    std::vector<int> defineSwizzle(const VL_CHANNEL_FORMAT sourceFormat, const VL_CHANNEL_FORMAT targetFormat) {
        auto decoder = [&](const VL_CHANNEL_FORMAT fmt) {
            switch (fmt) {
                case VL_CHANNEL_R:    return std::vector<int>{0};
                case VL_CHANNEL_RG:   return std::vector<int>{0,1};
                case VL_CHANNEL_RGB:  return std::vector<int>{0,1,2};
                case VL_CHANNEL_RGBA: return std::vector<int>{0,1,2,3};
                case VL_CHANNEL_BGR:  return std::vector<int>{2,1,0};
                case VL_CHANNEL_BGRA: return std::vector<int>{2,1,0,3};
                default: return std::vector<int>{};
            }
        };
        std::vector<int> sourceChannels = decoder(sourceFormat);
        std::vector<int> targetChannels = decoder(targetFormat);

        // Build swizzle map
        // -1 indicates that the channel is not present in the source and should be filled
        std::vector<int> swizzle(targetChannels.size(), -1);
        for (Size i = 0; i < targetChannels.size(); ++i) {
            int targetChannel = targetChannels[i];
            auto it = std::find(sourceChannels.begin(), sourceChannels.end(), targetChannel);
            if (it != sourceChannels.end()) {
                swizzle[i] = static_cast<int>(std::distance(sourceChannels.begin(), it));
            }
        }
        return swizzle;
    }

    __m256i buildSwizzleMask_U8_AVX2(const VL_CHANNEL_FORMAT sourceFormat, const VL_CHANNEL_FORMAT targetFormat) {
        std::array<U8, 32> mask{};
        const std::vector<int> swizzle = defineSwizzle(sourceFormat, targetFormat);
        const U32 sourceChannelCount = getChannelCountFromFormat(sourceFormat);
        const U32 targetChannelCount = getChannelCountFromFormat(targetFormat);

        const Size pixelsPerLane = 16 / targetChannelCount;

        for (Size lane = 0; lane < 2; ++lane) {
            for (Size p = 0; p < pixelsPerLane; ++p) {
                const Size pixelBase = lane * 16 + p * targetChannelCount;

                for (U32 c = 0; c < targetChannelCount; ++c) {
                    const Size outIndex = pixelBase + c;

                    const int srcChannel = swizzle[c];
                    if (srcChannel < 0) {
                        // Fill: mark 0x80
                        mask[outIndex] = U8{0x80};
                    } else {
                        // srcChannel >= 0 here, so the cast to the unsigned Size is safe
                        const Size srcByte = p * sourceChannelCount + static_cast<Size>(srcChannel);

                        // Limit to lane (0–15 or 16–31)
                        mask[outIndex] = static_cast<U8>(srcByte);
                    }
                }
            }
        }
        return _mm256_loadu_si256(reinterpret_cast<__m256i const *>(mask.data()));
    }

    __m256i buildAlphaBlendMask_U8_AVX2(const VL_CHANNEL_FORMAT sourceFormat, const VL_CHANNEL_FORMAT targetFormat) {
        std::array<uint8_t, 32> mask{};
        const std::vector<int> swizzle = defineSwizzle(sourceFormat, targetFormat);
        const U32 targetChannelCount = getChannelCountFromFormat(targetFormat);
        const Size pixelsPerLane = 16 / targetChannelCount;

        for (Size lane = 0; lane < 2; ++lane) {
            for (Size p = 0; p < pixelsPerLane; ++p) {
                const Size pixelBase = lane * 16 + p * targetChannelCount;
                for (Size c = 0; c < targetChannelCount; ++c) {
                    const Size outIndex = pixelBase + c;
                    if (swizzle[c] < 0) {
                        // this output byte should be filled -> set mask to 0xFF (blend select)
                        mask[outIndex] = 0xFF;
                    } else {
                        mask[outIndex] = 0x00;
                    }
                }
            }
        }
        return _mm256_loadu_si256(reinterpret_cast<const __m256i*>(mask.data()));
    }

    void convertFormat_U8_AVX2(const VL_CHANNEL_FORMAT sourceFormat, const std::span<const U8> sourceData,
        const VL_CHANNEL_FORMAT targetFormat, std::vector<U8> &targetData, const VL_FORMAT_CONVERSION_FILL fillMode) {

        const U32 srcStride = getChannelCountFromFormat(sourceFormat);
        const U32 dstStride = getChannelCountFromFormat(targetFormat);

        const Size pixelCount = sourceData.size() / srcStride;
        const U8 fillValue = getFillValue<U8>(fillMode);

        // Pixels processed per 128-bit lane.
        //
        // We deliberately operate on a single 128-bit lane instead of a full 256-bit
        // vector: _mm256_shuffle_epi8 shuffles each 128-bit lane independently and
        // cannot move bytes across the 16-byte lane boundary. Whenever the source or
        // target stride does not tile that boundary (e.g. the 3-byte RGB/BGR formats),
        // a symmetric 256-bit mask makes the high lane read from the wrong source
        // offset and corrupts every second group of pixels. A per-lane 128-bit shuffle
        // sidesteps that limitation and is correct for every stride combination.
        //
        // The number of pixels we can safely produce per iteration is bounded by both
        // the 16-byte source load and the 16-byte target store.
        const Size pixelsPerVec = std::min<Size>(16 / srcStride, 16 / dstStride);
        if (pixelsPerVec == 0) {
            // A single pixel does not fit in a 128-bit lane; defer entirely to the scalar path.
            convertFormat_Scalar<U8>(sourceFormat, sourceData, targetFormat, targetData, fillMode);
            return;
        }

        // The low 128 bits of the AVX2 masks describe exactly one lane, which is what we need here.
        const __m128i shuffleMask = _mm256_castsi256_si128(buildSwizzleMask_U8_AVX2(sourceFormat, targetFormat));
        const __m128i fillVec = _mm_set1_epi8(static_cast<char>(fillValue));
        const __m128i blendMask = _mm256_castsi256_si128(buildAlphaBlendMask_U8_AVX2(sourceFormat, targetFormat));

        Size i = 0;
        const Size srcByteCount = sourceData.size();
        const Size dstByteCount = targetData.size();
        // vector loop: a 128-bit load/store always touches 16 bytes, so both ends must be fully in range
        for (; i + pixelsPerVec <= pixelCount; i += pixelsPerVec) {
            const Size srcByteOffset = i * srcStride;
            const Size dstByteOffset = i * dstStride;

            // make sure we have 16 bytes available to read from source and 16 bytes available to write to target
            if (srcByteOffset + 16 > srcByteCount) break;  // not enough source bytes for a safe 16-byte load
            if (dstByteOffset + 16 > dstByteCount) break;  // not enough room to store 16 bytes

            // load 16 bytes from source (UNALIGNED OK)
            const U8* srcPtr = sourceData.data() + srcByteOffset;
            const __m128i block = _mm_loadu_si128(reinterpret_cast<const __m128i*>(srcPtr));

            // shuffle to desired layout; bytes with 0x80 in mask become zero
            __m128i outVec = _mm_shuffle_epi8(block, shuffleMask);

            // blend the fill value only into the fill (e.g. alpha) positions
            // _mm_blendv_epi8 selects bytes from second operand where MSB of mask byte is set
            outVec = _mm_blendv_epi8(outVec, fillVec, blendMask);

            // store 16 bytes into target
            U8* dstPtr = targetData.data() + dstByteOffset;
            _mm_storeu_si128(reinterpret_cast<__m128i*>(dstPtr), outVec);
        }

        // Scalar tail (handles remaining pixels)
        const std::vector<int> swz = defineSwizzle(sourceFormat, targetFormat);
        for (; i < pixelCount; ++i) {
            const Size sOff = i * srcStride;
            const Size dOff = i * dstStride;
            for (U32 c = 0; c < dstStride; ++c) {
                int srcChannel = -1;
                srcChannel = swz[c];
                if (srcChannel < 0) {
                    targetData[dOff + c] = fillValue;
                } else {
                    targetData[dOff + c] = sourceData[sOff + static_cast<Size>(srcChannel)];
                }
            }
        }
    }

}