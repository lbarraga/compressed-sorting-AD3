cmake_minimum_required(VERSION 3.25)
project(projectAD3 C)

set(CMAKE_C_STANDARD 17)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
#set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -fsanitize=address")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")

include_directories(${PROJECT_SOURCE_DIR}/tests)

add_executable(projectAD3
        src/main.c

        src/compress/compress.c
        src/extract/extract.c

        src/sort/sort.c
        src/tree/tree.c

        src/tree/make_OPC.c
        src/tree/make_OPC.h

        src/tree/hu_tucker.c
        src/tree/hu_tucker.h

        src/tree/linkedList/linkedlist.c
        src/tree/linkedList/linkedlist.h
)

# Add test executable
add_executable(MyTest
        src/tests/test_example.c
)
