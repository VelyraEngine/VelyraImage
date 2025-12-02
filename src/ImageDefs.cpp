#include <VelyraImage/ImageDefs.hpp>

namespace Velyra::Image {

    U32 getChannelCountFromFormat(const VL_CHANNEL_FORMAT format) {
        switch (format) {
            case VL_CHANNEL_R:    return 1;
            case VL_CHANNEL_RG:   return 2;
            case VL_CHANNEL_RGB:  return 3;
            case VL_CHANNEL_RGBA: return 4;
            case VL_CHANNEL_BGR:  return 3;
            case VL_CHANNEL_BGRA: return 4;
            default:              return 0;
        }
    }

    VL_CHANNEL_FORMAT getChannelFormatFromCount(const U32 channelCount) {
        switch (channelCount) {
            case 1: return VL_CHANNEL_R;
            case 2: return VL_CHANNEL_RG;
            case 3: return VL_CHANNEL_RGB;
            case 4: return VL_CHANNEL_RGBA;
            default: return VL_CHANNEL_UNKNOWN;
        }
    }


}
