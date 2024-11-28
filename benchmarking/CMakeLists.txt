cmake_minimum_required(VERSION 3.18)
project(fern_benchmarks)
set(CMAKE_CXX_STANDARD 17)

include_directories(../benchmark/include)
add_executable(fern_benchmarks fern_benchmarks.cpp)

set(GOOGLETEST_PATH ../googletest)
find_package(benchmark REQUIRED PATHS ../benchmark/build)

# Add compiler flags for the test executable
target_compile_options(fern_benchmarks PRIVATE
-Wall
-Wextra
-stdlib=libc++
-O3
-Wl,-no_compact_unwind
)

target_link_libraries(fern_benchmarks
benchmark::benchmark
)
