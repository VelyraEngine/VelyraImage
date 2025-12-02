#include "Pch.hpp"

#include "ImageUtils.hpp"

namespace Velyra::Image {

    stbir_pixel_layout vlFormatToStbirFormat(const VL_CHANNEL_FORMAT format) {
        switch (format) {
            case VL_CHANNEL_R:      return STBIR_1CHANNEL;
            case VL_CHANNEL_RG:     return STBIR_2CHANNEL;
            case VL_CHANNEL_RGB:    return STBIR_RGB;
            case VL_CHANNEL_RGBA:   return STBIR_RGBA;
            case VL_CHANNEL_BGR:    return STBIR_BGR;
            case VL_CHANNEL_BGRA:   return STBIR_BGRA;
            default:                VL_THROW("Unsupported VL_CHANNEL_FORMAT {} for stb_image_resize conversion", format);
        }
    }

}
