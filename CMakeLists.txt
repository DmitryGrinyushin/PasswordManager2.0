cmake_minimum_required(VERSION 3.14)
project(PasswordManager2.0)

set(CMAKE_CXX_STANDARD 17)

include_directories(include external)

file(GLOB SOURCES "src/*.cpp")

list(REMOVE_ITEM SOURCES "${CMAKE_SOURCE_DIR}/src/main_web.cpp")

add_executable(PasswordManager ${SOURCES})

find_package(OpenSSL REQUIRED)

target_link_libraries(PasswordManager
    OpenSSL::SSL
    OpenSSL::Crypto
    sqlite3
)

set_target_properties(PasswordManager PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)

add_executable(PasswordManagerWeb src/main_web.cpp src/WebServer.cpp)

target_link_libraries(PasswordManagerWeb
    OpenSSL::SSL
    OpenSSL::Crypto
    sqlite3
)

set_target_properties(PasswordManagerWeb PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)

add_subdirectory(tests)