cmake_minimum_required(VERSION 3.5.0)
project(jsonrpc VERSION 0.1.0)

include_directories(inc)

# mjson library
set(mjson_src pkg/mjson/src/mjson.c)
add_library(mjson STATIC ${mjson_src})
target_include_directories(mjson PUBLIC pkg/mjson/src)
target_compile_options(mjson PUBLIC -Wno-incompatible-pointer-types)

# nats library
add_subdirectory(pkg/nats.c)

# str library
set(str_src	pkg/str/strbuf.c pkg/str/strnum.c pkg/str/strview.c)
add_library(str STATIC ${str_src})
target_include_directories(str PUBLIC pkg/str)
target_compile_definitions(str PUBLIC STRBUF_PROVIDE_PRINTF	STRBUF_DEFAULT_ALLOCATOR_STDLIB	STRBUF_ASSERT_DEFAULT_ALLOCATOR_STDLIB)

# project source
FILE(GLOB_RECURSE src_source src/*.c)

add_executable(${PROJECT_NAME} ${src_source})
target_link_libraries(${PROJECT_NAME} mjson)
target_link_libraries(${PROJECT_NAME} nats_static)
target_link_libraries(${PROJECT_NAME} str)

# demo
set(server_src ./demo/server.c)
add_executable(${PROJECT_NAME}-server ${server_src})
set(client_src ./demo/client.c)
add_executable(${PROJECT_NAME}-client ${client_src})

target_link_libraries(${PROJECT_NAME}-server mjson)
target_link_libraries(${PROJECT_NAME}-server nats_static)
target_link_libraries(${PROJECT_NAME}-client mjson)
target_link_libraries(${PROJECT_NAME}-client nats_static)