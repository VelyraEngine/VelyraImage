#include <gtest/gtest.h>

#include <VelyraImage/ImageFactory.hpp>
#include <VelyraUtils/Math.hpp>
#include <VelyraUtils/TypeTraits.hpp>
#include <VelyraUtils/DevUtils/PrettyTypeFormatter.hpp>

#include "../TypeUtils.hpp"

using namespace Velyra;
using namespace Velyra::Image;
using namespace Velyra::Test;

template<VL_TYPE TargetType>
struct TargetTypeWrapper {
    static constexpr VL_TYPE Type = TargetType;
};

using TargetDataTypes = Utils::TypeList<
    TargetTypeWrapper<VL_UINT8>,
    TargetTypeWrapper<VL_FLOAT32>
>;

// Calculate cartesian product of ImageDataTypes, SimdModes, and TargetDataTypes
using Combinations = Utils::CartesianProduct<ImageDataTypes, SimdModes, TargetDataTypes>;
using ConversionCases = Utils::ToGTestTypes<Combinations::type>::type;

template<typename Case>
class TestDataTypeConversion : public ::testing::Test {
public:
    using SourceType = std::tuple_element_t<0, Case>;
    static constexpr VL_SIMD_MODE SimdMode = std::tuple_element_t<1, Case>::SimdMode;
    static constexpr VL_TYPE TargetDataType = std::tuple_element_t<2, Case>::Type;
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