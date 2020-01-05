cmake_minimum_required(VERSION 3.12)

add_library(saltflake saltflake.cpp saltflake.h saltflakeimpl.cpp saltflakeimpl.h byteswap_compat.h)
add_library(saltflake_coverage saltflake.cpp saltflake.h saltflakeimpl.cpp saltflakeimpl.h byteswap_compat.h)
target_compile_options(saltflake_coverage PRIVATE --coverage)