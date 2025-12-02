#include "Pch.hpp"

#include <VelyraImage/IImage.hpp>

namespace Velyra::Image {

    Size IImage::getPixelCount() const {
        return m_Width * m_Height;
    }

    Size IImage::getCount() const {
        return getPixelCount() * getChannelCountFromFormat(m_Format);
    }

    Size IImage::getSize() const {
        const Size typeSize = Utils::getTypeSize(m_DataType);
        return getCount() * typeSize;
    }


    IImage::IImage(const VL_TYPE type):
    m_DataType(type) {

    }

    IImage::IImage(const U32 width, const U32 height, const VL_TYPE type, const VL_CHANNEL_FORMAT format):
    m_Width(width),
    m_Height(height),
    m_DataType(type),
    m_Format(format) {

    }

}
