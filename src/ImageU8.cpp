#include "Pch.hpp"

#include "ImageU8.hpp"
#include "ImageUtils.hpp"
#include "DataTypeConversion/DataTypeConversion.hpp"
#include "ImageF32.hpp"

namespace Velyra::Image {

    ImageU8::ImageU8(const ImageLoadDesc &desc):
    IImage(VL_UINT8, LOGGER_UI8){
        stbi_set_flip_vertically_on_load(desc.flipOnLoad);
        I32 channelCount = 0;
        I32 width = 0;
        I32 height = 0;
        U8* pData = stbi_load(desc.fileName.string().c_str(), &width, &height, &channelCount, 0);
        if (!pData) {
            SPDLOG_LOGGER_ERROR(m_Logger, "Image: {} failed to load", desc.fileName.string());
            return;
        }
        SPDLOG_LOGGER_INFO(m_Logger, "Loaded ImageU8: {} with size ({}x{}) and format {}", desc.fileName.string(), m_Width, m_Height, m_Format);

        setData<U8>(desc, pData, width, height, channelCount, m_Data);

        stbi_image_free(pData);
    }

    ImageU8::ImageU8(const ImageU8Desc &desc):
    IImage(desc.width, desc.height, VL_UINT8, desc.format, LOGGER_UI8),
    m_Data(desc.width * desc.height * getChannelCountFromFormat(desc.format), 255) {
        if (desc.data != nullptr) {
            memcpy(m_Data.data(), desc.data, desc.width * desc.height * getChannelCountFromFormat(desc.format));
        }
        SPDLOG_LOGGER_INFO(m_Logger, "Created ImageUI8 with size ({}x{}) and format {}", m_Width, m_Height, m_Format);
    }

    void ImageU8::write(const ImageWriteDesc &desc) {
        stbi_flip_vertically_on_write(desc.flipOnWrite);
        const auto width = static_cast<I32>(m_Width);
        const auto height = static_cast<I32>(m_Height);
        const I32 channelCount = static_cast<I32>(getChannelCountFromFormat(m_Format));

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

    UP<IImage> ImageU8::resize(const Size width, const Size height) {
        if (width == 0 || height == 0) {
            SPDLOG_LOGGER_WARN(m_Logger, "Image cannot be resized to ({}x{})", width, height);

            // Simply return a copy of the current image
            ImageU8Desc desc;
            desc.width = m_Width;
            desc.height = m_Height;
            desc.format = m_Format;
            desc.data = m_Data.data();
            return createUP<ImageU8>(desc);
        }

        ImageU8Desc desc;
        desc.width = width;
        desc.height = height;
        desc.format = m_Format;
        desc.data = nullptr;
        auto resizedImage = createUP<ImageU8>(desc);
        if (!stbir_resize_uint8_linear(
                m_Data.data(), static_cast<I32>(m_Width), static_cast<I32>(m_Height), 0,
                static_cast<unsigned char *>(resizedImage->getData()), static_cast<I32>(width), static_cast<I32>(height), 0,
                vlFormatToStbirFormat(m_Format))) {
            SPDLOG_LOGGER_ERROR(m_Logger, "Failed to resize ImageUI8 from ({}x{}) to ({}x{})", m_Width, m_Height, width, height);
        }
        return resizedImage;
    }

    void* ImageU8::getData() {
        return m_Data.data();
    }

    UP<IImage> ImageU8::convertToFormat(const FormatConversionDesc &desc) const {
        ImageU8Desc targetDesc;
        targetDesc.width = m_Width;
        targetDesc.height = m_Height;
        targetDesc.format = desc.targetFormat;
        targetDesc.data = nullptr;
        auto targetImage = createUP<ImageU8>(targetDesc);
        convertFormat<U8>(m_Format, m_Data, targetImage->m_Data, desc);
        return targetImage;
    }

    UP<IImage> ImageU8::translateDataType(const TranslationDesc &desc) const {
        switch (desc.targetType) {
            case VL_UINT8: {
                // Same type, return a copy
                ImageU8Desc targetDesc;
                targetDesc.width = m_Width;
                targetDesc.height = m_Height;
                targetDesc.format = m_Format;
                targetDesc.data = m_Data.data();
                return createUP<ImageU8>(targetDesc);
            }
            case VL_FLOAT32: {
                ImageF32Desc targetDesc;
                targetDesc.width = m_Width;
                targetDesc.height = m_Height;
                targetDesc.format = m_Format;
                targetDesc.data = nullptr;
                auto targetImage = createUP<ImageF32>(targetDesc);

                // Get direct access to the target data
                std::vector<float>& targetData = targetImage->m_Data;
                TranslateDataType::translateDataType<U8, float>(m_Data, targetData, desc);

                SPDLOG_LOGGER_INFO(m_Logger, "Translated ImageU8 to ImageF32 with size ({}x{}) and format {}",
                    m_Width, m_Height, m_Format);
                return targetImage;
            }
            default: {
                SPDLOG_LOGGER_ERROR(m_Logger, "Unsupported target type for translation from U8: {}", desc.targetType);
                return nullptr;
            }
        }
    }
}

