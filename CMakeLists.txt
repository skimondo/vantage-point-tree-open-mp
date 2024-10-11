cmake_minimum_required(VERSION 3.22)

project(inf5171-243-tp2 LANGUAGES C CXX VERSION 1)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
set(BENCHMARK_ENABLE_TESTING FALSE)
add_subdirectory(3rdparty/benchmark)
find_package(Threads REQUIRED)
find_package(Catch2 3 REQUIRED)
find_package(TBB REQUIRED)
find_package(Eigen3 REQUIRED NO_MODULE)

# ThreadSanitizer
option(ENABLE_THREAD_SANITIZER "Enable ThreadSanitizer" OFF)
if(ENABLE_THREAD_SANITIZER)
  add_compile_options(-fsanitize=address)
  add_link_options(-fsanitize=address)
endif()

configure_file(env.sh.in env.sh)
configure_file(tp.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/uqam/tp.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)

add_subdirectory(3rdparty)
add_subdirectory(src)
add_subdirectory(test)

add_custom_target(README SOURCES README.md)

add_custom_target(remise
    COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/remise.py"
        "${CMAKE_PROJECT_NAME}"
        --destdir="${CMAKE_CURRENT_BINARY_DIR}"
)
