cmake_minimum_required(VERSION 3.14)
project(server)

add_compile_options(-g -O3)
set(CMAKE_CXX_STANDARD 17)

#add_subdirectory(grpc)
add_subdirectory(http)
