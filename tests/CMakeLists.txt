#收集当前目录下的所有源文件
file(GLOB PANS_TEST_SOURCE_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cxx"
)

#进行一下排序
list(SORT PANS_TEST_SOURCE_FILES)

#寻找多线程库
find_package(Threads REQUIRED)

#设置一下输出目录
set(PANS_TEST_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin/tests") 


#遍历所有的源文件，生成可执行文件
foreach(TEST_SOURCE IN LISTS PANS_TEST_SOURCE_FILES)
    message(STATUS "=========================${TEST_SOURCE}")
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    message(STATUS ">>>>>>>>>>>>>>>>>>>>>>>>>${TEST_NAME}")


    if(TARGET ${TEST_NAME})
        message(FATAL_ERROR "Target ${TEST_NAME} already exists, skipping...")
    endif()


    #生成可执行文件
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    #链接pans库和线程库
    target_link_libraries(${TEST_NAME} PRIVATE pans Threads::Threads)
    #设置输出目录
    set_target_properties(${TEST_NAME} PROPERTIES 
        RUNTIME_OUTPUT_DIRECTORY ${PANS_TEST_OUTPUT_DIRECTORY}
    )

    #多配置生成器默认会增加Debug/Release子目录 这里保持输出路径一致
    foreach(CONFIG IN LISTS CMAKE_CONFIGURATION_TYPES)
        string(TOUPPER ${CONFIG} CONFIG_UPPER)
        set_target_properties(${TEST_NAME} PROPERTIES 
            RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${PANS_TEST_OUTPUT_DIRECTORY}
        )
    endforeach()

    #添加测试
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()
 

message(STATUS "CURRENT SOURCE DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "INNER TESTS DIR: ${CMAKE_CURRENT_SOURCE_DIR}/inner_tests")
message(STATUS "EXISTS: ${EXISTS} ${CMAKE_CURRENT_SOURCE_DIR}/inner_tests")

add_subdirectory(inner_tests)