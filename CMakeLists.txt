cmake_minimum_required(VERSION 3.18)
project(Fern)

set(CMAKE_CXX_STANDARD 17)

set(FERN_PROJECT_DIR ${CMAKE_CURRENT_LIST_DIR})


#add cblas
include_directories(/opt/homebrew/opt/openblas/include)
# add_library(cblas SHARED IMPORTED)
# set_property(TARGET cblas PROPERTY IMPORTED_LOCATION "/opt/homebrew/opt/openblas/lib/libopenblas.a")

FIND_PACKAGE( Boost 1.40 COMPONENTS program_options REQUIRED )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

add_subdirectory(src)
include_directories(include)
# find_package(OpenVDB REQUIRED)


################################
# GTest
################################
ADD_SUBDIRECTORY (googletest)
enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# ADD_SUBDIRECTORY (benchmark)

link_directories(/Users/manyab/openvdb/install_dir/lib/
/opt/homebrew/opt/tbb/lib
/opt/homebrew/lib)

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
"libtbb.dylib"
"libginac.a"
${Boost_LIBRARIES})


add_subdirectory(test)

