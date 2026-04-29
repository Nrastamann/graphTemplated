cmake_minimum_required(VERSION 3.25)

project(GraphsLabs
        LANGUAGES CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)

#sanitizers
option(SANITIZERS "Build with sanitizers" OFF)

add_executable(GraphsLabs src/main.cpp)


if(SANITIZERS)
        set(SANITIZER_FLAGS -g -fsanitize=address -fsanitize=undefined -fsanitize=float-divide-by-zero -fsanitize=leak -fsanitize=pointer-compare -fsanitize=pointer-subtract)
 
  if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_compile_options(-O1)
  endif()
  
  add_link_options(${SANITIZER_FLAGS})
endif()

add_compile_options(${SANITIZER_FLAGS})

set(SRCS
        src/graph.cpp
       src/graph_generation.cpp
       src/triangle_mesh.cpp
       src/config.cpp
       src/material.cpp
)

option(cppcheck "Run CppCheck static code analysis" ON)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")
target_compile_options(GraphsLabs PRIVATE
        -Wall -Wextra -Wpedantic -Wconversion -Weffc++ -Winline -stdlib=libc++)

if(cppcheck)
  find_program(cppcheck_exe NAMES cppcheck REQUIRED)
  set(cppcheck_opts --enable=all --inline-suppr --quiet --suppressions-list=${PROJECT_SOURCE_DIR}/cppcheck.supp)
  set(CMAKE_C_CPPCHECK ${cppcheck_exe} --std=c23 ${cppcheck_opts})
  set(CMAKE_CXX_CPPCHECK ${cppcheck_exe} --std=c++23 ${cppcheck_opts})
endif()

find_library(GLFW_LIBRARY NAMES glfw glfw3 PATHS /usr/lib /usr/local/lib)
find_path(GLFW_INCLUDES glfw3.h /usr/local/include /usr/include/)

find_package(OpenGL REQUIRED)

target_sources(GraphsLabs 
        PRIVATE src/main.cpp ${SRCS}
        PRIVATE
                FILE_SET HEADERS
                BASE_DIRS include ${GLFW_INCLUDES} 
)
target_include_directories(GraphsLabs SYSTEM PRIVATE deps/)
target_link_libraries(GraphsLabs ${GLFW_LIBRARY} OpenGL::GL)

