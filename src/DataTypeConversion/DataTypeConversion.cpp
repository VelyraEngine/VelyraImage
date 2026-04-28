#include "../Pch.hpp"

#include "DataTypeConversion.hpp"

#include <algorithm>

namespace Velyra::Image::TranslateDataType {

    void translateDataType_Scalar(const std::vector<U8>& source, std::vector<float>& destination) {
        const Size count = source.size();
        
        // Convert U8 [0, 255] to float [0.0, 1.0]
        constexpr float scale = 1.0f / 255.0f;
        
        for (Size i = 0; i < count; ++i) {
            destination[i] = static_cast<float>(source[i]) * scale;
        }
    }

    void translateDataType_Scalar(const std::vector<float>& source, std::vector<U8>& destination) {
        const Size count = source.size();
        
        // Convert float [0.0, 1.0] to U8 [0, 255]
        constexpr float scale = 255.0f;
        
        for (Size i = 0; i < count; ++i) {
            // Clamp to [0.0, 1.0] range, scale to [0, 255], and round
            float value = std::clamp(source[i], 0.0f, 1.0f);
            value = value * scale + 0.5f; // Add 0.5 for rounding
            destination[i] = static_cast<U8>(value);
        }
    }

    void translateDataType_AVX2(const std::vector<U8>& source, std::vector<float>& destination) {
        const Size count = source.size();
        const __m256 scale = _mm256_set1_ps(1.0f / 255.0f);
        
        Size i = 0;
        // Process 8 elements at a time
        for (; i + 8 <= count; i += 8) {
            // Load 8 bytes from source
            // We load into a 64-bit value, then use _mm_loadl_epi64 to get into XMM
            __m128i bytes_128 = _mm_loadl_epi64(reinterpret_cast<const __m128i*>(&source[i]));
            
            // Zero-extend U8 to 32-bit integers
            __m256i ints = _mm256_cvtepu8_epi32(bytes_128);
            
            // Convert 32-bit integers to floats
            __m256 floats = _mm256_cvtepi32_ps(ints);
            
            // Scale from [0, 255] to [0.0, 1.0]
            floats = _mm256_mul_ps(floats, scale);
            
            // Store 8 floats to destination
            _mm256_storeu_ps(&destination[i], floats);
        }
        
        // Scalar tail for remaining elements
        for (; i < count; ++i) {
            destination[i] = static_cast<float>(source[i]) / 255.0f;
        }
    }

    void translateDataType_AVX2(const std::vector<float>& source, std::vector<U8>& destination) {
        const Size count = source.size();
        const __m256 zero = _mm256_setzero_ps();
        const __m256 one = _mm256_set1_ps(1.0f);
        const __m256 scale = _mm256_set1_ps(255.0f);
        const __m256 half = _mm256_set1_ps(0.5f);
        
        Size i = 0;
        // Process 8 elements at a time
        for (; i + 8 <= count; i += 8) {
            // Load 8 floats from source
            __m256 floats = _mm256_loadu_ps(&source[i]);
            
            // Clamp to [0.0, 1.0]
            floats = _mm256_max_ps(floats, zero);
            floats = _mm256_min_ps(floats, one);
            
            // Scale to [0, 255] and add 0.5 for rounding
            floats = _mm256_mul_ps(floats, scale);
            floats = _mm256_add_ps(floats, half);
            
            // Convert to 32-bit integers
            __m256i ints = _mm256_cvtps_epi32(floats);
            
            // Pack 32-bit integers down to 16-bit integers (saturated)
            // This requires some shuffling since AVX2 operates on 128-bit lanes
            // Extract low and high 128-bit lanes
            __m128i low_128 = _mm256_castsi256_si128(ints);           // Lower 4 ints
            __m128i high_128 = _mm256_extracti128_si256(ints, 1);    // Upper 4 ints
            
            // Pack 32-bit to 16-bit (8 int32 -> 8 int16)
            __m128i packed_16 = _mm_packus_epi32(low_128, high_128);
            
            // Pack 16-bit to 8-bit (8 int16 -> 8 int8)
            __m128i packed_8 = _mm_packus_epi16(packed_16, packed_16);
            
            // Store 8 bytes to destination
            _mm_storel_epi64(reinterpret_cast<__m128i*>(&destination[i]), packed_8);
        }
        
        // Scalar tail for remaining elements
        for (; i < count; ++i) {
            float value = std::clamp(source[i], 0.0f, 1.0f);
            value = value * 255.0f + 0.5f;
            destination[i] = static_cast<U8>(value);
        }
    }

}
