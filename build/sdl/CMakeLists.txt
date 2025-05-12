cmake_minimum_required(VERSION 3.31)
project(SdlPlatformGame)

set(CMAKE_CXX_STANDARD 23)

find_package(SDL3 REQUIRED)

set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}")

get_filename_component(GAME_BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
set(SOURCE_DIR "${GAME_BASE_DIR}/source")

message(STATUS "Source: ${SOURCE_DIR}")

add_library(game_lib STATIC
    ${SOURCE_DIR}/animation.cpp
    ${SOURCE_DIR}/blit.cpp
    ${SOURCE_DIR}/boss1.cpp
    ${SOURCE_DIR}/boss2.cpp
    ${SOURCE_DIR}/credits_loader.cpp
    ${SOURCE_DIR}/detect_lines.cpp
    ${SOURCE_DIR}/enemy.cpp
    ${SOURCE_DIR}/exception.cpp
    ${SOURCE_DIR}/eye.cpp
    ${SOURCE_DIR}/fire_ball.cpp
    ${SOURCE_DIR}/font_writer.cpp
    ${SOURCE_DIR}/font.cpp
    ${SOURCE_DIR}/framebuffer_gfx.cpp
    ${SOURCE_DIR}/game_save.cpp
    ${SOURCE_DIR}/game.cpp
    ${SOURCE_DIR}/i18n.cpp
    ${SOURCE_DIR}/key_mapper.cpp
    ${SOURCE_DIR}/keyboard.cpp
    ${SOURCE_DIR}/level.cpp
    ${SOURCE_DIR}/palette_tools.cpp
    ${SOURCE_DIR}/physics.cpp
    ${SOURCE_DIR}/platform/sdl/commandline_parser.cpp
    ${SOURCE_DIR}/platform/sdl/decode_vorbis.cpp
    ${SOURCE_DIR}/platform/sdl/joystick_sdl.cpp
    ${SOURCE_DIR}/platform/sdl/keyboard_sdl.cpp
    ${SOURCE_DIR}/platform/sdl/music_controller_sdl.cpp
    ${SOURCE_DIR}/platform/sdl/sdl_gfx.cpp
    ${SOURCE_DIR}/platform/sdl/sound_controller_sdl.cpp
    ${SOURCE_DIR}/rectangle.cpp
    ${SOURCE_DIR}/seeker_enemy.cpp
    ${SOURCE_DIR}/sprite.cpp
    ${SOURCE_DIR}/tentacle_arm.cpp
    ${SOURCE_DIR}/tentacle.cpp
    ${SOURCE_DIR}/text.cpp
    ${SOURCE_DIR}/tga_image.cpp
    ${SOURCE_DIR}/time_tools.cpp
    ${SOURCE_DIR}/tiny_string.cpp

    ${SOURCE_DIR}/3rd_party/stb/stb_vorbis.c
)

target_link_libraries(game_lib PRIVATE SDL3::SDL3)

string(TIMESTAMP CMAKE_BUILD_DATE "%Y-%m-%d")
target_compile_definitions(game_lib PRIVATE PLATFORM_SDL BUILD_DATE="${CMAKE_BUILD_DATE}")

#target_compile_options(game_lib PRIVATE -fsanitize=address)
#target_link_options(game_lib PRIVATE -fsanitize=address)

target_include_directories(game_lib PUBLIC ${SOURCE_DIR})

# Create your game executable target as usual
add_executable(SdlPlatformGame 
    ${SOURCE_DIR}/platform/sdl/main.cpp)
target_link_libraries(SdlPlatformGame PRIVATE game_lib)
target_compile_definitions(SdlPlatformGame PRIVATE PLATFORM_SDL BUILD_DATE="${CMAKE_BUILD_DATE}")
target_link_libraries(SdlPlatformGame PRIVATE SDL3::SDL3)

# install(TARGETS SDL3-shared DESTINATION bin)

install(TARGETS SdlPlatformGame DESTINATION bin)

install(DIRECTORY "${GAME_BASE_DIR}/levels/"
    DESTINATION bin
    FILES_MATCHING PATTERN "*.map"
)

install(DIRECTORY "${GAME_BASE_DIR}/images/"
    DESTINATION bin
    FILES_MATCHING PATTERN "*.tga" PATTERN "*.ani" 
)

install(DIRECTORY "${GAME_BASE_DIR}/text/"
    DESTINATION bin
    FILES_MATCHING PATTERN "*.en" PATTERN "*.de" 
)

install(DIRECTORY "${GAME_BASE_DIR}/sound/"
    DESTINATION bin
    FILES_MATCHING PATTERN "*.ogg"
)

set(TEST_DATA_DIR "${GAME_BASE_DIR}/testdata/")

# Create test executable target
add_executable(test
    ${SOURCE_DIR}/blit_test.cpp
    ${SOURCE_DIR}/credits_loader_test.cpp
    ${SOURCE_DIR}/exception_test.cpp
    ${SOURCE_DIR}/font_test.cpp
    ${SOURCE_DIR}/font_writer_test.cpp
    ${SOURCE_DIR}/i18n_test.cpp
    ${SOURCE_DIR}/level_test.cpp
    ${SOURCE_DIR}/map_test.cpp
    ${SOURCE_DIR}/physics_test.cpp
    ${SOURCE_DIR}/platform/sdl/decode_vorbis_test.cpp
    ${SOURCE_DIR}/ptr_vector_test.cpp
    ${SOURCE_DIR}/raii_ptr_test.cpp
    ${SOURCE_DIR}/rectangle_test.cpp
    ${SOURCE_DIR}/shared_ptr_test.cpp
    ${SOURCE_DIR}/sprite_test.cpp
    ${SOURCE_DIR}/test_class.cpp
    ${SOURCE_DIR}/test_main.cpp
    ${SOURCE_DIR}/text_layout_test.cpp
    ${SOURCE_DIR}/tga_image_test.cpp
    ${SOURCE_DIR}/time_tools_test.cpp
    ${SOURCE_DIR}/tiny_string_test.cpp
    ${SOURCE_DIR}/unit_test.cpp
    ${SOURCE_DIR}/vector_test.cpp
)

target_include_directories(test PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}/DosPlatformGame/source
)

target_compile_definitions(test PRIVATE TEST_DATA_DIR="${TEST_DATA_DIR}")

target_link_libraries(test PRIVATE game_lib)

# Enable AddressSanitizer for the test target
target_compile_options(test PRIVATE -fsanitize=address)
target_link_options(test PRIVATE -fsanitize=address)


