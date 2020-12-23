cmake_minimum_required(VERSION 3.16)
project(ft_IRC)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

set (CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -Wunreachable-code")


add_subdirectory(server)
add_subdirectory(client)
