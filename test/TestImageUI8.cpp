#include <gtest/gtest.h>
#include <VelyraImage/ImageDefs.hpp>

#include "../src/ImageUI8.hpp"

using namespace Velyra;
using namespace Velyra::Image;


class TestImageUI8 : public ::testing::Test {
protected:

    void checkRedImage(ImageUI8& image) {
        auto pixelPtr = static_cast<U8*>(image.getData());
        for (Size i = 0; i < image.getPixelCount(); i += 3) {
            EXPECT_EQ(pixelPtr[i], 255);     // R
            EXPECT_EQ(pixelPtr[i + 1], 0);   // G
            EXPECT_EQ(pixelPtr[i + 2], 0);   // B
        }
    }
};

TEST_F(TestImageUI8, ReadImageFromFile) {
    /*
     * Load a simple 100x100 red image in UI8 RGB format and verify its properties and pixel data.
     */
    const fs::path testImagePath = fs::current_path() / "Resources" / "Red-100x100-UI8-RGB.png";
    ImageLoadDesc desc;
    desc.fileName = testImagePath;
    desc.flipOnLoad = true;
    ImageUI8 image(desc);
    EXPECT_EQ(image.getWidth(), 100);
    EXPECT_EQ(image.getHeight(), 100);
    EXPECT_EQ(image.getChannelFormat(), VL_CHANNEL_RGB);
    EXPECT_EQ(image.getDataType(), VL_UINT8);
    EXPECT_EQ(image.getPixelCount(), 100 * 100);
    EXPECT_EQ(image.getCount(), 100 * 100 * 3);
    EXPECT_EQ(image.getSize(), 100 * 100 * 3 * sizeof(U8));
    checkRedImage(image);
}

TEST_F(TestImageUI8, CreateImageFromData) {
    /*
     * Create a 50x50 red image in UI8 RGB format from raw data and verify its properties and pixel data.
     */
    constexpr U32 width = 50;
    constexpr U32 height = 50;
    std::vector<U8> imageData(width * height * 3, 0);
    for (Size i = 0; i < imageData.size(); i += 3) {
        imageData[i] = 255;     // R
        imageData[i + 1] = 0;   // G
        imageData[i + 2] = 0;   // B
    }

    ImageUI8Desc desc;
    desc.width = width;
    desc.height = height;
    desc.format = VL_CHANNEL_RGB;
    desc.data = imageData.data();

    ImageUI8 image(desc);
    EXPECT_EQ(image.getWidth(), width);
    EXPECT_EQ(image.getHeight(), height);
    EXPECT_EQ(image.getChannelFormat(), VL_CHANNEL_RGB);
    EXPECT_EQ(image.getDataType(), VL_UINT8);
    EXPECT_EQ(image.getPixelCount(), width * height);
    EXPECT_EQ(image.getCount(), width * height * 3);
    EXPECT_EQ(image.getSize(), width * height * 3 * sizeof(U8));
    checkRedImage(image);
}

TEST_F(TestImageUI8, WriteImageToFile) {
    /*
     * Create a 20x20 red image in UI8 RGB format, write it to a file, then read it back and verify its properties and pixel data.
     */
    constexpr U32 width = 20;
    constexpr U32 height = 20;
    std::vector<U8> imageData(width * height * 3, 0);
    for (Size i = 0; i < imageData.size(); i += 3) {
        imageData[i] = 255;     // R
        imageData[i + 1] = 0;   // G
        imageData[i + 2] = 0;   // B
    }

    ImageUI8Desc desc;
    desc.width = width;
    desc.height = height;
    desc.format = VL_CHANNEL_RGB;
    desc.data = imageData.data();

    ImageUI8 image(desc);

    const fs::path outputImagePath = fs::current_path() / "TestImageUI8-WriteImageToFile-Red-20x20-UI8-RGB.png";
    ImageWriteDesc writeDesc;
    writeDesc.fileName = outputImagePath;
    writeDesc.fileType = VL_IMAGE_PNG;
    writeDesc.flipOnWrite = true;

    image.write(writeDesc);

    // Now read back the written image and verify
    ImageLoadDesc loadDesc;
    loadDesc.fileName = outputImagePath;
    loadDesc.flipOnLoad = true;

    ImageUI8 loadedImage(loadDesc);
    EXPECT_EQ(loadedImage.getWidth(), width);
    EXPECT_EQ(loadedImage.getHeight(), height);
    EXPECT_EQ(loadedImage.getChannelFormat(), VL_CHANNEL_RGB);
    EXPECT_EQ(loadedImage.getDataType(), VL_UINT8);
    EXPECT_EQ(loadedImage.getPixelCount(), width * height);
    EXPECT_EQ(loadedImage.getCount(), width * height * 3);
    EXPECT_EQ(loadedImage.getSize(), width * height * 3 * sizeof(U8));
    checkRedImage(loadedImage);
}

TEST_F(TestImageUI8, ResizeImage) {
    /*
     * Create a 30x30 red image in UI8 RGB format, resize it to 60x60, and verify the new properties and pixel data.
     */
    constexpr U32 originalWidth = 30;
    constexpr U32 originalHeight = 30;
    std::vector<U8> imageData(originalWidth * originalHeight * 3, 0);
    for (Size i = 0; i < imageData.size(); i += 3) {
        imageData[i] = 255;     // R
        imageData[i + 1] = 0;   // G
        imageData[i + 2] = 0;   // B
    }

    ImageUI8Desc desc;
    desc.width = originalWidth;
    desc.height = originalHeight;
    desc.format = VL_CHANNEL_RGB;
    desc.data = imageData.data();

    ImageUI8 image(desc);

    constexpr U32 newWidth = 60;
    constexpr U32 newHeight = 60;
    const UP<IImage> resizedImagePtr = image.resize(newWidth, newHeight);
    auto resizedImage = dynamic_cast<ImageUI8*>(resizedImagePtr.get());

    ASSERT_NE(resizedImage, nullptr);
    EXPECT_EQ(resizedImage->getWidth(), newWidth);
    EXPECT_EQ(resizedImage->getHeight(), newHeight);
    EXPECT_EQ(resizedImage->getChannelFormat(), VL_CHANNEL_RGB);
    EXPECT_EQ(resizedImage->getDataType(), VL_UINT8);
    EXPECT_EQ(resizedImage->getPixelCount(), newWidth * newHeight);
    EXPECT_EQ(resizedImage->getCount(), newWidth * newHeight * 3);
    EXPECT_EQ(resizedImage->getSize(), newWidth * newHeight * 3 * sizeof(U8));
    checkRedImage(*resizedImage);
}