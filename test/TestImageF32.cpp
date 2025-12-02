#include <gtest/gtest.h>
#include <VelyraImage/ImageDefs.hpp>

#include "../src/ImageF32.hpp"

using namespace Velyra;
using namespace Velyra::Image;

class TestImageF32 : public ::testing::Test {
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

}
