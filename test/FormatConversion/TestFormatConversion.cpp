#include <gtest/gtest.h>

#include "../../src/FormatConversion/FormatConversion.hpp"
#include "ImageConfig.hpp"

template<typename IMAGE_CONFIG>
class TestFormatConversion : public ::testing::Test {
public:
    using ImageType = typename IMAGE_CONFIG::ImageType;
    using PixelType = typename IMAGE_CONFIG::PixelType;
    using ImageDesc = typename IMAGE_CONFIG::ImageDesc;

    static constexpr PixelType r = IMAGE_CONFIG::r;
    static constexpr PixelType g = IMAGE_CONFIG::g;
    static constexpr PixelType b = IMAGE_CONFIG::b;
    static constexpr PixelType a = IMAGE_CONFIG::a;
    static constexpr PixelType fillMin = IMAGE_CONFIG::fillMin;
    static constexpr PixelType fillMax = IMAGE_CONFIG::fillMax;

protected:
    static ImageType createImage(const U32 width, const U32 height, const VL_CHANNEL_FORMAT format) {
        ImageDesc desc;
        desc.width = width;
        desc.height = height;
        desc.format = format;
        ImageType image(desc);
        auto* data = static_cast<PixelType*>(image.getData());
        const U32 channelCount = getChannelCountFromFormat(format);
        for (U32 y = 0; y < height; ++y) {
            for (U32 x = 0; x < width; ++x) {
                U32 index = (y * width + x) * channelCount;
                if (format == VL_CHANNEL_R) {
                    data[index + 0] = r;
                }
                else if (format == VL_CHANNEL_RG) {
                    data[index + 0] = r;
                    data[index + 1] = g;
                }
                else if (format == VL_CHANNEL_RGB) {
                    data[index + 0] = r;
                    data[index + 1] = g;
                    data[index + 2] = b;
                }
                else if (format == VL_CHANNEL_RGBA) {
                    data[index + 0] = r;
                    data[index + 1] = g;
                    data[index + 2] = b;
                    data[index + 3] = a;
                }
                else if (format == VL_CHANNEL_BGR) {
                    data[index + 0] = b;
                    data[index + 1] = g;
                    data[index + 2] = r;
                }
                else if (format == VL_CHANNEL_BGRA) {
                    data[index + 0] = b;
                    data[index + 1] = g;
                    data[index + 2] = r;
                    data[index + 3] = a;
                }
            }
        }
        return image;
    }
};

using TestTypes = ::testing::Types<ImageConfig<ImageUI8>, ImageConfig<ImageF32>>;
TYPED_TEST_SUITE(TestFormatConversion, TestTypes);

TYPED_TEST(TestFormatConversion, DefineSwizzle) {
    {
        const std::vector<int> swizzle = defineSwizzle(VL_CHANNEL_RGB, VL_CHANNEL_RGBA);
        const std::vector<int> expected = {0, 1, 2, -1};
        EXPECT_EQ(swizzle, expected);
    }
    {
        const std::vector<int> swizzle = defineSwizzle(VL_CHANNEL_RGBA, VL_CHANNEL_BGRA);
        const std::vector<int> expected = {2, 1, 0, 3};
        EXPECT_EQ(swizzle, expected);
    }
    {
        const std::vector<int> swizzle = defineSwizzle(VL_CHANNEL_BGRA, VL_CHANNEL_RGB);
        const std::vector<int> expected = {2, 1, 0};
        EXPECT_EQ(swizzle, expected);
    }
    {
        const std::vector<int> swizzle = defineSwizzle(VL_CHANNEL_R, VL_CHANNEL_RGBA);
        const std::vector<int> expected = {0, -1, -1, -1};
        EXPECT_EQ(swizzle, expected);
    }
}

TYPED_TEST(TestFormatConversion, GetFillValue) {
    using C = TypeParam;

    const auto actualFillMin = getFillValue<typename C::PixelType>(VL_FILL_MIN);
    C::expectEqual(actualFillMin, C::fillMin);

    const auto actualFillMax = getFillValue<typename C::PixelType>(VL_FILL_MAX);
    C::expectEqual(actualFillMax, C::fillMax);
}

