cmake_minimum_required(VERSION 3.14)
project(server_http)

find_package(Boost REQUIRED COMPONENTS context)
find_library(Datacarder NAMES datacarder PATHS /app/build/datacarder)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LUAJIT REQUIRED luajit)

add_executable(server_http src/server_http.cpp src/http_router.cpp src/handlers.cpp)

target_include_directories(server_http PRIVATE include /app/datacarder/include /third_party/json-3.11.2/include)
target_link_libraries(server_http PRIVATE ${Datacarder} boost_context ${LUAJIT_LIBRARIES} -ldl -lm)
