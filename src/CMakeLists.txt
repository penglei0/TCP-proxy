aux_source_directory(. DIR_SOURCES)
#add_subdirectory(tun)
#add_subdirectory(tcp)
#add_subdirectory(poll)
add_subdirectory(base)
add_executable(
    tcp_proxy
    main.cc
)
#target_link_libraries(main)