#pans的编译选项文件

# 创建一个接口库，用于集中管理和传递编译配置选项
add_library(pans_options INTERFACE)

# 添加 GCC/Clang 编译器通用警告选项
# -Wall: 开启常见编译警告
# -Wextra: 开启额外警告
# -Wpedantic: 严格按照 C++ 标准检查
# -fno-strict-aliasing: 关闭严格别名优化，避免部分类型转换导致的问题
target_compile_options(pans_options INTERFACE
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:
        -Wall
        -Wextra
        -Wpedantic
        -fno-strict-aliasing
    >
)

# Linux 平台下 GCC/Clang 开启位置无关代码，支持动态库编译
target_compile_options(pans_options INTERFACE
    $<$<AND:$<PLATFORM_ID:Linux>,$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>>:
        -fPIC
    >
)

# Linux 平台下导出符号信息，支持程序运行时生成完整函数调用栈
target_link_options(pans_options INTERFACE
    $<$<AND:$<PLATFORM_ID:Linux>,$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>>:
        -rdynamic
    >
)

# Debug 模式下定义 PANS_DEBUG 宏，用于代码中的调试逻辑判断
target_compile_definitions(pans_options INTERFACE
    $<$<CONFIG:Debug>:PANS_DEBUG>
)

# Debug 模式关闭优化，并生成完整调试信息，方便 GDB 调试
target_compile_options(pans_options INTERFACE
    $<$<CONFIG:Debug>:-O0>
    $<$<CONFIG:Debug>:-g3>
    $<$<CONFIG:Debug>:-ggdb>
)

# Release/RelWithDebInfo 模式通用编译配置
# 开启优化，同时保留必要的调试和性能分析能力
target_compile_options(pans_options INTERFACE
    $<$<CONFIG:Release>:-DNDEBUG>
    $<$<CONFIG:Release>:-O2>
    $<$<CONFIG:Release>:-fno-omit-frame-pointer>

    $<$<CONFIG:RelWithDebInfo>:-DNDEBUG>
    $<$<CONFIG:RelWithDebInfo>:-O2>
    $<$<CONFIG:RelWithDebInfo>:-g>
    $<$<CONFIG:RelWithDebInfo>:-fno-omit-frame-pointer>
)

# 定义代码覆盖率开关，默认关闭
option(ENABLE_COVERAGE "Enable code coverage instrumentation" OFF)

# 开启代码覆盖率统计时，添加覆盖率编译和链接参数
if(ENABLE_COVERAGE)
    target_compile_options(pans_options INTERFACE
        $<$<CONFIG:Debug>:--coverage>
    )

    target_link_options(pans_options INTERFACE
        $<$<CONFIG:Debug>:--coverage>
    )
endif()
