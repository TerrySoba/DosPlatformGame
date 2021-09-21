cmake_minimum_required(VERSION 3.10)
project(Png2Tga)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "-O3")

add_executable(${PROJECT_NAME}
    main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/3rdparty/lodepng/lodepng.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/3rdparty)