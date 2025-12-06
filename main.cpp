#include <iostream>
#include <VelyraImage/ImageFactory.hpp>
#include <VelyraUtils/Clock/Clock.hpp>

using namespace Velyra;
using namespace Velyra::Image;
using namespace Velyra::Utils;

double runExperiment(const VL_SIMD_MODE mode, const UP<IImage>& image) {
    constexpr Size experimentRuns = 10;
    Duration totalDuration = Duration::zero();
    for (Size i = 0; i < experimentRuns; ++i) {
        FormatConversionDesc convDesc;
        convDesc.targetFormat = VL_CHANNEL_BGRA;
        convDesc.fillMode = VL_FILL_MAX;
        convDesc.simdMode = mode;

        TimePoint start = getTime();
        auto convertedImage = image->convertToFormat(convDesc);
        TimePoint end = getTime();

        Duration endDuration = end - start;
        std::cout << "Run " << (i + 1) << ": Format conversion took " << endDuration << " ms" << std::endl;
        totalDuration += endDuration;

        if (i == 0) {
            ImageWriteDesc writeDesc;
            writeDesc.fileName = fs::current_path() / "Resources" / "Test_grid_4000x4000_converted.png";
            writeDesc.fileType = VL_IMAGE_PNG;
            writeDesc.flipOnWrite = true;
            convertedImage->write(writeDesc);
        }
    }
    std::cout << "Average time over " << experimentRuns << " runs: " << (totalDuration.count() / experimentRuns) << " ms" << std::endl;

    return totalDuration.count() / experimentRuns;
}

int main() {
    std::cout << "Experiment with AVX2 optimizations in Velyra Image Format Conversion" << std::endl;
    ImageLoadDesc loadDesc;
    loadDesc.fileName = fs::current_path() / "Resources" / "Test_grid_4000x4000.png";
    loadDesc.flipOnLoad = true;

    auto image = ImageFactory::createImage(loadDesc);
    double scalarTime = runExperiment(VL_SIMD_SCALAR, image);
    double avx2Time = runExperiment(VL_SIMD_AVX2, image);
    std::cout << "===============================" << std::endl;
    std::cout << "SCALAR TIME: " << scalarTime << " ms" << std::endl;
    std::cout << "AVX2 TIME: " << avx2Time << " ms" << std::endl;
    std::cout << "Speedup (Scalar / AVX2): " << (scalarTime / avx2Time) << "x" << std::endl;


    return 0;
}