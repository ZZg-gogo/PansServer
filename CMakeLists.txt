cmake_minimum_required(VERSION 3.25)

#指定编译器版本
set(CMAKE_C_COMPILER "/usr/bin/gcc" CACHE FILEPATH "C compiler" FORCE)
set(CMAKE_CXX_COMPILER "/usr/bin/g++" CACHE FILEPATH "C++ compiler" FORCE)

# 设置项目使用 C++20 标准进行编译
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)


# 定义项目名称、版本号，并指定使用 C++ 和 C 语言
project(PansServer VERSION 0.1.0 LANGUAGES CXX C)
# 引入 GNU 标准目录变量，用于获取系统安装路径（如 bin、lib、include）
include(GNUInstallDirs)
# 引入 CMake 包配置辅助模块，用于生成项目安装后的 Config.cmake 文件
include(CMakePackageConfigHelpers)

#编译开关
option(PANS_BUILD_EXAMPLES "Build examples" ON)
option(PANS_BUILD_TESTS    "Build tests"    ON)
option(PANS_BUILD_TOOLS    "Build tools"    ON)

# 如果当前没有指定多配置生成器，并且没有设置编译类型，则默认使用 Debug 模式
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif()

# 设置 CMAKE_BUILD_TYPE 可选的编译类型列表
# Debug: 调试模式，包含调试信息
# Release: 发布模式，开启优化
# RelWithDebInfo: 带调试信息的优化模式
# MinSizeRel: 最小体积优化模式
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)

# 输出当前使用的编译类型
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")

# 获取当前服务器的逻辑 CPU 核心数量（包含超线程）
cmake_host_system_information(RESULT CPU_NUMS QUERY NUMBER_OF_LOGICAL_CORES)

# 输出当前服务器 CPU 核心数量
message(STATUS "CPU NUMS on current server is: ${CPU_NUMS}")

# 根据 CPU 核心数计算并行编译任务数量，这里设置为 CPU 核心数的 1.5 倍
math(EXPR JOBS "${CPU_NUMS} + ${CPU_NUMS} / 2")

# 设置 CMake 默认并行编译线程数，提高项目构建速度
set(CMAKE_BUILD_PARALLEL_LEVEL ${JOBS} CACHE STRING "并行编译线程数")

# 输出默认编译并行任务数量
message(STATUS "Compiler default work jobs: ${CMAKE_BUILD_PARALLEL_LEVEL}")


# 引入项目自定义配置选项，例如是否编译测试、工具等
include(cmake/pansOptions.cmake)

# 添加核心项目源码目录
add_subdirectory(pans)

# 如果开启测试编译，则启用 CTest 并添加测试代码目录
if(PANS_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# 如果开启工具编译，则添加工具程序目录
if(PANS_BUILD_TOOLS)
    add_subdirectory(tools)
endif()