
cmake_minimum_required(VERSION 3.16)
project(neural_network_encryption C)

set(CMAKE_C_STANDARD 11)

add_executable(neural_network_encryption main.c)

if(UNIX)
    target_link_libraries(neural_network_encryption m)
endif()

if(WIN32)
    target_link_libraries(neural_network_encryption bcrypt)
endif()

