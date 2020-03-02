cmake_minimum_required(VERSION 3.12)

add_library(saltflake saltflake.cpp saltflake.h saltflakeimpl.cpp saltflakeimpl.h byteswap_compat.h tcb_span.h io/messageparser.cpp io/messageparser.h io/server.h io/server.cpp io/pipe.cpp io/pipe.h)
add_library(saltflake_coverage saltflake.cpp saltflake.h saltflakeimpl.cpp saltflakeimpl.h byteswap_compat.h tcb_span.h io/messageparser.cpp io/messageparser.h io/server.h io/server.cpp io/pipe.cpp io/pipe.h)
target_include_directories(saltflake PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(saltflake_coverage  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(saltflake PRIVATE uv)
target_link_libraries(saltflake_coverage PRIVATE uv)
target_compile_options(saltflake_coverage PRIVATE --coverage)