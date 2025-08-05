cmake_minimum_required(VERSION 3.13)

project(Assortativity)

# setting the build type to debug when in the debug directory and to
# release otherwise
get_filename_component(build_dir ${CMAKE_CURRENT_BINARY_DIR} NAME)
if (${build_dir} MATCHES "debug")
  set(CMAKE_BUILD_TYPE Debug)
else()
  set(CMAKE_BUILD_TYPE Release)
endif()

# c++ version 20
set(CMAKE_CXX_STANDARD 20)

# export compile_commands.json, which can be used by editors for auto
# completion (e.g., using lsp)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# helping the compiler to find header files
include_directories(include lib)

# header, source and executable files
set(HEADERS
  lib/CLI11.hpp
  lib/doctest.h
  include/app.hpp
  include/timer.hpp
  include/random.hpp
  include/types.hpp
  include/random_graph.hpp
  include/kendall.hpp
  include/pearson.hpp
  include/spearman.hpp
  include/edge_list.hpp
  include/histogram.hpp
  include/girg.hpp
  include/ipe.hpp
  )

set(SOURCE_FILES
  src/timer.cpp
  src/random.cpp
  src/types.cpp
  src/random_graph.cpp
  src/kendall.cpp
  src/pearson.cpp
  src/spearman.cpp
  src/edge_list.cpp
  src/histogram.cpp
  src/ipe.cpp
  )

set(EXECUTABLE_FILES
  cli/chung_lu.cpp
  cli/stats.cpp
  cli/random_graph.cpp
  cli/girg.cpp
  cli/edge_degrees.cpp
  cli/degree_distribution.cpp
  cli/joint_degree_distr.cpp
  cli/joint_histogram.cpp
  )

# create a target for every executable linking all source files
add_library(code STATIC ${SOURCE_FILES} ${HEADERS})
target_compile_definitions(code PUBLIC DOCTEST_CONFIG_DISABLE)
foreach (file ${EXECUTABLE_FILES})
  get_filename_component(TARGET ${file} NAME_WE)
  add_executable(${TARGET} ${file})
  target_link_libraries(${TARGET} code)
  target_compile_definitions(${TARGET} PUBLIC DOCTEST_CONFIG_DISABLE)

  # show all warnings
  if(MSVC)
    target_compile_options(${TARGET} PRIVATE /W4 /WX)
  else()
    target_compile_options(${TARGET} PRIVATE -Wall -Wextra -Wpedantic)
  endif()
endforeach ()

# tests
add_library(code_with_tests STATIC ${SOURCE_FILES} ${HEADERS})
add_executable(run_tests cli/run_tests.cpp)
target_link_libraries(run_tests code_with_tests girgs)

# use girgs library
set(OPTION_BUILD_CLI OFF)
set(CMAKE_POLICY_DEFAULT_CMP0120 OLD)
add_subdirectory(../girgs girgs)

target_include_directories(girg PRIVATE ../girgs/source/girgs/include)
target_link_libraries(girg girgs)

