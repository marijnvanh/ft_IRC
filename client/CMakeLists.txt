project(irc_client VERSION 1.0)

include_directories(include)

file(GLOB SOURCES "src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries (${PROJECT_NAME} LINK_PUBLIC shared_lib)