# 设置cmake最低版本与项目名称
cmake_minimum_required(VERSION 3.0)
project(mprpc)

# 设置gdb编译模式：debug
set(CMAKE_BUILD_TYPE "Debug")

# 设置调试信息以及启动C++11语言标准
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11")
# 设置项目可执行文件的输出地址
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
# 设置项目库文件的输出地址
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# 设置项目头文件搜索路径 -I
include_directories(${PROJECT_SOURCE_DIR}/src/include)
include_directories(${PROJECT_SOURCE_DIR}/example)
include_directories(${PROJECT_SOURCE_DIR}/protobuf)

# 设置项目库文件搜索路径 -L
link_directories(${PROJECT_SOURCE_DIR}/lib)

# src包含了mprpc框架的所有相关源代码
add_subdirectory(src)
# example包含了mprpc框架的所有示例代码
add_subdirectory(example)


