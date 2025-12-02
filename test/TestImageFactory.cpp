#include <gtest/gtest.h>
#include <VelyraImage/ImageFactory.hpp>

using namespace Velyra;
using namespace Velyra::Image;

class TestImageFactory : public ::testing::Test {
};

TEST_F(TestImageFactory, TestCreateImageUI8FromFile) {
    const fs::path testImagePath = fs::current_path() / "Resources" / "Red-100x100-UI8-RGB.png";
    ImageLoadDesc desc;
    desc.fileName = testImagePath;
    desc.flipOnLoad = true;
    UP<IImage> image = ImageFactory::createImage(desc);
    ASSERT_NE(image, nullptr);
    EXPECT_EQ(image->getWidth(), 100);
    EXPECT_EQ(image->getHeight(), 100);
    EXPECT_EQ(image->getChannelFormat(), VL_CHANNEL_RGB);
    EXPECT_EQ(image->getDataType(), VL_UINT8);
}
