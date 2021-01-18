cmake_minimum_required(VERSION 3.16)
project(ft_IRC)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

set (CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -Wunreachable-code -fsanitize=address -g")

add_subdirectory(shared_lib)
add_subdirectory(tcp_lib)
add_subdirectory(server)
add_subdirectory(client)

option(NIX "Use nix for dependencies" OFF)
option(PACKAGE_TESTS "Build tests" OFF)

if(PACKAGE_TESTS)
    enable_testing()
    include(GoogleTest)
    if (NOT(NIX))
        add_subdirectory(extern/googletest)
    endif()
    add_subdirectory(tcp_lib/tests)
    add_subdirectory(shared_lib/tests)
    add_subdirectory(server/tests)
endif()
