#pragma once

#include <VelyraImage/ImageDefs.hpp>
#include <VelyraUtils/Types/SymbolicTypes.hpp>

namespace Velyra::Image {

    class VL_API IImage {
    public:
        virtual ~IImage() = default;

        virtual void write(const ImageWriteDesc& desc) = 0;

        virtual UP<IImage> resize(U32 width, U32 height) = 0;

        virtual void* getData() = 0;

        virtual UP<IImage> convertToFormat(const FormatConversionDesc& desc) const = 0;

        U32 getWidth() const { return m_Width; }

        U32 getHeight() const { return m_Height; }

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
        explicit IImage(VL_TYPE type);

        IImage(U32 width, U32 height, VL_TYPE type, VL_CHANNEL_FORMAT format);

    protected:
        U32 m_Width = 0;
        U32 m_Height = 0;
        const VL_TYPE m_DataType = VL_TYPE_NONE;
        VL_CHANNEL_FORMAT m_Format = VL_CHANNEL_UNKNOWN;

    };

}