cmake_minimum_required(VERSION 3.18)

project(${PROJECT_NAME} LANGUAGES C VERSION 1.0.0)
add_compile_options(-Wall -Wextra -Wpedantic -Werror)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/58d77fa8070e8cec2dc1ed015d66b454c8d78850.zip
)
FetchContent_MakeAvailable(googletest)

set(SRC 
src/main.c
src/rpc.c)

add_executable(
  main
  ${SRC}
)

target_include_directories(main PRIVATE include)
