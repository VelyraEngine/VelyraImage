#include "Pch.hpp"

#include <VelyraImage/IImage.hpp>

#include "FormatConversion/FormatConversion.hpp"

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


    IImage::IImage(const VL_TYPE type, const char* loggerName):
    m_DataType(type),
    m_Logger(Utils::getLogger(loggerName)) {

    }

    IImage::IImage(const Size width, const Size height, const VL_TYPE type, const VL_CHANNEL_FORMAT format, const char* loggerName):
    m_Width(width),
    m_Height(height),
    m_DataType(type),
    m_Format(format),
    m_Logger(Utils::getLogger(loggerName)) {

    }

    template<typename T>
    void IImage::setData(const ImageLoadDesc& desc, const T* loadedData, const I32 loadedWidth, const I32 loadedHeight, I32 loadedChannels,
        std::vector<T>& destinationData) {
        m_Width = static_cast<Size>(loadedWidth);
        m_Height = static_cast<Size>(loadedHeight);
        const VL_CHANNEL_FORMAT loadedFormat = getChannelFormatFromCount(static_cast<U32>(loadedChannels));

        if (loadedFormat != desc.requestedFormat && desc.requestedFormat != VL_CHANNEL_FORMAT_MAX_VALUE) {
            SPDLOG_LOGGER_INFO(m_Logger, "Image: {} loaded with {} channels, converting to requested format {}", desc.fileName.string(), loadedChannels, desc.requestedFormat);
            m_Format = desc.requestedFormat;

            const std::span<const T> sourceView(loadedData, m_Width * m_Height * static_cast<Size>(loadedChannels));

            destinationData.resize(m_Width * m_Height * getChannelCountFromFormat(desc.requestedFormat));

            FormatConversionDesc conversionDesc;
            conversionDesc.targetFormat = desc.requestedFormat;
            conversionDesc.fillMode = desc.fillMode;
            convertFormat<T>(loadedFormat, sourceView, destinationData, conversionDesc);
        }
        else {
            m_Format = loadedFormat;
            destinationData.resize(m_Width * m_Height * static_cast<U32>(loadedChannels));
            memcpy(destinationData.data(), loadedData, m_Width * m_Height * static_cast<U32>(loadedChannels) * sizeof(T));
        }
    }

    template void IImage::setData<U8>(const ImageLoadDesc& desc, const U8* loadedData, const I32 loadedWidth, const I32 loadedHeight, I32 loadedChannels,
        std::vector<U8>& destinationData);
    template void IImage::setData<float>(const ImageLoadDesc& desc, const float* loadedData, const I32 loadedWidth, const I32 loadedHeight, I32 loadedChannels,
        std::vector<float>& destinationData);

}
