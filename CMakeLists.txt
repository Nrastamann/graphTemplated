cmake_minimum_required(VERSION 4.2.3)
project(GraphEngine)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


set(CMAKE_CXX_STANDART 20)
set(CMAKE_CXX_STANDART_REQUIRED ON)

add_subdirectory(libEngine)
add_subdirectory(src)
