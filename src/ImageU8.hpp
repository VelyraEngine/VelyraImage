#pragma once

#include <VelyraImage/IImage.hpp>
#include <vector>

#include "LoggerNames.hpp"

namespace Velyra::Image {

    class ImageU8: public IImage {
    public:
        explicit ImageU8(const ImageLoadDesc& desc);

        explicit ImageU8(const ImageU8Desc& desc);

        ~ImageU8() override = default;

        void write(const ImageWriteDesc& desc) override;

        UP<IImage> resize(U32 width, U32 height) override;

        void* getData() override;

        UP<IImage> convertToFormat(const FormatConversionDesc& desc) const override;

    private:
        std::vector<U8> m_Data;
        Utils::LogPtr m_Logger = Utils::getLogger(LOGGER_UI8);
    };

}