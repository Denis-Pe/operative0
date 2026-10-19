cmake_minimum_required(VERSION 3.30)
project(operative C)

enable_testing()

set(CMAKE_C_STANDARD 11)

add_executable(operative src/main.c
        include/seq.h
        include/error.h
        src/error.c
        src/string/string.c
        include/string/string.h
        src/string/pred.c
        include/string/pred.h
        src/parsing.c
        include/parsing.h
        src/runtime/value.c
        include/runtime/value.h
        src/runtime/env.c
        include/runtime/env.h
        src/surface.c
        include/surface.h
        src/miscutil.c
        include/miscutil.h
)

add_executable(tests
        include/seq.h
        include/error.h
        src/error.c
        src/string/string.c
        include/string/string.h
        src/string/pred.c
        include/string/pred.h
        src/parsing.c
        include/parsing.h
        src/runtime/value.c
        include/runtime/value.h
        src/runtime/env.c
        include/runtime/env.h
        src/surface.c
        include/surface.h
        src/miscutil.c
        include/miscutil.h
        include/unity.h
        include/unity_internals.h
        src/unity.c
        src/tests/t-string.c
)

add_test(NAME tests COMMAND tests)

if(NOT MSVC)
    add_compile_options(-Wall -Wextra -Wpedantic)
    target_compile_options(operative PRIVATE
            $<$<CONFIG:Debug>:-fsanitize=address,undefined>
            $<$<CONFIG:Debug>:-fno-omit-frame-pointer>
    )

    target_link_options(operative PRIVATE
            $<$<CONFIG:Debug>:-fsanitize=address,undefined>
    )


    target_compile_options(tests PRIVATE
            $<$<CONFIG:Debug>:-fsanitize=address,undefined>
            $<$<CONFIG:Debug>:-fno-omit-frame-pointer>
    )

    target_link_options(tests PRIVATE
            $<$<CONFIG:Debug>:-fsanitize=address,undefined>
    )
endif()

target_include_directories(operative PRIVATE ${PROJECT_SOURCE_DIR}/include)

target_include_directories(tests PRIVATE ${PROJECT_SOURCE_DIR}/include)