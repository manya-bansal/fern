cmake_minimum_required(VERSION 3.18)
project(Fern)
set(CMAKE_CXX_STANDARD 17)

set(FERN_PROJECT_DIR ${CMAKE_CURRENT_LIST_DIR})

include_directories(include)

################################
# GINAC
################################
include_directories(/opt/homebrew/include)
link_directories(/opt/homebrew/lib)


################################
# GTest
################################
ADD_SUBDIRECTORY (googletest)
enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# ADD_SUBDIRECTORY (benchmark)

link_directories(/opt/homebrew/lib)

add_executable(test_exec main.cpp)

# Add compiler flags for the test executable
target_compile_options(test_exec PRIVATE
    -Wall
    -Wextra
    -pedantic
    -stdlib=libc++
    -Wc++17-attribute-extensions
    -O3
    -Wl,-no_compact_unwind
    -fvisibility=hidden
)

target_link_libraries(test_exec
${CMAKE_DL_LIBS}
"libginac.a"
${Boost_LIBRARIES})


add_subdirectory(src)
add_subdirectory(test)

