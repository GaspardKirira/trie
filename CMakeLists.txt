cmake_minimum_required(VERSION 3.20)
project(trie LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(trie INTERFACE)
add_library(trie::trie ALIAS trie)

target_include_directories(trie INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

if (MSVC)
  target_compile_options(trie INTERFACE /W4 /permissive-)
else()
  target_compile_options(trie INTERFACE -Wall -Wextra -Wpedantic)
endif()

include(CTest)
enable_testing()

option(TRIE_BUILD_EXAMPLES "Build examples" ON)

if (TRIE_BUILD_EXAMPLES)
  add_executable(trie_example_basic examples/basic_usage.cpp)
  target_link_libraries(trie_example_basic PRIVATE trie)

  add_executable(trie_example_autocomplete examples/autocomplete.cpp)
  target_link_libraries(trie_example_autocomplete PRIVATE trie)

  add_executable(trie_example_ranked examples/ranked_search.cpp)
  target_link_libraries(trie_example_ranked PRIVATE trie)
endif()

add_executable(trie_basic_test tests/test_basic.cpp)
target_link_libraries(trie_basic_test PRIVATE trie::trie)
add_test(NAME trie.basic COMMAND trie_basic_test)
