macro(package_add_test TESTNAME)
    add_executable(${TESTNAME} ${ARGN})
    target_include_directories (${TESTNAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Mocks)
    target_include_directories (${TESTNAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
    target_include_directories (${TESTNAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include/MessageHandlers)
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

package_add_test( Client_test
    Client_test.cpp
    ../src/Client.cpp
)
package_add_test( ClientDatabase_test
ClientDatabase_test.cpp
../src/ClientDatabase.cpp
../src/Client.cpp
../src/Server.cpp
)
package_add_test( ChannelDatabase_test
ChannelDatabase_test.cpp
../src/ChannelDatabase.cpp
../src/Channel.cpp
)
package_add_test( PASSHandler_test
./HandlerTests/PASSHandler_test.cpp
../src/MessageHandlers/PASSHandler.cpp
)
package_add_test( NICKHandler_test
./HandlerTests/NICKHandler_test.cpp
../src/MessageHandlers/NICKHandler.cpp
)
package_add_test( USERHandler_test
./HandlerTests/USERHandler_test.cpp
../src/MessageHandlers/USERHandler.cpp
)
package_add_test( Server_test
    Server_test.cpp
    ../src/Client.cpp
    ../src/Server.cpp
)
package_add_test( User_test
    User_test.cpp
    ../src/Client.cpp
)
package_add_test( QUITHandler_test
    ./HandlerTests/QUITHandler_test.cpp
    ../src/MessageHandlers/QUITHandler.cpp
)
package_add_test( ClientUpgrade_test
ClientUpgrade_test.cpp
../src/ClientDatabase.cpp
../src/Client.cpp
../src/Server.cpp
)

package_add_test( PRIVMSGHandler_test
./HandlerTests/PRIVMSGHandler_test.cpp
../src/MessageHandlers/PRIVMSGHandler.cpp
)