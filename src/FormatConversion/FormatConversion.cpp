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
        std::array<U8, 32> mask;
        const std::vector<int> swizzle = defineSwizzle(sourceFormat, targetFormat);
        const U32 sourceChannelCount = getChannelCountFromFormat(sourceFormat);
        const U32 targetChannelCount = getChannelCountFromFormat(targetFormat);

        const int pixelsPerLane = 16 / targetChannelCount;

        for (int lane = 0; lane < 2; ++lane) {
            for (int p = 0; p < pixelsPerLane; ++p) {
                const int pixelBase = lane * 16 + p * targetChannelCount;

                for (int c = 0; c < targetChannelCount; ++c) {
                    const int outIndex = pixelBase + c;

                    int srcChannel = swizzle[c];
                    if (srcChannel < 0) {
                        // Fill: mark 0x80
                        mask[outIndex] = 0x80;
                    } else {
                        int srcByte = p * sourceChannelCount + srcChannel;

                        // Limit to lane (0–15 or 16–31)
                        mask[outIndex] = static_cast<uint8_t>(srcByte);
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
        const int pixelsPerLane = 16 / targetChannelCount;

        for (int lane = 0; lane < 2; ++lane) {
            for (int p = 0; p < pixelsPerLane; ++p) {
                const int pixelBase = lane * 16 + p * targetChannelCount;
                for (int c = 0; c < (int)targetChannelCount; ++c) {
                    const int outIndex = pixelBase + c;
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

    void convertFormat_U8_AVX2(const VL_CHANNEL_FORMAT sourceFormat, const std::vector<U8> &sourceData,
        const VL_CHANNEL_FORMAT targetFormat, std::vector<U8> &targetData, const VL_FORMAT_CONVERSION_FILL fillMode) {

        const U32 srcStride = getChannelCountFromFormat(sourceFormat);
        const U32 dstStride = getChannelCountFromFormat(targetFormat);

        const Size pixelCount = sourceData.size() / srcStride;
        const U8 fillValue = getFillValue<U8>(fillMode);

        // pixels processed per 256-bit vector (32 bytes)
        const int pixelsPerVec = 32 / dstStride;
        if (pixelsPerVec <= 0) {
            // defensive: divide by zero protection
            // fallback to scalar
            for (Size i = 0; i < pixelCount; ++i) {
                for (U32 c = 0; c < dstStride; ++c) { targetData[i * dstStride + c] = 0; } // minimal
            }
            return;
        }

        const __m256i shuffleMask = buildSwizzleMask_U8_AVX2(sourceFormat, targetFormat);
        const __m256i alphaVec = _mm256_set1_epi8(static_cast<char>(fillValue));
        const __m256i alphaBlendMask = buildAlphaBlendMask_U8_AVX2(sourceFormat, targetFormat);

        Size i = 0;
        const Size srcByteCount = sourceData.size();
        const Size dstByteCount = targetData.size();
        // vector loop: ensure we don't read past source buffer (need at least 32 source bytes available at read address)
        for (; i + pixelsPerVec <= pixelCount; i += pixelsPerVec) {
            const Size srcByteOffset = i * srcStride;
            const Size dstByteOffset = i * dstStride;

            // make sure we have 32 bytes available to read from source and 32 bytes available to write to target
            if (srcByteOffset + 32 > srcByteCount) break;  // not enough source bytes for a safe 32-byte load
            if (dstByteOffset + 32 > dstByteCount) break;  // not enough room to store 32 bytes

            // load 32 bytes from source (UNALIGNED OK)
            const uint8_t* srcPtr = sourceData.data() + srcByteOffset;
            __m256i block = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(srcPtr));

            // shuffle to desired layout; bytes with 0x80 in mask become zero
            __m256i outVec = _mm256_shuffle_epi8(block, shuffleMask);

            // blend alpha fillValue only into the alpha positions
            // _mm256_blendv_epi8 selects bytes from second operand where MSB of mask byte is set
            outVec = _mm256_blendv_epi8(outVec, alphaVec, alphaBlendMask);

            // store 32 bytes into target
            uint8_t* dstPtr = targetData.data() + dstByteOffset;
            _mm256_storeu_si256(reinterpret_cast<__m256i*>(dstPtr), outVec);
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
                    targetData[dOff + c] = sourceData[sOff + srcChannel];
                }
            }
        }
    }

}