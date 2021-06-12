cmake_minimum_required(VERSION 3.16)
project(ft_IRC)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

set (CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -Wunreachable-code -fsanitize=address -g -pthread")

set(JSON_BuildTests OFF CACHE INTERNAL "")

# To use SSL the server requires OpenSSL to be installed
# SSL can be disabled on machines without OpenSSL
option(ENABLE_SSL "Enable SSL" ON)
if(ENABLE_SSL)
  add_definitions(-DENABLE_SSL=1)
endif()

add_subdirectory(shared_lib)
add_subdirectory(server)
add_subdirectory(extern/json)

option(PACKAGE_TESTS "Build tests" OFF)

if(PACKAGE_TESTS)
    enable_testing()
    include(GoogleTest)
    add_subdirectory(extern/googletest)
    add_subdirectory(shared_lib/tests)
    add_subdirectory(server/tests)
endif()
