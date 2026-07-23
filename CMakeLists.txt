cmake_minimum_required(VERSION 4.2)
project(operative C)

set(CMAKE_C_STANDARD 11)

add_compile_options(-Wall -Wextra -Wpedantic)

add_executable(operative src/main.c
        include/seq.h
        include/error.h
        src/error.c
        src/string/string.c
        include/string/string.h
        src/string/pred.c
        include/string/pred.h
)

target_compile_options(operative PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
target_link_options(operative PRIVATE -fsanitize=address,undefined)

target_include_directories(operative PRIVATE ${PROJECT_SOURCE_DIR}/include)