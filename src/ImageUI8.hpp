#pragma once

#include <VelyraImage/IImage.hpp>
#include <vector>

#include "LoggerNames.hpp"

namespace Velyra::Image {

    class ImageUI8: public IImage {
    public:
        explicit ImageUI8(const ImageLoadDesc& desc);

        explicit ImageUI8(const ImageUI8Desc& desc);

        ~ImageUI8() override = default;

        void write(const ImageWriteDesc& desc) override;

        UP<IImage> resize(U32 width, U32 height) override;

        void* getData() override;

    private:
        std::vector<U8> m_Data;
        Utils::LogPtr m_Logger = Utils::getLogger(LOGGER_UI8);
    };

}