
file(GLOB SRC_FILES *.cpp)

set(FERN_SRC_DIRS . dependency_lang dependency_resolver)

foreach(dir ${FERN_SRC_DIRS})
  file(GLOB SRC_FILES ${SRC_FILES} ${dir}/*.cpp)
endforeach()

# include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(../include)
add_library(fern STATIC ${SRC_FILES})
