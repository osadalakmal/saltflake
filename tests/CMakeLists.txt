cmake_minimum_required(VERSION 3.12)

file(GLOB_RECURSE TEST_SRC *.t.cpp)
add_executable(saltflake.t.tsk ${TEST_SRC})
target_include_directories(saltflake.t.tsk PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(saltflake.t.tsk saltflake gtest gtest_main)
gtest_discover_tests(saltflake.t.tsk)

add_executable(saltflake.coverage.t.tsk ${TEST_SRC})
target_include_directories(saltflake.coverage.t.tsk PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(saltflake.coverage.t.tsk saltflake_coverage gtest gtest_main --coverage)
target_compile_options(saltflake.coverage.t.tsk PRIVATE --coverage)
#gtest_discover_tests(saltflake.coverage.t.tsk)
