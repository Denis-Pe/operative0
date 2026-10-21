cmake_minimum_required(VERSION 3.20)
project(operative LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_library(operative_options INTERFACE)

if(MSVC)
    target_compile_options(operative_options INTERFACE /W4)
else()
    target_compile_options(operative_options INTERFACE
            -Wall -Wextra -Wpedantic
            $<$<CONFIG:Debug>:-fsanitize=address,undefined -fno-omit-frame-pointer>
    )
    target_link_options(operative_options INTERFACE
            $<$<CONFIG:Debug>:-fsanitize=address,undefined>
    )
endif()

add_library(operative_core STATIC
        src/error.c
        src/string/string.c
        src/string/pred.c
        src/parsing.c
        src/runtime/value.c
        src/runtime/env.c
        src/surface.c
        src/miscutil.c
)
target_include_directories(operative_core PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(operative_core PUBLIC operative_options)

add_executable(operative src/main.c)
target_link_libraries(operative PRIVATE operative_core)

# Tests
include(CTest)

if(BUILD_TESTING)
    add_library(unity STATIC src/unity.c)
    target_include_directories(unity PUBLIC ${PROJECT_SOURCE_DIR}/include)

    add_executable(t-string src/tests/t-string.c)
    target_link_libraries(t-string PRIVATE operative_core unity)
    add_test(NAME string COMMAND t-string)
endif()
