cmake_minimum_required(VERSION 3.9)
project(adpcm_encoder)
set(CMAKE_CXX_STANDARD 20)

#find_package(Catch2 REQUIRED)
#find_package(vectorclass REQUIRED)
find_package(OpenMP REQUIRED)

add_library(vectorclass INTERFACE)
target_include_directories(vectorclass INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/vectorclass
)


add_library(adpcm_encoder_lib STATIC
    src/file_tools.cpp
    src/voc_format.cpp
    src/read_wave.cpp
    src/resampling.cpp
    src/encode_creative_adpcm.cpp
    src/encode_creative_adpcm_simd.cpp
)

target_link_libraries(adpcm_encoder_lib
    PUBLIC vectorclass
    PUBLIC OpenMP::OpenMP_CXX)

add_executable(adpcm_encoder
    src/main.cpp
)

target_link_libraries(adpcm_encoder
    adpcm_encoder_lib
)

# add_executable(adpcm_encoder_test
#     src/resampling_test.cpp 
#     )

# target_link_libraries(adpcm_encoder_test
#     PUBLIC adpcm_encoder_lib
#     PUBLIC Catch2::Catch2WithMain
# )

# target_compile_definitions(adpcm_encoder_test PUBLIC TEST_DATA_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/testdata\")
