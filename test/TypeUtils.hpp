#pragma once

#include <VelyraUtils/DevUtils/CartesianProduct.hpp>
#include <VelyraUtils/Types/Types.hpp>

#include <VelyraImage/ImageDefs.hpp>

namespace Velyra::Test {

    using ImageDataTypes = Utils::TypeList<float, U8>;

    template<VL_SIMD_MODE Mode>
    struct SimdWrapper {
        static constexpr VL_SIMD_MODE SimdMode = Mode;
    };

    using SimdModes = Utils::TypeList<
        SimdWrapper<VL_SIMD_SCALAR>,
        SimdWrapper<VL_SIMD_AVX2>
    >;

}
