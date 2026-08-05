# 递归搜索项目中的所有 C/C++ 源文件
# CONFIGURE_DEPENDS: 当新增或删除源文件时，自动重新触发 CMake 配置
file(GLOB_RECURSE PANS_SOURCES CONFIGURE_DEPENDS *.cpp *.cc *.c)

# 递归搜索项目中的所有头文件
file(GLOB_RECURSE PANS_HEADERS CONFIGURE_DEPENDS *.h *.hpp)

# 创建静态库 pans，将源文件和头文件编译到静态库中
add_library(pans STATIC ${PANS_SOURCES} ${PANS_HEADERS})


# 设置 pans 库的头文件搜索路径
target_include_directories(pans
    PUBLIC
        # 编译阶段使用源码目录下的 include
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>

        # 安装后使用系统安装目录下的 include
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>

    PRIVATE
        # 库内部源码使用的私有头文件目录
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)


# 链接 pans_options 接口库
# 继承其中定义的编译选项、链接选项和宏定义
target_link_libraries(pans PUBLIC pans_options)


# 为 pans 主库添加额外编译选项
target_compile_options(pans PRIVATE
    # GCC/Clang 下开启严格编译检查
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:
        -Werror          # 将所有警告视为编译错误，保证代码质量
    >
)


# 安装 pans 静态库
install(TARGETS pans
    EXPORT pansTargets

    # 静态库安装目录，例如 Linux 下的 lib/
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}

    # 动态库安装目录，例如 .so 文件
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}

    # 可执行文件安装目录
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)


# 安装头文件到指定 include 目录
install(FILES ${PANS_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/pans
)