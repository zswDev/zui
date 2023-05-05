
cmake_minimum_required(VERSION 3.0.0)

find_program(CCACHE_FOUND ccache) #开启ccahe
if(CCACHE_FOUND)
    message(STATUS "ccache_exists")
    # set(CMAKE_CXX_COMPILER ccache g++)
    # SET(CMAKE_C_COMPILER ccache gcc)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)


set(CMAKE_CXX_STANDARD 17) #c++版本

# set(CMAKE_UNITY_BUILD ON) #集成编译

# set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -flto")
#set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -g -Wall") #gcc参数
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3  -Wall")

#set(CMAKE_EXE_LINKER_FLAGS "-static" )  #链接参数
#set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -lssl")

set(CMAKE_TOOLCHAIN_FILE /home/code/download/vcpkg/scripts/buildsystems/vcpkg.cmake) # 设置vcpkg

project(zui VERSION 0.0.1)

include_directories(include) #本地头文件
# include_directories(/usr/local/include)

link_directories(lib/quickjs)
#link_directories(/usr/local/lib)

add_executable(main 
                ./src/compile.cpp
                ./src/fs.cpp
                ./src/vm.cpp
                ./src/evt_bus.cpp
                main.cpp) #添加可执行文件

#quickjs
target_link_libraries(main PRIVATE quickjs dl)

# set_target_properties(main PROPERTIES UNITY_BUILD ON)  #集成编译

target_precompile_headers(main 
    PRIVATE
    <iostream>
    <string>
    <fstream>
    <quickjs/quickjs.h>
    <quickjs/quickjs-libc.h>) #预编译头
# [["httplib.h"]]

# find_package(Boost REQUIRED COMPONENTS algorithm)
# include_directories(${Boost_INCLUDE_DIRS})
# target_link_libraries(main PRIVATE Boost::algorithm)

# set(BOOST_ROOT /home/code/downlaod/vcpkg/installed/x64-linux)

# find_package(Boost COMPONENTS system REQUIRED)
# if(Boost_FOUND)
#     include_directories(${Boost_INCLUDE_DIRS})    
#     # MESSAGE( STATUS "Boost_INCLUDE_DIRS = ${Boost_INCLUDE_DIRS}.")
#     # MESSAGE( STATUS "Boost_LIBRARIES = ${Boost_LIBRARIES}.")
#     # MESSAGE( STATUS "Boost_LIB_VERSION = ${Boost_LIB_VERSION}.")
#     target_link_libraries(main PRIVATE ${Boost_LIBRARIES})
# endif()

find_package(PkgConfig)
pkg_search_module(GLIB2 REQUIRED glib-2.0)
# message(STATUS ${GLIB2_LIBRARY_DIRS})
# message(STATUS ${GLIB2_LIBRARIES})
include_directories(${GLIB2_INCLUDE_DIRS})
link_directories(${GLIB2_LIBRARY_DIRS})
target_link_libraries(main PRIVATE ${GLIB2_LIBRARIES})

pkg_search_module(GTK3 REQUIRED gtk+-3.0)
# message(STATUS ${GTK3_LIBRARY_DIRS})
include_directories(${GTK3_INCLUDE_DIRS})
target_link_libraries(main PRIVATE ${GTK3_LIBRARIES})

pkg_search_module(WEBKIT4 REQUIRED webkit2gtk-4.0)
# message(STATUS ${WEBKIT4_INCLUDE_DIRS})
include_directories(${WEBKIT4_INCLUDE_DIRS})
target_link_libraries(main PRIVATE ${WEBKIT4_LIBRARIES})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(main PRIVATE Threads::Threads)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(main PRIVATE nlohmann_json::nlohmann_json)
set(nlohmann-json_IMPLICIT_CONVERSIONS OFF)

