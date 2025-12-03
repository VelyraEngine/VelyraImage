#include "Pch.hpp"

#include "ImageF32.hpp"
#include "ImageUtils.hpp"
#include "FormatConversion/FormatConversion.hpp"

namespace Velyra::Image {

    ImageF32::ImageF32(const ImageLoadDesc &desc):
    IImage(VL_FLOAT32){
        stbi_set_flip_vertically_on_load(desc.flipOnLoad);
        I32 channelCount = 0;
        I32 width = 0;
        I32 height = 0;
        float* pData = stbi_loadf(desc.fileName.string().c_str(), &width, &height, &channelCount, 0);
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
            memcpy(m_Data.data(), pData, m_Width * m_Height * channelCount * sizeof(float));
        }
        stbi_image_free(pData);

        SPDLOG_LOGGER_INFO(m_Logger, "Loaded ImageUI8: {} with size ({}x{}) and format {}", desc.fileName.string(), m_Width, m_Height, m_Format);
    }

    ImageF32::ImageF32(const ImageF32Desc &desc):
    IImage(desc.width, desc.height, VL_FLOAT32, desc.format),
    m_Data(desc.width * desc.height * getChannelCountFromFormat(desc.format), 1.0f) {
        if (desc.data != nullptr) {
            memcpy(m_Data.data(), desc.data, desc.width * desc.height * getChannelCountFromFormat(desc.format) * sizeof(float));
        }
        SPDLOG_LOGGER_INFO(m_Logger, "Created ImageF32 with size ({}x{}) and format {}", m_Width, m_Height, m_Format);
    }

    void ImageF32::write(const ImageWriteDesc &desc) {
        if (desc.fileType != VL_IMAGE_HDR) {
            SPDLOG_LOGGER_WARN(m_Logger, "ImageF32 can only be written to HDR format. Image: {} will not be written", desc.fileName.string());
            return;
        }
        stbi_flip_vertically_on_write(desc.flipOnWrite);
        const auto width = static_cast<I32>(m_Width);
        const auto height = static_cast<I32>(m_Height);
        const I32 channelCount = getChannelCountFromFormat(m_Format);
        if (!stbi_write_hdr(desc.fileName.string().c_str(), width, height, channelCount, &m_Data[0])){
            SPDLOG_LOGGER_ERROR(m_Logger, "Image: {} failed to write", desc.fileName.string());
        }
    }

    UP<IImage> ImageF32::resize(U32 width, U32 height) {
        if (width == 0 || height == 0) {
            SPDLOG_LOGGER_WARN(m_Logger, "Image cannot be resized to ({}x{})", width, height);

            // Simply return a copy of the current image
            ImageF32Desc desc;
            desc.width = m_Width;
            desc.height = m_Height;
            desc.format = m_Format;
            desc.data = m_Data.data();
            return createUP<ImageF32>(desc);
        }

        ImageF32Desc desc;
        desc.width = width;
        desc.height = height;
        desc.format = m_Format;
        desc.data = nullptr;
        auto resizedImage = createUP<ImageF32>(desc);
        if (!stbir_resize_float_linear(&m_Data[0], m_Width, m_Height, 0,
            static_cast<float*>(resizedImage->getData()), width, height, 0,
            vlFormatToStbirFormat(m_Format))){
            SPDLOG_LOGGER_ERROR(m_Logger, "Failed to resize ImageF32 from ({}x{}) to ({}x{})", m_Width, m_Height, width, height);
        }
        return resizedImage;
    }

    void* ImageF32::getData() {
        return m_Data.data();
    }

    UP<IImage> ImageF32::convertToFormat(const FormatConversionDesc &desc) const {
        ImageF32Desc targetDesc;
        targetDesc.width = m_Width;
        targetDesc.height = m_Height;
        targetDesc.format = desc.targetFormat;
        targetDesc.data = nullptr;
        auto targetImage = createUP<ImageF32>(targetDesc);
        convertFormat<float>(m_Format, m_Data, desc.targetFormat, targetImage->m_Data, desc.fillMode);
        return targetImage;
    }
}