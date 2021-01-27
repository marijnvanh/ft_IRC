macro(package_add_test TESTNAME)
    add_executable(${TESTNAME} ${ARGN})
<<<<<<< HEAD
    target_link_libraries(${TESTNAME} gtest gmock gtest_main shared_lib)
=======
    target_include_directories (${TESTNAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
    target_link_libraries(${TESTNAME} gtest gmock gtest_main tcp_lib shared_lib)
>>>>>>> develop
    gtest_discover_tests(${TESTNAME}
        WORKING_DIRECTORY ${PROJECT_DIR}
        PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_DIR}"
    )
    set_target_properties(${TESTNAME}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests
        FOLDER tests)
endmacro()

package_add_test(Client_test Client_test.cpp ../src/Client.cpp)