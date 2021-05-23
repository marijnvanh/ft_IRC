project(shared_lib VERSION 1.0)

file(GLOB SOURCES "src/*.cpp")


#//TODO Fix open SSL include path
if(ENABLE_SSL)
    set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl@1.1/")
    find_package(OpenSSL REQUIRED)
endif()

add_library(${PROJECT_NAME} ${SOURCES})

if(ENABLE_SSL)
    target_link_libraries(${PROJECT_NAME} OpenSSL::SSL)
    target_include_directories (${PROJECT_NAME} PUBLIC
        /usr/local/opt/openssl@1.1/include
    )
endif()

target_include_directories (${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
