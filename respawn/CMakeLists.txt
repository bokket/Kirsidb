# 创建接口库，包含头文件
add_library(librespawn INTERFACE)

# 设置头文件的包含路径
target_include_directories(librespawn INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# 添加 test 子目录
add_subdirectory(test)