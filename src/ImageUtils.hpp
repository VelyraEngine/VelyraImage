#pragma once

#include <VelyraImage/ImageDefs.hpp>

namespace Velyra::Image {

    stbir_pixel_layout vlFormatToStbirFormat(VL_CHANNEL_FORMAT format);

}