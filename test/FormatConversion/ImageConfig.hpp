#pragma once

#include "../../src/ImageUI8.hpp"
#include "../../src/ImageF32.hpp"

using namespace Velyra;
using namespace Velyra::Image;

template<typename IMAGE_TYPE, VL_SIMD_MODE simdMode>
struct ImageConfig;

template<>
struct ImageConfig<ImageUI8, VL_SIMD_SCALAR> {
    using PixelType = U8;
    using ImageType = ImageUI8;
    using ImageDesc = ImageUI8Desc;
    static constexpr PixelType r = 10;
    static constexpr PixelType g = 20;
    static constexpr PixelType b = 30;
    static constexpr PixelType a = 40;
    static constexpr PixelType fillMin = 0;
    static constexpr PixelType fillMax = 255;
    static constexpr VL_SIMD_MODE simdMode = VL_SIMD_SCALAR;

    static void expectEqual(const PixelType expected, const PixelType actual) {
        EXPECT_EQ(expected, actual);
    }
};

template<>
struct ImageConfig<ImageUI8, VL_SIMD_AVX2> {
    using PixelType = U8;
    using ImageType = ImageUI8;
    using ImageDesc = ImageUI8Desc;
    static constexpr PixelType r = 10;
    static constexpr PixelType g = 20;
    static constexpr PixelType b = 30;
    static constexpr PixelType a = 40;
    static constexpr PixelType fillMin = 0;
    static constexpr PixelType fillMax = 255;
    static constexpr VL_SIMD_MODE simdMode = VL_SIMD_AVX2;

    static void expectEqual(const PixelType expected, const PixelType actual) {
        EXPECT_EQ(expected, actual);
    }
};

template<>
struct ImageConfig<ImageF32, VL_SIMD_SCALAR> {
    using PixelType = float;
    using ImageType = ImageF32;
    using ImageDesc = ImageF32Desc;
    static constexpr PixelType r = 0.1f;
    static constexpr PixelType g = 0.2f;
    static constexpr PixelType b = 0.3f;
    static constexpr PixelType a = 0.4f;
    static constexpr PixelType fillMin = 0.0f;
    static constexpr PixelType fillMax = 1.0f;
    static constexpr VL_SIMD_MODE simdMode = VL_SIMD_SCALAR;

    static void expectEqual(const PixelType expected, const PixelType actual) {
        EXPECT_FLOAT_EQ(expected, actual);
    }
};