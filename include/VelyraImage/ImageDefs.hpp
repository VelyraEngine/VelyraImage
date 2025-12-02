#pragma once

#include <VelyraUtils/Types/Types.hpp>
#include <VelyraUtils/ExportUtils.hpp>
#include <VelyraUtils/VelyraEnum.hpp>
#include <filesystem>

VL_ENUM(VL_IMAGE_TYPE, int,
    VL_IMAGE_NONE      = 0x00,
    VL_IMAGE_PNG       = 0x01,
    VL_IMAGE_JPG       = 0x02,
    VL_IMAGE_BMP       = 0x03,
    VL_IMAGE_HDR       = 0x04
);

VL_ENUM(VL_CHANNEL_FORMAT, int,
    VL_CHANNEL_UNKNOWN  = 0x00,
    VL_CHANNEL_R        = 0x01,
    VL_CHANNEL_RG       = 0x02,
    VL_CHANNEL_RGB      = 0x03,
    VL_CHANNEL_RGBA     = 0x04,
    VL_CHANNEL_BGR      = 0x05,
    VL_CHANNEL_BGRA     = 0x06
);

VL_ENUM(VL_FORMAT_CONVERSION_FILL, int,
    VL_FILL_MIN = 0x00,
    VL_FILL_MAX = 0x01
    );

namespace Velyra::Image {

    namespace fs = std::filesystem;

    /**
     * @brief Gets the number of channels from a VL_CHANNEL_FORMAT.
     * @param format The channel format.
     * @return Number of channels (1-4), or 0 if unknown format.
     */
    U32 VL_API getChannelCountFromFormat(VL_CHANNEL_FORMAT format);

    /**
     * @brief Converts a channel count to a VL_CHANNEL_FORMAT.
     *        In case of 3 or 4, channels are assumed to be in RGB(A) order.
     * @param channelCount Number of channels (1-4)
     * @return
     */
    VL_CHANNEL_FORMAT VL_API getChannelFormatFromCount(U32 channelCount);

    struct VL_API ImageLoadDesc {
        fs::path fileName;
        bool flipOnLoad         = true;
        VL_CHANNEL_FORMAT requestedFormat = VL_CHANNEL_UNKNOWN; // If UNKNOWN, load all channels available in the image
    };

    struct VL_API ImageWriteDesc {
        fs::path fileName;
        bool flipOnWrite        = false;
        VL_IMAGE_TYPE fileType  = VL_IMAGE_PNG;
    };

    struct VL_API ConvertToF32Desc {
        bool decodeSRGB       = true;   // If true, convert from sRGB to linear color space (for HDR calculations)
    };

    struct VL_API ImageUI8Desc {
        const U8* data               = nullptr;
        U32 width                    = 0;
        U32 height                   = 0;
        VL_CHANNEL_FORMAT format     = VL_CHANNEL_RGBA;
        U8 defaultChannelValue      = 255; // In case an empty image is requested, fill buffer with this value
    };

    struct VL_API ImageUI16Desc {
        const U16* data              = nullptr;
        U32 width                    = 0;
        U32 height                   = 0;
        VL_CHANNEL_FORMAT format     = VL_CHANNEL_RGBA;
        U16 defaultChannelValue     = 65535; // In case an empty image is requested, fill buffer with this value
    };

    struct VL_API ImageF32Desc {
        const float* data              = nullptr;
        U32 width                    = 0;
        U32 height                   = 0;
        VL_CHANNEL_FORMAT format     = VL_CHANNEL_RGBA;
        float defaultChannelValue     = 1.0f; // In case an empty image is requested, fill buffer with this value
    };

    struct VL_API FormatConversionDesc {
        VL_CHANNEL_FORMAT targetFormat;
        /*
         * In case of up-conversion, fill new channels with max or min value
         * For UI8, these values are 0 and 255
         * For UI16, these values are 0 and 65535
         * For F32, these values are 0.0f and 1.0f
         */
        VL_FORMAT_CONVERSION_FILL fillMode = VL_FILL_MAX;
    };

}