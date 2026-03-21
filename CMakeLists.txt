cmake_minimum_required(VERSION 3.20)
project(pls LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(CURL REQUIRED)

add_executable(pls main.cpp)
target_link_libraries(pls PRIVATE CURL::libcurl)

# Включаем LLD, если компилятор Clang
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_link_options(pls PRIVATE -fuse-ld=lld)
    # Можно добавить флаги для более строгой проверки кода
    target_compile_options(pls PRIVATE -Wall -Wextra)
endif()
