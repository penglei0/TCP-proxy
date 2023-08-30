aux_source_directory(. DIR_SOURCES)
add_executable(
    tcp_proxy
    main.cc
)
#target_link_libraries(main)