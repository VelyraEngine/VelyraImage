#pragma once

#include <VelyraImage/ImageDefs.hpp>
#include <vector>

namespace Velyra::Image::TranslateDataType {

    /**
     * @brief Scalar conversion from UI8 to F32
     * Converts U8 values [0, 255] to float values [0.0, 1.0]
     */
    void translateDataType_Scalar(const std::vector<U8>& source, std::vector<float>& destination);

    /**
     * @brief Scalar conversion from F32 to UI8
     * Converts float values [0.0, 1.0] to U8 values [0, 255]
     * Values outside [0.0, 1.0] are clamped
     */
    void translateDataType_Scalar(const std::vector<float>& source, std::vector<U8>& destination);

    /**
     * @brief AVX2-optimized conversion from UI8 to F32
     * Converts U8 values [0, 255] to float values [0.0, 1.0]
     */
    void translateDataType_AVX2(const std::vector<U8>& source, std::vector<float>& destination);

    /**
     * @brief AVX2-optimized conversion from F32 to UI8
     * Converts float values [0.0, 1.0] to U8 values [0, 255]
     * Values outside [0.0, 1.0] are clamped
     */
    void translateDataType_AVX2(const std::vector<float>& source, std::vector<U8>& destination);

    template<typename SrcType, typename DstType>
    void translateDataType(const std::vector<SrcType>& source,
                          std::vector<DstType>& destination,
                          const TranslationDesc& desc) {
        
        if constexpr (std::is_same_v<SrcType, DstType>) {
            destination = source;
            return;
        }
        const Utils::CpuFeatures cpuFeatures = Utils::detectCpuFeatures();
        switch (desc.simdMode) {
            case VL_SIMD_AVX2: {
                if (cpuFeatures.avx2) {
                    translateDataType_AVX2(source, destination);
                    return;
                }
                SPDLOG_WARN("AVX2 not supported on this CPU, falling back to scalar translation");
                break;
            }
            default: {
                break;
            }
        }
        translateDataType_Scalar(source, destination);
    }

}
