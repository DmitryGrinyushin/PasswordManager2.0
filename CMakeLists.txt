cmake_minimum_required(VERSION 3.14)
project(PasswordManager2.0)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)
file(GLOB SOURCES "src/*.cpp")

add_executable(PasswordManager ${SOURCES})

find_package(OpenSSL REQUIRED)

target_link_libraries(PasswordManager
    OpenSSL::SSL
    OpenSSL::Crypto
    sqlite3
)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)