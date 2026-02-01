cmake_minimum_required(VERSION 3.22)
project(VelyraImage DESCRIPTION "Velyra Image library")

include(VelyraBuildUtils/VelyraBuildUtils.cmake)

set(VELYRA_UTILS_PATH)
include_or_fetch(VelyraUtils VELYRA_UTILS_PATH)
message(STATUS "VelyraUtils found at ${VELYRA_UTILS_PATH}")

set(VELYRA_IMAGE_HEADERS
    include/VelyraImage/IImage.hpp
    include/VelyraImage/ImageDefs.hpp
    include/VelyraImage/ImageFactory.hpp
    include/VelyraImage/VelyraImage.hpp

    src/LoggerNames.hpp
    src/ImageUtils.hpp
    src/ImageU8.hpp
    src/ImageF32.hpp

    src/FormatConversion/FormatConversion.hpp
)

set(VELYRA_IMAGE_SRC
    src/IImage.cpp
    src/ImageFactory.cpp
    src/ImageDefs.cpp
    src/ImageUtils.cpp
    src/ImageU8.cpp
    src/ImageF32.cpp

    src/FormatConversion/FormatConversion.cpp
)

set(STB_IMAGE_SRC
    src/stb_image/stb_image.c
    src/stb_image/stb_image_resize2.c
    src/stb_image/stb_image_write.c
)

add_library(stb_image STATIC ${STB_IMAGE_SRC})
target_include_directories(stb_image PUBLIC src/stb_image)
set_target_properties(stb_image PROPERTIES LINKER_LANGUAGE C)

add_library(VelyraImage STATIC ${VELYRA_IMAGE_HEADERS} ${VELYRA_IMAGE_SRC})
velyra_target_set_compile_flags(VelyraImage)
target_precompile_headers(VelyraImage PRIVATE src/Pch.hpp)
target_include_directories(VelyraImage PUBLIC include)
target_link_libraries(VelyraImage PRIVATE stb_image)
target_link_libraries(VelyraImage PUBLIC VelyraUtils)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # Enable SIMD features
    target_compile_options(VelyraImage PRIVATE -march=native)
endif ()

set(VELYRA_IMAGE_TEST_SRC
    test/TestImageDefs.cpp
    test/TestImageFactory.cpp
    test/TestImageUI8.cpp
    test/TestImageF32.cpp
    test/FormatConversion/TestFormatConversion.cpp
    test/FormatConversion/ImageConfig.hpp
)

if (BUILD_TESTING)
    fetch_gtest()

    add_executable(TestVelyraImage ${VELYRA_IMAGE_HEADERS} ${VELYRA_IMAGE_SRC} ${VELYRA_IMAGE_TEST_SRC})
    target_include_directories(TestVelyraImage PUBLIC include)
    target_precompile_headers(TestVelyraImage PRIVATE src/Pch.hpp)
    target_link_libraries(TestVelyraImage PUBLIC GTest::gtest GTest::gtest_main stb_image VelyraUtils)
    add_test(NAME CTestVelyraImage COMMAND ${PROOT} $<TARGET_FILE:TestVelyraImage>)
    set_tests_properties(CTestVelyraImage PROPERTIES
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        # Enable SIMD features
        target_compile_options(TestVelyraImage PRIVATE -march=native)
    endif ()
endif ()

add_executable(VelyraImageMain main.cpp)
target_link_libraries(VelyraImageMain PUBLIC VelyraImage)