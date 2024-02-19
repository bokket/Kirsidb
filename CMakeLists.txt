cmake_minimum_required(VERSION 3.10.2)
project(bokedb)

# 设置C++标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_COMPILER clang++)

# 设置编译选项
set(CXX_FLAGS
        -O2
        -g2
        -march=native
        -ffast-math
        -std=c++17
        )

# 添加编译选项到所有目标
add_compile_options(${CXX_FLAGS})

# 查找并链接第三方库
find_package(GTest REQUIRED)
find_package(spdlog REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(absl REQUIRED)
find_package(fmt REQUIRED)

# 添加头文件目录
include_directories(src)

# 添加源文件目录
file(GLOB_RECURSE SRC ./SkipList/skiplist.h
        ./SkipList/skiplist.cpp
        )
file(GLOB_RECURSE SRC_WITHOUT_MAIN ./SkipList/skiplist.h
        ./SkipList/skiplist.cpp
        ./Log/Log.h
        ./Log/Log.cpp
        ./db/status.h
        ./file/file_writer.h
        ./file/file_writer.cpp
        ./sstable/block_builder.h
        ./sstable/block_builder.cpp
        ./sstable/block.h
        ./sstable/block.cpp
        ./sstable/coding.h
        ./sstable/coding.cpp
        ./sstable/format.h
        ./bloomfilter/bloom_filter.h
        ./bloomfilter/bloom_filter.cpp
        ./bloomfilter/murmur_hash2.h
        ./bloomfilter/murmur_hash2.cpp
        ./bloomfilter/filter_policy.h)
file(GLOB_RECURSE SRC_TEST ./tests/test_skiplist.cpp)
file(GLOB_RECURSE SRC_STATUS_TEST ./tests/test_status.cpp)
file(GLOB_RECURSE SRC_FILE_WRITER_TEST ./tests/test_file_writer.cpp)
file(GLOB_RECURSE SRC_BLOCK_BUILDER_TEST tests/test_block_builder.cpp)
file(GLOB_RECURSE SRC_BLOCK_TEST tests/test_block.cpp)
file(GLOB_RECURSE SRC_BLOOM_FILTER_TEST tests/test_bloom_filter.cpp)

# 生成可执行文件
add_executable(skiplist-test ${SRC_WITHOUT_MAIN} ${SRC_TEST})
add_executable(status_test ${SRC_WITHOUT_MAIN} ${SRC_STATUS_TEST})
add_executable(file_writer_test ${SRC_WITHOUT_MAIN} ${SRC_FILE_WRITER_TEST})
add_executable(block_builder_test ${SRC_WITHOUT_MAIN} ${SRC_BLOCK_BUILDER_TEST})
add_executable(block_test ${SRC_WITHOUT_MAIN} ${SRC_BLOCK_TEST})
add_executable(bloom_filter_test ${SRC_WITHOUT_MAIN} ${SRC_BLOOM_FILTER_TEST})

# 链接目标库和第三方库
target_link_libraries(skiplist-test PRIVATE GTest::GTest GTest::Main pthread nlohmann_json::nlohmann_json spdlog::spdlog fmt::fmt absl::crc32c absl::crc_internal)

target_link_libraries(status_test PRIVATE GTest::GTest GTest::Main pthread nlohmann_json::nlohmann_json spdlog::spdlog fmt::fmt absl::crc32c absl::crc_internal)

target_link_libraries(file_writer_test PRIVATE GTest::GTest GTest::Main pthread nlohmann_json::nlohmann_json spdlog::spdlog fmt::fmt absl::crc32c absl::crc_internal)

target_link_libraries(block_builder_test PRIVATE GTest::GTest GTest::Main pthread nlohmann_json::nlohmann_json spdlog::spdlog absl::crc32c absl::crc_internal)

target_link_libraries(block_test PRIVATE GTest::GTest GTest::Main pthread nlohmann_json::nlohmann_json spdlog::spdlog absl::crc32c absl::crc_internal)

target_link_libraries(bloom_filter_test PRIVATE GTest::GTest GTest::Main pthread nlohmann_json::nlohmann_json spdlog::spdlog absl::crc32c absl::crc_internal)