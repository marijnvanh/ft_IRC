project(shared_lib VERSION 1.0)

file(GLOB SOURCES "src/*.cpp")

if(ENABLE_SSL)
    find_package(OpenSSL REQUIRED)
endif()

add_library(${PROJECT_NAME} ${SOURCES})

if(ENABLE_SSL)
    target_link_libraries(${PROJECT_NAME} OpenSSL::SSL)
    target_include_directories (${PROJECT_NAME} PUBLIC
        ${OPENSSL_ROOT_DIR}/include
    )
endif()

target_include_directories (${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
