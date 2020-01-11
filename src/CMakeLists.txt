cmake_minimum_required(VERSION 3.12)

add_library(saltflake saltflake.cpp saltflake.h saltflakeimpl.cpp saltflakeimpl.h byteswap_compat.h io/server.cpp io/server.h tcb_span.h)
add_library(saltflake_coverage saltflake.cpp saltflake.h saltflakeimpl.cpp saltflakeimpl.h byteswap_compat.h io/server.cpp io/server.h tcb_span.h)
target_include_directories(saltflake PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(saltflake_coverage  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_options(saltflake_coverage PRIVATE --coverage)