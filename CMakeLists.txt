cmake_minimum_required(VERSION 4.2)
project(operative C)

set(CMAKE_C_STANDARD 11)

add_executable(operative src/main.c
        src/string_utils.c
        include/string_utils.h
        src/forms.c
        include/forms.h
        src/root_forms.c
        include/root_forms.h)

target_include_directories(operative PRIVATE ${PROJECT_SOURCE_DIR}/include)