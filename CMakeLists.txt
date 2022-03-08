CMAKE_MINIMUM_REQUIRED(VERSION 3.16)
project(p1.1_chenst_shaoym1)

set(CMAKE_C_FLAGS "-Wpedantic -Wall -Wextra -Werror")

#set(CMAKE_C_FLAGS "-Wpedantic -Wall -Wextra -Werror -std=c89")

add_executable(p1.1_chenst_shaoym1 translator.c translator.h ./src/compression.c ./src/compression.h ./src/utils.c ./src/utils.h)