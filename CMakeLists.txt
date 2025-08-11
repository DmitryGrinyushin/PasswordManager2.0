cmake_minimum_required(VERSION 3.14)
project(PasswordManager2.0)

set(CMAKE_CXX_STANDARD 17)

include_directories(include external)

file(GLOB SOURCES "src/*.cpp")

set(CLI_SOURCES "")
foreach(src_file IN LISTS SOURCES)
    if(NOT src_file MATCHES "main_web.cpp$")
        list(APPEND CLI_SOURCES ${src_file})
    endif()
endforeach()

set(WEB_SOURCES "")
foreach(src_file IN LISTS SOURCES)
    if(NOT src_file MATCHES "main.cpp$")
        list(APPEND WEB_SOURCES ${src_file})
    endif()
endforeach()

add_executable(PasswordManager ${CLI_SOURCES})
add_executable(PasswordManagerWeb ${WEB_SOURCES})

add_subdirectory(external/jwt-cpp)

find_package(OpenSSL REQUIRED)

target_link_libraries(PasswordManager
    PRIVATE
    jwt-cpp::jwt-cpp
    OpenSSL::SSL
    OpenSSL::Crypto
    sqlite3
)

target_link_libraries(PasswordManagerWeb
    PRIVATE
    jwt-cpp::jwt-cpp
    OpenSSL::SSL
    OpenSSL::Crypto
    sqlite3
)

set_target_properties(PasswordManager PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)

set_target_properties(PasswordManagerWeb PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)

add_subdirectory(tests)