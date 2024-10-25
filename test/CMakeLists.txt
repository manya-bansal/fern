file(GLOB TEST_SOURCES *.cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_executable(all_tests ${TEST_SOURCES})



# Add compiler flags for the test executable
target_compile_options(all_tests PRIVATE
-Wall
-Wextra
-stdlib=libc++
-O3
-Wl,-no_compact_unwind
)

target_link_libraries(all_tests
gtest_main
"libginac.a"
# benchmark::benchmark
${CMAKE_DL_LIBS}
fern
)

