# AccountManager test
add_executable(test_account_manager
        test_account_manager.cpp
        ../src/UserManager.cpp
        ../src/AccountManager.cpp
        ../src/DatabaseManager.cpp
        ../src/StatementWrapper.cpp
        ../src/Logger.cpp
        ../src/PasswordHasher.cpp
        ../src/EncryptionManager.cpp
)
target_include_directories(test_account_manager PRIVATE ../include)
target_link_libraries(test_account_manager
        OpenSSL::SSL
        OpenSSL::Crypto
        sqlite3
)
set_target_properties(test_account_manager PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# DatabaseManager test
add_executable(test_database_manager
        test_database_manager.cpp
        ../src/DatabaseManager.cpp
        ../src/Logger.cpp
)
target_include_directories(test_database_manager PRIVATE ../include)
target_link_libraries(test_database_manager
        sqlite3
)
set_target_properties(test_database_manager PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# UserManager test
add_executable(test_user_manager
        test_user_manager.cpp
        ../src/UserManager.cpp
        ../src/DatabaseManager.cpp
        ../src/StatementWrapper.cpp
        ../src/Logger.cpp
        ../src/PasswordHasher.cpp
)
target_include_directories(test_user_manager PRIVATE ../include)
target_link_libraries(test_user_manager
        OpenSSL::SSL
        OpenSSL::Crypto
        sqlite3
)
set_target_properties(test_user_manager PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)