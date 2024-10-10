
file(GLOB SRC_FILES *.cpp)

set(FERN_SRC_DIRS . dependency_lang dependency_resolver codegen)

foreach(dir ${FERN_SRC_DIRS})
  file(GLOB SRC_FILES ${SRC_FILES} ${dir}/*.cpp)
endforeach()

# include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(../include)
include_directories(/Users/maggie/.pyenv/versions/3.12.5/lib/python3.12/site-packages/pybind11/include)
include_directories(/Library/Frameworks/Python.framework/Versions/3.12/Headers)
add_library(fern STATIC ${SRC_FILES})
set(pybind11_DIR /Users/maggie/.pyenv/versions/3.12.5/lib/python3.12/site-packages/pybind11/share/cmake/pybind11)
find_package(pybind11 REQUIRED)

link_directories(/opt/homebrew/Cellar/tbb/2021.13.0/lib
/opt/homebrew/Cellar/ginac/1.8.7/lib)
pybind11_add_module(fern_py ${SRC_FILES})
target_link_libraries(fern_py PRIVATE "libginac.a")
