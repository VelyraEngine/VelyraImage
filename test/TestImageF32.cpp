#include <gtest/gtest.h>
#include <VelyraImage/ImageDefs.hpp>

#include "../src/ImageF32.hpp"

using namespace Velyra;
using namespace Velyra::Image;

class TestImageF32 : public ::testing::Test {
protected:
    void checkRedImage(ImageF32& image) {
        auto pixelPtr = static_cast<float*>(image.getData());
        for (Size i = 0; i < image.getPixelCount(); i += 3) {
            EXPECT_NEAR(pixelPtr[i], 1.0f, 0.01f);  // R
            EXPECT_FLOAT_EQ(pixelPtr[i + 1], 0.0f); // G
            EXPECT_FLOAT_EQ(pixelPtr[i + 2], 0.0f); // B
        }
    }
};

TEST_F(TestImageF32, ReadImageFromFile) {
    /*
     * Load a simple 100x100 red image in F32 RGB format and verify its properties and pixel data.
     */
    const fs::path testImagePath = fs::current_path() / "Resources" / "Red-100x100-F32-RGB.hdr";
    ImageLoadDesc desc;
    desc.fileName = testImagePath;
    desc.flipOnLoad = true;
    ImageF32 image(desc);
    EXPECT_EQ(image.getWidth(), 100);
    EXPECT_EQ(image.getHeight(), 100);
    EXPECT_EQ(image.getChannelFormat(), VL_CHANNEL_RGB);
    EXPECT_EQ(image.getDataType(), VL_FLOAT32);
    EXPECT_EQ(image.getPixelCount(), 100 * 100);
    EXPECT_EQ(image.getCount(), 100 * 100 * 3);
    EXPECT_EQ(image.getSize(), 100 * 100 * 3 * sizeof(float));

    checkRedImage(image);
}

TEST_F(TestImageF32, CreateImageFromData) {
    /*
     * Create a 50x50 red image in F32 RGB format from raw data and verify its properties and pixel data.
     */
    constexpr U32 width = 50;
    constexpr U32 height = 50;
    std::vector<float> imageData(width * height * 3, 0.0f);
    for (Size i = 0; i < imageData.size(); i += 3) {
        imageData[i] = 1.0f;     // R
        imageData[i + 1] = 0.0f; // G
        imageData[i + 2] = 0.0f; // B
    }

    ImageF32Desc desc;
    desc.width = width;
    desc.height = height;
    desc.format = VL_CHANNEL_RGB;
    desc.data = imageData.data();

    ImageF32 image(desc);

    EXPECT_EQ(image.getWidth(), width);
    EXPECT_EQ(image.getHeight(), height);
    EXPECT_EQ(image.getChannelFormat(), VL_CHANNEL_RGB);
    EXPECT_EQ(image.getDataType(), VL_FLOAT32);
    EXPECT_EQ(image.getPixelCount(), width * height);
    EXPECT_EQ(image.getCount(), width * height * 3);
    EXPECT_EQ(image.getSize(), width * height * 3 * sizeof(float));

    checkRedImage(image);
}

TEST_F(TestImageF32, WriteImageToFile) {
    /*
     * Create a 20x20 red image in F32 RGB format, write it to a file, then read it back and verify its properties and pixel data.
     */
    constexpr U32 width = 20;
    constexpr U32 height = 20;
    std::vector<float> imageData(width * height * 3, 0.0f);
    for (Size i = 0; i < imageData.size(); i += 3) {
        imageData[i] = 1.0f;        // R
        imageData[i + 1] = 0.0f;    // G
        imageData[i + 2] = 0.0f;    // B
    }

    ImageF32Desc desc;
    desc.width = width;
    desc.height = height;
    desc.format = VL_CHANNEL_RGB;
    desc.data = imageData.data();

    ImageF32 image(desc);

    const fs::path outputImagePath = fs::current_path() / "TestImageF32-WriteImageToFile-Red-20x20-F32-RGB.png";
    ImageWriteDesc writeDesc;
    writeDesc.fileName = outputImagePath;
    writeDesc.fileType = VL_IMAGE_HDR;
    writeDesc.flipOnWrite = true;

    image.write(writeDesc);

    // Now read back the written image and verify
    ImageLoadDesc loadDesc;
    loadDesc.fileName = outputImagePath;
    loadDesc.flipOnLoad = true;

    ImageF32 loadedImage(loadDesc);
    EXPECT_EQ(loadedImage.getWidth(), width);
    EXPECT_EQ(loadedImage.getHeight(), height);
    EXPECT_EQ(loadedImage.getChannelFormat(), VL_CHANNEL_RGB);
    EXPECT_EQ(loadedImage.getDataType(), VL_FLOAT32);
    EXPECT_EQ(loadedImage.getPixelCount(), width * height);
    EXPECT_EQ(loadedImage.getCount(), width * height * 3);
    EXPECT_EQ(loadedImage.getSize(), width * height * 3 * sizeof(float));
    checkRedImage(loadedImage);
}

TEST_F(TestImageF32, ResizeImage) {
    /*
     * Create a 30x30 red image in F32 RGB format, resize it to 60x60, and verify the new properties and pixel data.
     */
    constexpr U32 originalWidth = 30;
    constexpr U32 originalHeight = 30;
    std::vector<float> imageData(originalWidth * originalHeight * 3, 0.0f);
    for (Size i = 0; i < imageData.size(); i += 3) {
        imageData[i] = 1.0f;     // R
        imageData[i + 1] = 0.0f; // G
        imageData[i + 2] = 0.0f; // B
    }

    ImageF32Desc desc;
    desc.width = originalWidth;
    desc.height = originalHeight;
    desc.format = VL_CHANNEL_RGB;
    desc.data = imageData.data();

    ImageF32 image(desc);

    constexpr U32 newWidth = originalWidth * 2;
    constexpr U32 newHeight = originalHeight * 2;
    const UP<IImage> resizedImagePtr = image.resize(newWidth, newHeight);
    const auto resizedImage = dynamic_cast<ImageF32*>(resizedImagePtr.get());

    EXPECT_EQ(resizedImage->getWidth(), newWidth);
    EXPECT_EQ(resizedImage->getHeight(), newHeight);
    EXPECT_EQ(resizedImage->getChannelFormat(), VL_CHANNEL_RGB);
    EXPECT_EQ(resizedImage->getDataType(), VL_FLOAT32);
    EXPECT_EQ(resizedImage->getPixelCount(), newWidth * newHeight);
    EXPECT_EQ(resizedImage->getCount(), newWidth * newHeight * 3);
    EXPECT_EQ(resizedImage->getSize(), newWidth * newHeight * 3 * sizeof(float));

    checkRedImage(*resizedImage);
}
