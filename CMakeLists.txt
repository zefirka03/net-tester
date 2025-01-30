cmake_minimum_required(VERSION 3.13)
project(net)
set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES "./src/main.cpp" "./src/client.cpp" "./src/server.cpp")

add_compile_definitions(DEBUG)
add_executable(net ${SOURCE_FILES})