TYPED_TEST(TestFormatConversion, RGB_2_RGBA) {
    using C = TypeParam;

    const typename C::ImageType sourceImage = this->createImage(20, 20, VL_CHANNEL_RGB);
    FormatConversionDesc desc;
    desc.fillMode = VL_FILL_MAX;
    desc.targetFormat = VL_CHANNEL_RGBA;
    auto targetImage = sourceImage.convertToFormat(desc);
    EXPECT_EQ(targetImage->getWidth(), sourceImage.getWidth());
    EXPECT_EQ(targetImage->getHeight(), sourceImage.getHeight());
    EXPECT_EQ(targetImage->getChannelFormat(), VL_CHANNEL_RGBA);

    auto* targetData = static_cast<typename C::PixelType*>(targetImage->getData());
    for (Size i = 0; i < targetImage->getCount(); i += 4) {
        C::expectEqual(targetData[i + 0], C::r);  // R
        C::expectEqual(targetData[i + 1], C::g);  // G
        C::expectEqual(targetData[i + 2], C::b);  // B
        C::expectEqual(targetData[i + 3], C::fillMax); // A (filled)
    }
}

TYPED_TEST(TestFormatConversion, RGBA_2_BGRA) {
    using C = TypeParam;

    const typename C::ImageType sourceImage = this->createImage(20, 20, VL_CHANNEL_RGBA);
    FormatConversionDesc desc;
    desc.fillMode = VL_FILL_MAX;
    desc.targetFormat = VL_CHANNEL_BGRA;
    auto targetImage = sourceImage.convertToFormat(desc);
    EXPECT_EQ(targetImage->getWidth(), sourceImage.getWidth());
    EXPECT_EQ(targetImage->getHeight(), sourceImage.getHeight());
    EXPECT_EQ(targetImage->getChannelFormat(), VL_CHANNEL_BGRA);

    auto* targetData = static_cast<typename C::PixelType*>(targetImage->getData());
    for (Size i = 0; i < targetImage->getCount(); i += 4) {
        C::expectEqual(targetData[i + 0], C::b);  // B
        C::expectEqual(targetData[i + 1], C::g);  // G
        C::expectEqual(targetData[i + 2], C::r);  // R
        C::expectEqual(targetData[i + 3], C::a);  // A
    }
}

TYPED_TEST(TestFormatConversion, BGRA_2_RGB) {
    using C = TypeParam;

    const typename C::ImageType sourceImage = this->createImage(20, 20, VL_CHANNEL_BGRA);
    FormatConversionDesc desc;
    desc.fillMode = VL_FILL_MAX;
    desc.targetFormat = VL_CHANNEL_RGB;
    auto targetImage = sourceImage.convertToFormat(desc);
    EXPECT_EQ(targetImage->getWidth(), sourceImage.getWidth());
    EXPECT_EQ(targetImage->getHeight(), sourceImage.getHeight());
    EXPECT_EQ(targetImage->getChannelFormat(), VL_CHANNEL_RGB);

    auto* targetData = static_cast<typename C::PixelType*>(targetImage->getData());
    for (Size i = 0; i < targetImage->getCount(); i += 3) {
        C::expectEqual(targetData[i + 0], C::r);  // R
        C::expectEqual(targetData[i + 1], C::g);  // G
        C::expectEqual(targetData[i + 2], C::b);  // B
    }
}

TYPED_TEST(TestFormatConversion, R_2_BGRA) {
    using C = TypeParam;

    const typename C::ImageType sourceImage = this->createImage(20, 20, VL_CHANNEL_R);
    FormatConversionDesc desc;
    desc.fillMode = VL_FILL_MIN;
    desc.targetFormat = VL_CHANNEL_BGRA;
    auto targetImage = sourceImage.convertToFormat(desc);
    EXPECT_EQ(targetImage->getWidth(), sourceImage.getWidth());
    EXPECT_EQ(targetImage->getHeight(), sourceImage.getHeight());
    EXPECT_EQ(targetImage->getChannelFormat(), VL_CHANNEL_BGRA);

    auto* targetData = static_cast<typename C::PixelType*>(targetImage->getData());
    for (Size i = 0; i < targetImage->getCount(); i += 4) {
        C::expectEqual(targetData[i + 0], C::fillMin);  // B (filled)
        C::expectEqual(targetData[i + 1], C::fillMin);  // G (filled)
        C::expectEqual(targetData[i + 2], C::r);  // R
        C::expectEqual(targetData[i + 3], C::fillMin);  // A (filled)
    }
}

