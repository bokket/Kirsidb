file(GLOB test-sources "*_test.cpp")

# 遍历每个测试文件，创建可执行文件并链接到 uriex 库
foreach(file-path ${test-sources})
    string( REPLACE ".cpp" "" file-path-without-ext ${file-path} )

    # 获取文件名作为测试名称
     get_filename_component(file-name ${file-path-without-ext} NAME)
    
    # 创建测试可执行文件
    add_executable( ${file-name} ${file-path})

    # 链接 uriex 库
    target_link_libraries(${file-name} PRIVATE liburiex gtest)

    # 注册测试
    add_test(NAME "test-${file-name}" COMMAND ${file-name})
endforeach()

# 启用测试
enable_testing()