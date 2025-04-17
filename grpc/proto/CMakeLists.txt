cmake_minimum_required(VERSION 3.14)
project(proto)

# Trova Protobuf
find_package(Protobuf REQUIRED)

# Trova gRPC
find_package(gRPC REQUIRED)

# Imposta i file .proto da compilare
set(PROTO_FILES service.proto)

# Definisce i percorsi ai file generati
set(PROTO_SRC_DIR ${CMAKE_BINARY_DIR}/generated)
set(GRPC_SRC_DIR ${CMAKE_BINARY_DIR}/generated)

# Crea le directory per i file generati
file(MAKE_DIRECTORY ${PROTO_SRC_DIR})
file(MAKE_DIRECTORY ${GRPC_SRC_DIR})

find_program(GRPC_CPP_PLUGIN NAMES grpc_cpp_plugin PATHS /usr/bin NO_DEFAULT_PATH)

message(STATUS "Protobuf: ${PROTOBUF_PROTOC_EXECUTABLE}")
message(STATUS "gRPC Plugin: ${GRPC_CPP_PLUGIN}")

# Esegui il comando 'protoc' con il plugin gRPC
execute_process(
    COMMAND ${PROTOBUF_PROTOC_EXECUTABLE} --proto_path=. --cpp_out=${PROTO_SRC_DIR} --grpc_out=${GRPC_SRC_DIR} --plugin=protoc-gen-grpc=/usr/bin/grpc_cpp_plugin ${PROTO_FILES}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

# Include directories per protoc
include_directories(${PROTO_SRC_DIR} ${GRPC_SRC_DIR})

# Aggiungi i file generati alla libreria
add_library(proto_service STATIC 
    ${PROTO_SRC_DIR}/service.pb.cc
    ${GRPC_SRC_DIR}/service.grpc.pb.cc
    ${PROTO_SRC_DIR}/service.pb.h
    ${GRPC_SRC_DIR}/service.grpc.pb.h
)

# Linka le librerie necessarie
target_link_libraries(proto_service PUBLIC gRPC::grpc++ protobuf::libprotobuf)
