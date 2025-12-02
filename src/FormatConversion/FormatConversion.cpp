#include "../Pch.hpp"

#include "FormatConversion.hpp"

namespace Velyra::Image {

    std::vector<int> defineSwizzle(const VL_CHANNEL_FORMAT sourceFormat, const VL_CHANNEL_FORMAT targetFormat) {
        auto decoder = [&](const VL_CHANNEL_FORMAT fmt) {
            switch (fmt) {
                case VL_CHANNEL_R:    return std::vector<int>{0};
                case VL_CHANNEL_RG:   return std::vector<int>{0,1};
                case VL_CHANNEL_RGB:  return std::vector<int>{0,1,2};
                case VL_CHANNEL_RGBA: return std::vector<int>{0,1,2,3};
                case VL_CHANNEL_BGR:  return std::vector<int>{2,1,0};
                case VL_CHANNEL_BGRA: return std::vector<int>{2,1,0,3};
                default: return std::vector<int>{};
            }
        };
        std::vector<int> sourceChannels = decoder(sourceFormat);
        std::vector<int> targetChannels = decoder(targetFormat);

        // Build swizzle map
        // -1 indicates that the channel is not present in the source and should be filled
        std::vector<int> swizzle(targetChannels.size(), -1);
        for (Size i = 0; i < targetChannels.size(); ++i) {
            int targetChannel = targetChannels[i];
            auto it = std::find(sourceChannels.begin(), sourceChannels.end(), targetChannel);
            if (it != sourceChannels.end()) {
                swizzle[i] = static_cast<int>(std::distance(sourceChannels.begin(), it));
            }
        }
        return swizzle;
    }
}