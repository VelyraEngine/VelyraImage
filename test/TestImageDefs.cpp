#include <gtest/gtest.h>
#include <VelyraImage/ImageDefs.hpp>

using namespace Velyra;
using namespace Velyra::Image;


class TestImageDefs : public ::testing::Test {
};

TEST_F(TestImageDefs, TestGetChannelCountFromFormat) {
    EXPECT_EQ(getChannelCountFromFormat(VL_CHANNEL_R), 1);
    EXPECT_EQ(getChannelCountFromFormat(VL_CHANNEL_RG), 2);
    EXPECT_EQ(getChannelCountFromFormat(VL_CHANNEL_RGB), 3);
    EXPECT_EQ(getChannelCountFromFormat(VL_CHANNEL_RGBA), 4);
    EXPECT_EQ(getChannelCountFromFormat(VL_CHANNEL_BGR), 3);
    EXPECT_EQ(getChannelCountFromFormat(VL_CHANNEL_BGRA), 4);
    EXPECT_EQ(getChannelCountFromFormat(VL_CHANNEL_UNKNOWN), 0);
}

TEST_F(TestImageDefs, TestGetChannelFormatFromCount) {
    EXPECT_EQ(getChannelFormatFromCount(1), VL_CHANNEL_R);
    EXPECT_EQ(getChannelFormatFromCount(2), VL_CHANNEL_RG);
    EXPECT_EQ(getChannelFormatFromCount(3), VL_CHANNEL_RGB);
    EXPECT_EQ(getChannelFormatFromCount(4), VL_CHANNEL_RGBA);
    EXPECT_EQ(getChannelFormatFromCount(0), VL_CHANNEL_UNKNOWN);
    EXPECT_EQ(getChannelFormatFromCount(5), VL_CHANNEL_UNKNOWN);
}


