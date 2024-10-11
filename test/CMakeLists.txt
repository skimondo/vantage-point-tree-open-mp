enable_testing()
include(CTest)
include(Catch)

add_library(testutils testutils.cpp testutils.h)
target_link_libraries(testutils PUBLIC vptree)

add_executable(test_serial test_serial.cpp)
target_link_libraries(test_serial PRIVATE vptree Catch2::Catch2WithMain)

add_executable(test_parallel test_parallel.cpp)
target_link_libraries(test_parallel PRIVATE vptree Catch2::Catch2WithMain)

catch_discover_tests(test_serial)
catch_discover_tests(test_parallel)
