#pragma once

#include <VelyraImage/ImageDefs.hpp>
#include <VelyraUtils/CpuFeatures.hpp>

namespace Velyra::Image {

    std::vector<int> defineSwizzle(VL_CHANNEL_FORMAT sourceFormat, VL_CHANNEL_FORMAT targetFormat);

    void convertFormat_U8_AVX2(VL_CHANNEL_FORMAT sourceFormat, const std::vector<U8>& sourceData,
        VL_CHANNEL_FORMAT targetFormat, std::vector<U8>& targetData, VL_FORMAT_CONVERSION_FILL fillMode);

    template<typename T>
    T getFillValue(const VL_FORMAT_CONVERSION_FILL fillMode) {
        if constexpr (std::is_same_v<T, float>) {
            switch (fillMode) {
                case VL_FILL_MIN: return 0.0f;
                case VL_FILL_MAX: return 1.0f;
                default: return 1.0f;
            }
        }
        else if constexpr (std::is_integral_v<T>) {
            switch (fillMode) {
                case VL_FILL_MIN: return std::numeric_limits<T>::min();
                case VL_FILL_MAX: return std::numeric_limits<T>::max();
                default: return std::numeric_limits<T>::max();
            }
        }
    }

    template<typename T>
    void convertFormat_Scalar(const VL_CHANNEL_FORMAT sourceFormat, const std::vector<T>& sourceData,
        const VL_CHANNEL_FORMAT targetFormat, std::vector<T>& targetData, const VL_FORMAT_CONVERSION_FILL fillMode) {

        const std::vector<int> swizzle = defineSwizzle(sourceFormat, targetFormat);
        const T fillValue = getFillValue<T>(fillMode);
        const U32 sourceChannelCount = getChannelCountFromFormat(sourceFormat);

        const Size pixelCount = sourceData.size() / getChannelCountFromFormat(sourceFormat);
        for (Size i = 0; i < pixelCount; ++i) {
            for (Size j = 0; j < swizzle.size(); ++j) {
                // Is the channel present in the source format?
                if (swizzle[j] != -1) {
                    targetData[i * swizzle.size() + j] = sourceData[i * sourceChannelCount + swizzle[j]];
                }
                else {
                    targetData[i * swizzle.size() + j] = fillValue;
                }
            }
        }
    }

    template<typename T>
    void convertFormat(const VL_CHANNEL_FORMAT sourceFormat, const std::vector<T>& sourceData,
        std::vector<T>& targetData, const FormatConversionDesc& desc) {

        // Dispatch based on CPU features and data type
        static Utils::CpuFeatures cpuFeatures = Utils::detectCpuFeatures();
        if constexpr (std::is_same_v<T, U8>) {
            if (cpuFeatures.avx2 && desc.simdMode == VL_SIMD_AVX2) {
                convertFormat_U8_AVX2(sourceFormat, sourceData, desc.targetFormat, targetData, desc.fillMode);
                return;
            }
        }
        convertFormat_Scalar<T>(sourceFormat, sourceData, desc.targetFormat, targetData, desc.fillMode);
    }
}