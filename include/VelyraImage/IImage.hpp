#pragma once

#include <VelyraImage/ImageDefs.hpp>
#include <VelyraUtils/Types/SymbolicTypes.hpp>
#include <VelyraUtils/LoggingFwd.hpp>

namespace Velyra::Image {

    class VL_API IImage {
    public:
        virtual ~IImage() = default;

        /**
         * @brief Writes the image to disk using the specified format and settings.
         * @param desc
         */
        virtual void write(const ImageWriteDesc& desc) = 0;

        /**
         * @brief Resizes the image to the specified width and height using the specified interpolation method.
         * @param width New width of the image
         * @param height New height of the image
         * @return
         */
        virtual UP<IImage> resize(Size width, Size height) = 0;

        virtual void* getData() = 0;

        /**
         * @brief Converts the image to a different channel format. For example, RGB to RGBA or BGR to RGB.
         * @param desc Description of the format conversion, including the target channel format and how to fill missing channels if necessary.
         * @return
         */
        virtual UP<IImage> convertToFormat(const FormatConversionDesc& desc) const = 0;

        /**
         * @brief Converts the image to a different data type. For example, from UI8 to F32 or from UI16 to UI8.
         * @param desc
         * @return
         */
        virtual UP<IImage> translateDataType(const TranslationDesc& desc) const = 0;

        Size getWidth() const { return m_Width; }

        Size getHeight() const { return m_Height; }

        VL_CHANNEL_FORMAT getChannelFormat() const { return m_Format; }

        VL_TYPE getDataType() const { return m_DataType; }

        /**
         * @brief Returns the number of pixels in the image (width * height)
         * @return Number of pixels
         */
        Size getPixelCount() const;

        /**
         * @brief Returns the total number of elements in the image (pixels * channels)
         * @return Total number of elements
         */
        Size getCount() const;

        /**
         * @brief Returns the total size of the image data in bytes
         * @return Size in bytes
         */
        Size getSize() const;

    protected:
        IImage(VL_TYPE type, const char* loggerName);

        IImage(Size width, Size height, VL_TYPE type, VL_CHANNEL_FORMAT format, const char* loggerName);

        template<typename T>
        void setData(const ImageLoadDesc& desc, const T* loadedData, I32 loadedWidth, I32 loadedHeight, I32 loadedChannels,
            std::vector<T>& destinationData);

    protected:
        Size m_Width = 0;
        Size m_Height = 0;
        const VL_TYPE m_DataType = VL_TYPE_NONE;
        VL_CHANNEL_FORMAT m_Format = VL_CHANNEL_FORMAT_MAX_VALUE;
        Utils::LogPtr m_Logger;

    };

}