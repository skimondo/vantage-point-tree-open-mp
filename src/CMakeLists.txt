add_library(vptree STATIC

  experiments.cpp
  experiments.h

  vpserial.cpp
  vpserial.h

  vpparallel.cpp
  vpparallel.h

  optparser.cpp
  optparser.hpp

  vtkpointcloud.cpp
  vtkpointcloud.h

)
target_include_directories(vptree PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(vptree PUBLIC TBB::tbb Eigen3::Eigen)

add_executable(vantagepoint main.cpp)
target_link_libraries(vantagepoint PRIVATE vptree)

add_executable(bench_naive bench_naive.cpp)
target_link_libraries(bench_naive PUBLIC vptree TBB::tbb Eigen3::Eigen benchmark::benchmark)

add_executable(bench_vptree bench_vptree.cpp)
target_link_libraries(bench_vptree PUBLIC vptree TBB::tbb Eigen3::Eigen)
