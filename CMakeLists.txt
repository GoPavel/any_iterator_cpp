cmake_minimum_required(VERSION 2.8)

project(any_iterator)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -Wextra -std=c++14 -fsanitize=address,undefined -fno-sanitize-recover -fstack-protector")

add_executable(test
                     any_iterator.h
                     main.cpp)

add_executable(example
                    any_iterator.h
                    example.cpp)

# useful stuff:
# ------------------------------------------------------------s
# -Wall -Wextra -pedantic -std=c++11 -O2 -Wshadow -Wformat=2 -Wfloat-equal -Wconversion -Wlogical-op -Wshift-overflow=2
# -Wduplicated-cond -Wcast-qual -Wcast-align -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC
# -D_FORTIFY_SOURCE=2 -fsanitize=address -fsanitize=undefined -fno-sanitize-recover -fstack-protector
