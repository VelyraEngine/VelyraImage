#include <gtest/gtest.h>

#include "../../src/FormatConversion/FormatConversion.hpp"

using namespace Velyra;
using namespace Velyra::Image;

class TestFormatConversion : public ::testing::Test {
};

TEST_F(TestFormatConversion, TestDefineSwizzle) {
    {
        std::vector<int> swizzle = defineSwizzle(VL_CHANNEL_RGB, VL_CHANNEL_RGBA);
        std::vector<int> expected = {0, 1, 2, -1};
        EXPECT_EQ(swizzle, expected);
    }
    {
        std::vector<int> swizzle = defineSwizzle(VL_CHANNEL_RGBA, VL_CHANNEL_BGRA);
        std::vector<int> expected = {2, 1, 0, 3};
        EXPECT_EQ(swizzle, expected);
    }
    {
        std::vector<int> swizzle = defineSwizzle(VL_CHANNEL_BGRA, VL_CHANNEL_RGB);
        std::vector<int> expected = {2, 1, 0};
        EXPECT_EQ(swizzle, expected);
    }
    {
        std::vector<int> swizzle = defineSwizzle(VL_CHANNEL_R, VL_CHANNEL_RGBA);
        std::vector<int> expected = {0, -1, -1, -1};
        EXPECT_EQ(swizzle, expected);
    }
}

TEST_F(TestFormatConversion, TestGetFillValue) {
    {
        float fillMin = getFillValue<float>(VL_FILL_MIN);
        EXPECT_FLOAT_EQ(fillMin, 0.0f);
    }
    {
        float fillMax = getFillValue<float>(VL_FILL_MAX);
        EXPECT_FLOAT_EQ(fillMax, 1.0f);
    }
    {
        uint8_t fillMin = getFillValue<U8>(VL_FILL_MIN);
        EXPECT_EQ(fillMin, 0);
    }
    {
        uint8_t fillMax = getFillValue<U8>(VL_FILL_MAX);
        EXPECT_EQ(fillMax, 255);
    }
}