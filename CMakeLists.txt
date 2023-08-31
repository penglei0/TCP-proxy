cmake_minimum_required(VERSION 3.22)

project(tcp_proxy)
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "
    FATAL: In-source builds are not allowed.
           You should create a separate directory for build files.
           ")
endif()

# Generating a compilation database: compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

message(STATUS "CMAKE Version: ${CMAKE_VERSION}")
message(STATUS "System name: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Host System name: ${CMAKE_HOST_SYSTEM_NAME}")
message(STATUS "System version: ${CMAKE_SYSTEM_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C compiler: ${CMAKE_C_COMPILER}")
message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")

# Set build-specific compiler flags
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Werror -O3")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -g -O3")

include(FetchContent)
## for duplicated build of gtest
cmake_policy(SET CMP0079 NEW)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
  FIND_PACKAGE_ARGS
)
# uncomment this line if you want to use the source code of googletest without downloading it.
set(FETCHCONTENT_SOURCE_DIR_GOOGLETEST ${CMAKE_BINARY_DIR}/_deps/googletest-src)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

set(gmock_build_tests OFF CACHE BOOL "" FORCE)
set(gtest_build_tests OFF CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)
message("GoogleTest source directory is :" ${googletest_SOURCE_DIR})
include_directories(${googletest_SOURCE_DIR}/googletest/include)

include_directories(src)
add_subdirectory(src)