#pragma once

#include <VelyraImage/IImage.hpp>
#include <vector>

#include "LoggerNames.hpp"

namespace Velyra::Image {

    class ImageF32; // Forward declaration

    class ImageU8: public IImage {
    public:
        explicit ImageU8(const ImageLoadDesc& desc);

        explicit ImageU8(const ImageU8Desc& desc);

        ~ImageU8() override = default;

        void write(const ImageWriteDesc& desc) override;

        UP<IImage> resize(Size width, Size height) override;

        void* getData() override;

        UP<IImage> convertToFormat(const FormatConversionDesc& desc) const override;

        UP<IImage> translateDataType(const TranslationDesc& desc) const override;

    private:
        friend class ImageF32; // Allow ImageF32 to access m_Data
        
        std::vector<U8> m_Data;
        Utils::LogPtr m_Logger = Utils::getLogger(LOGGER_UI8);
    };

}