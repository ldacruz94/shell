cmake_minimum_required(VERSION 3.20)
project(shell LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(shell 
  src/main.cpp
  src/parser.cpp
  src/builtins.cpp
  src/executor.cpp
)
target_compile_options(shell PRIVATE -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Werror=switch)

option(SANITIZE "Build with AddressSanitizer and UBSan" ON)

if(SANITIZE)
  target_compile_options(shell PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
  target_link_options(shell PRIVATE -fsanitize=address,undefined)
endif()