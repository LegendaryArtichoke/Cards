cmake_minimum_required(VERSION 2.8)
project(cards)

# Took some many of these parts for cmake off
#   https://github.com/RAttab/optics

enable_testing()

add_definitions("-Wall")
add_definitions("-Wextra")
add_definitions("-Wundef")
add_definitions("-Wformat=2")
add_definitions("-Winit-self")
add_definitions("-Wcast-align")
add_definitions("-Wswitch-enum")
add_definitions("-Wwrite-strings")
add_definitions("-Wswitch-default")
add_definitions("-Wunreachable-code")
add_definitions("-Wno-strict-aliasing")
add_definitions("-Wno-format-nonliteral")
add_definitions("-Wno-missing-field-initializers")
add_definitions("-pipe -g -O3 -Werror -march=native")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -std=gnu11")

set(CARD_SOURCES src/cards.c)

add_library(card STATIC ${CARD_SOURCES})

include_directories(include)

function(bds_add_test name)
  add_executable(${name} test/${name}.c)
  target_link_libraries(${name} card)
  add_test(
    NAME    ${name}_valgrind
    COMMAND valgrind --leak-check=full
                     --error-exitcode=1 $<TARGET_FILE:${name}>)
  add_test(${name} ${name})
endfunction(bds_add_test)

bds_add_test(blackjack)
