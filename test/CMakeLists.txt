file(GLOB TEST_SOURCES *.cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_executable(all_tests ${TEST_SOURCES})

include_directories(/Users/maggie/.pyenv/versions/3.12.5/lib/python3.12/site-packages/pybind11/include)
include_directories(/Library/Frameworks/Python.framework/Versions/3.12/Headers)


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
"libpython3.12.dylib"
# benchmark::benchmark
${CMAKE_DL_LIBS}
fern
)

target_link_options(all_tests PRIVATE
-undefined dynamic_lookup
-L/Library/Frameworks/Python.framework/Versions/3.12/lib 
)

