macro(package_add_test TESTNAME)
add_executable(${TESTNAME} ${ARGN})
target_include_directories (${TESTNAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Mocks)
target_link_libraries(${TESTNAME} gtest gmock gtest_main shared_lib)
    gtest_discover_tests(${TESTNAME}
        WORKING_DIRECTORY ${PROJECT_DIR}
        PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_DIR}"
    )
    set_target_properties(${TESTNAME}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests
        FOLDER tests)
endmacro()

package_add_test(Mutex_test Mutex_test.cpp)
package_add_test(TCPIOHandler_test TCPIOHandler_test.cpp)
package_add_test(TimeStampManager_test TimeStampManager_test.cpp)
package_add_test(IOController_test IOController_test.cpp)
