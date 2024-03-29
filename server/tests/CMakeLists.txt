macro(package_add_test TESTNAME)
    add_executable(${TESTNAME} ${ARGN})
    target_include_directories (${TESTNAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Mocks)
    target_include_directories (${TESTNAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
    target_include_directories (${TESTNAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include/MessageHandlers)
    target_link_libraries(${TESTNAME} gtest gmock gtest_main shared_lib nlohmann_json::nlohmann_json)
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
../src/LocalServer.cpp
../src/Server.cpp
../src/User.cpp
)
package_add_test( ChannelDatabase_test
ChannelDatabase_test.cpp
../src/ChannelDatabase.cpp
../src/Channel.cpp
)
package_add_test( PASSHandler_test
./HandlerTests/PASSHandler_test.cpp
../src/MessageHandlers/PASSHandler.cpp
../src/MessageHandlers/CommandHandler.cpp
)
package_add_test( NICKHandler_test
./HandlerTests/NICKHandler_test.cpp
../src/MessageHandlers/NICKHandler.cpp
../src/MessageHandlers/CommandHandler.cpp
../src/RemoteUser.cpp
../src/User.cpp
)
package_add_test( USERHandler_test
./HandlerTests/USERHandler_test.cpp
../src/MessageHandlers/USERHandler.cpp
../src/MessageHandlers/CommandHandler.cpp
)
package_add_test( JOINHandler_test
./HandlerTests/JOINHandler_test.cpp
../src/MessageHandlers/JOINHandler.cpp
../src/MessageHandlers/CommandHandler.cpp
)
package_add_test( KILLHandler_test
./HandlerTests/KILLHandler_test.cpp
../src/MessageHandlers/KILLHandler.cpp
../src/MessageHandlers/CommandHandler.cpp
)
package_add_test( MODEHandler_test
./HandlerTests/MODEHandler_test.cpp
../src/MessageHandlers/MODEHandler.cpp
../src/MessageHandlers/CommandHandler.cpp
)
package_add_test( LocalServer_test
LocalServer_test.cpp
../src/Client.cpp
../src/Server.cpp
../src/LocalServer.cpp
)
package_add_test( User_test
User_test.cpp
../src/User.cpp
../src/Client.cpp
)
package_add_test( QUITHandler_test
./HandlerTests/QUITHandler_test.cpp
../src/MessageHandlers/QUITHandler.cpp
../src/MessageHandlers/CommandHandler.cpp
)
package_add_test( ClientUpgrade_test
ClientUpgrade_test.cpp
../src/ClientDatabase.cpp
../src/Client.cpp
../src/LocalServer.cpp
../src/Server.cpp
../src/User.cpp
)

package_add_test( PRIVMSGHandler_test
./HandlerTests/PRIVMSGHandler_test.cpp
../src/MessageHandlers/PRIVMSGHandler.cpp
../src/MessageHandlers/CommandHandler.cpp
)

package_add_test( NAMESHandler_test
./HandlerTests/NAMESHandler_test.cpp
../src/MessageHandlers/NAMESHandler.cpp
../src/MessageHandlers/CommandHandler.cpp
)
package_add_test( RemoteUser_test
RemoteUser_test.cpp
../src/User.cpp
../src/RemoteUser.cpp
)
package_add_test( SERVERHandler_test
./HandlerTests/SERVERHandler_test.cpp
../src/MessageHandlers/SERVERHandler.cpp
../src/MessageHandlers/CommandHandler.cpp
../src/RemoteServer.cpp
../src/Server.cpp
)
package_add_test( NJOINHandler_test
./HandlerTests/NJOINHandler_test.cpp
../src/MessageHandlers/NJOINHandler.cpp
../src/MessageHandlers/CommandHandler.cpp
)