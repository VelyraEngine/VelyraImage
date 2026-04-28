#pragma once

#include <VelyraImage/IImage.hpp>

namespace Velyra::Image {

    class VL_API ImageFactory {
    public:
        static UP<IImage> createImage(const ImageLoadDesc& desc);

        static UP<IImage> createImageU8(const ImageU8Desc& desc);

        static UP<IImage> createImageF32(const ImageF32Desc& desc);

        /**
         * @brief Translate an image from one data type to another
         * @param source Source image to translate
         * @param desc Translation descriptor specifying target type and SIMD mode
         * @return New image with translated data type, or nullptr if translation fails
         */
        static UP<IImage> translateImageDataType(const IImage& source, const TranslationDesc& desc);
    };

}