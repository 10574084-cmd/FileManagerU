cmake_minimum_required(VERSION 3.7)
project(WiiUFileManager C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(WiiUFileManager
    source/main.cpp
    source/file_ops.cpp
)

target_include_directories(WiiUFileManager PRIVATE include)

# WUT helper creates the native Wii U RPX.
wut_create_rpx(WiiUFileManager)
