#include "Pch.hpp"

#include "ImageUI8.hpp"
#include "ImageUtils.hpp"
#include "FormatConversion/FormatConversion.hpp"

namespace Velyra::Image {

    ImageUI8::ImageUI8(const ImageLoadDesc &desc):
    IImage(VL_UINT8){
        stbi_set_flip_vertically_on_load(desc.flipOnLoad);
        I32 channelCount = 0;
        I32 width = 0;
        I32 height = 0;
        U8* pData = stbi_load(desc.fileName.string().c_str(), &width, &height, &channelCount, 0);
        if (!pData) {
            SPDLOG_LOGGER_ERROR(m_Logger, "Image: {} failed to load", desc.fileName.string());
            return;
        }
        m_Width = static_cast<U32>(width);
        m_Height = static_cast<U32>(height);

        const VL_CHANNEL_FORMAT loadedFormat = getChannelFormatFromCount(channelCount);
        if (loadedFormat != desc.requestedFormat && desc.requestedFormat != VL_CHANNEL_UNKNOWN) {
            SPDLOG_LOGGER_INFO(m_Logger, "Image: {} loaded with {} channels, converting to requested format {}", desc.fileName.string(), channelCount, desc.requestedFormat);
        }
        else {
            m_Format = loadedFormat;
            m_Data.resize(m_Width * m_Height * channelCount);
            memcpy(m_Data.data(), pData, m_Width * m_Height * channelCount);
        }
        stbi_image_free(pData);

        SPDLOG_LOGGER_INFO(m_Logger, "Loaded ImageUI8: {} with size ({}x{}) and format {}", desc.fileName.string(), m_Width, m_Height, m_Format);
    }

    ImageUI8::ImageUI8(const ImageUI8Desc &desc):
    IImage(desc.width, desc.height, VL_UINT8, desc.format),
    m_Data(desc.width * desc.height * getChannelCountFromFormat(desc.format), 255) {
        if (desc.data != nullptr) {
            memcpy(m_Data.data(), desc.data, desc.width * desc.height * getChannelCountFromFormat(desc.format));
        }
        SPDLOG_LOGGER_INFO(m_Logger, "Created ImageUI8 with size ({}x{}) and format {}", m_Width, m_Height, m_Format);
    }

    void ImageUI8::write(const ImageWriteDesc &desc) {
        stbi_flip_vertically_on_write(desc.flipOnWrite);
        const auto width = static_cast<I32>(m_Width);
        const auto height = static_cast<I32>(m_Height);
        const I32 channelCount = getChannelCountFromFormat(m_Format);

        switch (desc.fileType) {
            case VL_IMAGE_PNG: {
                stbi_write_png(desc.fileName.string().c_str(), width, height, channelCount, &m_Data[0], width * channelCount);
                break;
            }
            case VL_IMAGE_JPG: {
                stbi_write_jpg(desc.fileName.string().c_str(), width, height, channelCount, &m_Data[0], 100); // last parameter is the quality, 100 is the highest quality
                break;
            }
            case VL_IMAGE_BMP: {
                stbi_write_bmp(desc.fileName.string().c_str(), width, height, channelCount, &m_Data[0]);
                break;
            }
            default: {
                SPDLOG_LOGGER_WARN(m_Logger, "Image: {} cannot be written to file type: {}", desc.fileName.string(), desc.fileType);
                break;
            }
        }
    }

    UP<IImage> ImageUI8::resize(const U32 width, const U32 height) {
        if (width == 0 || height == 0) {
            SPDLOG_LOGGER_WARN(m_Logger, "Image cannot be resized to ({}x{})", width, height);

            // Simply return a copy of the current image
            ImageUI8Desc desc;
            desc.width = m_Width;
            desc.height = m_Height;
            desc.format = m_Format;
            desc.data = m_Data.data();
            return createUP<ImageUI8>(desc);
        }

        ImageUI8Desc desc;
        desc.width = width;
        desc.height = height;
        desc.format = m_Format;
        desc.data = nullptr;
        auto resizedImage = createUP<ImageUI8>(desc);
        if (!stbir_resize_uint8_linear(
                m_Data.data(), static_cast<I32>(m_Width), static_cast<I32>(m_Height), 0,
                static_cast<unsigned char *>(resizedImage->getData()), static_cast<I32>(width), static_cast<I32>(height), 0,
                vlFormatToStbirFormat(m_Format))) {
            SPDLOG_LOGGER_ERROR(m_Logger, "Failed to resize ImageUI8 from ({}x{}) to ({}x{})", m_Width, m_Height, width, height);
        }
        return resizedImage;
    }

    void* ImageUI8::getData() {
        return m_Data.data();
    }

    UP<IImage> ImageUI8::convertToFormat(const FormatConversionDesc &desc) const {
        ImageUI8Desc targetDesc;
        targetDesc.width = m_Width;
        targetDesc.height = m_Height;
        targetDesc.format = desc.targetFormat;
        targetDesc.data = nullptr;
        auto targetImage = createUP<ImageUI8>(targetDesc);
        convertFormat<U8>(m_Format, m_Data, targetImage->m_Data, desc);
        return targetImage;
    }
}
