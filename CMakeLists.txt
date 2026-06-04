cmake_minimum_required(VERSION 4.2)
project(Image_Viewer)

set(CMAKE_CXX_STANDARD 23)

include_directories(.)

add_executable(Image_Viewer
        main.cpp
        README.md
        helpers.hpp
)
