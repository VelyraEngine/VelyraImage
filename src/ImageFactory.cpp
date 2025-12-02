#include "Pch.hpp"

#include <VelyraImage/ImageFactory.hpp>

#include "ImageUI8.hpp"

namespace Velyra::Image {

    UP<IImage> ImageFactory::createImage(const ImageLoadDesc& desc) {
        Utils::LogPtr logger = Utils::getLogger(LOGGER_BASE);
        if (!fs::is_regular_file(desc.fileName)) {
            VL_THROW("Image file does not exist: {}", desc.fileName.string());
        }
        return createUP<ImageUI8>(desc);
    }

    UP<IImage> ImageFactory::createImageUI8(const ImageUI8Desc& desc) {
        return std::make_unique<ImageUI8>(desc);
    }

    UP<IImage> ImageFactory::createImageUI16(const ImageUI16Desc& desc) {
        VL_NOT_IMPLEMENTED();
    }
    
    UP<IImage> ImageFactory::createImageF32(const ImageF32Desc& desc) {
        VL_NOT_IMPLEMENTED();
    }

}