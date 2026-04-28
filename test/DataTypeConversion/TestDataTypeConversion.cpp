#include <gtest/gtest.h>

#include <VelyraImage/ImageFactory.hpp>
#include <VelyraUtils/Math.hpp>
#include <VelyraUtils/TypeTraits.hpp>

using namespace Velyra;
using namespace Velyra::Image;

template<typename T, VL_SIMD_MODE M, VL_TYPE TargetType>
struct ConversionCase {
    using SourceType = T;
    static constexpr VL_SIMD_MODE SimdMode = M;
    static constexpr VL_TYPE TargetDataType = TargetType;
};

template<typename ConversionCase>
class TestDataTypeConversion : public ::testing::Test {
public:
    using SourceType = ConversionCase::SourceType;
    static constexpr VL_SIMD_MODE SimdMode = ConversionCase::SimdMode;
    static constexpr VL_TYPE TargetDataType = ConversionCase::TargetDataType;
    static constexpr Size m_Width = 200;
    static constexpr Size m_Height = 200;

public:
    template<typename T>
    std::vector<T> createImageData() {
        if constexpr (std::is_same_v<T, float>) {
            std::vector<T> data(m_Width * m_Height * 4);
            for (Size i = 0; i < data.size(); i += 4) {
                data[i] = 1.0f; // R
                data[i + 1] = 0.0f; // G
                data[i + 2] = 1.0f; // B
                data[i + 3] = 1.0f; // A
            }
            return data;
        }
        else if constexpr (std::is_same_v<T, U8>) {
            std::vector<T> data(m_Width * m_Height * 4);
            for (Size i = 0; i < data.size(); i += 4) {
                data[i] = 255; // R
                data[i + 1] = 0; // G
                data[i + 2] = 255; // B
                data[i + 3] = 255; // A
            }
            return data;
        }
        else {
            VL_NOT_IMPLEMENTED();
        }
    }

    template<typename T>
    UP<IImage> createImage() {
        if constexpr (std::is_same_v<T, float>) {
            ImageF32Desc desc;
            desc.width = m_Width;
            desc.height = m_Height;
            desc.format = VL_CHANNEL_RGBA;
            auto data = createImageData<float>();
            desc.data = data.data();
            return ImageFactory::createImageF32(desc);
        }
        else if constexpr (std::is_same_v<T, U8>) {
            ImageU8Desc desc;
            desc.width = m_Width;
            desc.height = m_Height;
            desc.format = VL_CHANNEL_RGBA;
            auto data = createImageData<U8>();
            desc.data = data.data();
            return ImageFactory::createImageU8(desc);
        }
        else {
            VL_NOT_IMPLEMENTED();
        }
    }
};

using ConversionCases = ::testing::Types<
    ConversionCase<float, VL_SIMD_SCALAR, VL_UINT8>,
    ConversionCase<float, VL_SIMD_SCALAR, VL_FLOAT32>,
    ConversionCase<float, VL_SIMD_AVX2, VL_UINT8>,
    ConversionCase<float, VL_SIMD_AVX2, VL_UINT8>,
    ConversionCase<U8, VL_SIMD_SCALAR, VL_UINT8>,
    ConversionCase<U8, VL_SIMD_SCALAR, VL_FLOAT32>,
    ConversionCase<U8, VL_SIMD_AVX2, VL_UINT8>,
    ConversionCase<U8, VL_SIMD_AVX2, VL_UINT8>
>;

TYPED_TEST_SUITE(TestDataTypeConversion, ConversionCases);

TYPED_TEST(TestDataTypeConversion, ConvertDataType) {
    const auto sourceImage = this->template createImage<typename TestFixture::SourceType>();

    TranslationDesc desc;
    desc.targetType = TestFixture::TargetDataType;
    desc.simdMode = TestFixture::SimdMode;
    auto targetImage = sourceImage->translateDataType(desc);

    using TargetCppType = Utils::VLTypeToCpp<TestFixture::TargetDataType>::type;
    auto* targetData = static_cast<TargetCppType*>(targetImage->getData());
    auto expectedData = this->template createImageData<TargetCppType>();

    for (Size i = 0; i < targetImage->getCount(); ++i) {
        if constexpr (std::is_floating_point_v<TargetCppType>) {
            EXPECT_NEAR(targetData[i], expectedData[i], 0.01f);
        }
        else {
            EXPECT_EQ(targetData[i], expectedData[i]);
        }
    }
}