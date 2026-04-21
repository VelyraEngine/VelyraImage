#pragma once

#include <VelyraImage/IImage.hpp>
#include <vector>

#include "LoggerNames.hpp"

namespace Velyra::Image {

    class ImageF32: public IImage {
    public:
        explicit ImageF32(const ImageLoadDesc& desc);

        explicit ImageF32(const ImageF32Desc& desc);

        ~ImageF32() override = default;

        void write(const ImageWriteDesc& desc) override;

        UP<IImage> resize(Size width, Size height) override;

        void* getData() override;

        UP<IImage> convertToFormat(const FormatConversionDesc& desc) const override;

    private:
        std::vector<float> m_Data;
        Utils::LogPtr m_Logger = Utils::getLogger(LOGGER_F32);
    };

}