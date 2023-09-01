aux_source_directory(. DIR_SOURCES)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_subdirectory(tcp)
add_executable(
    tcp_proxy
    ${DIR_SOURCES}
)
target_link_libraries(tcp_proxy
    tcp_module
    Threads::Threads)