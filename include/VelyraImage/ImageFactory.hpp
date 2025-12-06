#pragma once

#include <VelyraImage/IImage.hpp>

namespace Velyra::Image {

    class VL_API ImageFactory {
    public:
        static UP<IImage> createImage(const ImageLoadDesc& desc);

        static UP<IImage> createImageU8(const ImageU8Desc& desc);

        static UP<IImage> createImageF32(const ImageF32Desc& desc);
    };

}