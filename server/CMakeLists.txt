project(irc_server VERSION 1.0)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})

configure_file("include/Config.h.in" Config.h)

target_link_libraries (${PROJECT_NAME} LINK_PUBLIC shared_lib)
target_link_libraries (${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
target_include_directories (${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_BINARY_DIR})
