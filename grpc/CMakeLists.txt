cmake_minimum_required(VERSION 3.14)
project(server_grpc)

find_package(gRPC REQUIRED)

add_subdirectory(proto)

#add_executable(server_grpc src/server_grpc.cpp)
#target_include_directories(server_grpc PRIVATE include)
#target_link_libraries(server_grpc PRIVATE datacarder proto_service gRPC::grpc++)
