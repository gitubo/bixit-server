cmake_minimum_required(VERSION 3.14)
project(bixit_http)

find_package(Boost REQUIRED COMPONENTS context)
find_library(BIXIT_LIB NAMES bixit PATHS /app/build/bixit)
message(STATUS "BIXIT_LIB = ${BIXIT_LIB}")

find_package(PkgConfig REQUIRED)
pkg_check_modules(LUAJIT REQUIRED luajit)

add_executable(bixit_http src/server_http.cpp src/http_router.cpp src/handlers.cpp)

target_include_directories(bixit_http PRIVATE include /app/build/bixit/include /third_party/json-3.11.2/include)
target_link_libraries(bixit_http PRIVATE ${BIXIT_LIB} boost_context ${LUAJIT_LIBRARIES} dl m)
