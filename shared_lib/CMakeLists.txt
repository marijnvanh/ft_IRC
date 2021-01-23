project(shared_lib VERSION 1.0)

file(GLOB SOURCES "src/*.cpp")

add_library(${PROJECT_NAME} ${SOURCES})

target_link_libraries (${PROJECT_NAME} LINK_PUBLIC tcp_lib)
target_include_directories (${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

