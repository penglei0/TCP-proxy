aux_source_directory(. DIR_TCP_SOURCES)
add_library(tcp_module STATIC ${DIR_TCP_SOURCES})
target_link_libraries(tcp_module
    Threads::Threads)