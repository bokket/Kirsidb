cmake_minimum_required(VERSION 3.10.2)

# 设置C++标准
#find_program(CLANG_EXECUTABLE NAMES clang clang++)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_COMPILER clang++)
#set(CMAKE_C_COMPILER ${CLANG_EXECUTABLE})
#set(CMAKE_CXX_COMPILER ${CLANG_EXECUTABLE})

project(bokedb)


# 设置编译选项
set(CXX_FLAGS
#        -fsanitize=address
#        -v
#        -g2
        -O2
        -march=native
        -ffast-math
        # -std=c++20
        # -stdlib=libc++
        )
# 添加编译选项到所有目标
add_compile_options(${CXX_FLAGS})

set(CMAKE_BUILD_TYPE Debug)

#https://stackoverflow.com/questions/64821367/clion-wont-run-binary-with-address-sanitizer
add_compile_options(-fsanitize=address -fno-omit-frame-pointer -fsanitize=leak)
add_link_options(-fsanitize=address -fno-omit-frame-pointer -fsanitize=leak)

# 查找并链接第三方库
find_package(Boost COMPONENTS thread system REQUIRED)
find_package(GTest REQUIRED)
find_package(benchmark REQUIRED)
find_package(spdlog REQUIRED)
#find_package(nlohmann_json 3.2.0 REQUIRED)
#find_package(absl REQUIRED)
find_package(fmt REQUIRED)

# 添加头文件目录
include_directories(src)

# 添加源文件目录
file(GLOB_RECURSE SRC ./SkipList/skiplist.h
        ./SkipList/skiplist.cpp
        )

file(GLOB_RECURSE SRC_BLOCK
        ./Log/Log.h
        ./Log/Log.cpp
        ./db/status.h

        ./memtable/comparator.cpp
        ./memtable/comparator.h

        ./tests/random.h
        ./sstable/block_builder.h
        ./sstable/block_builder.cpp
        ./sstable/block_iter.h
        ./sstable/block_iter.cpp
        ./sstable/coding.h
        ./sstable/coding.cpp

        ./iterator/iterator_trait.h
        ./iterator/iterator.h
        )


file(GLOB_RECURSE SRC_CACHE
        ./Log/Log.h
        ./Log/Log.cpp

        ./cache/cache.h
        ./cache/cache.cpp
        )


file(GLOB_RECURSE SRC_SST
        ./Log/Log.h
        ./Log/Log.cpp
        ./db/status.h

        ./file/file_writer.h
        ./file/file_writer.cpp
        ./file/writableFile.h

        ./tests/random.h
        ./db/export.h
        ./sstable/block_builder.h
        ./sstable/block_builder.cpp
        ./sstable/block_iter.h
        ./sstable/block_iter.cpp
        ./sstable/Buf.h
        ./sstable/footer.h
        ./sstable/footer.cpp
        ./sstable/coding.h
        ./sstable/coding.cpp
        ./sstable/format.h
        ./sstable/format.cpp
        ./sstable/SSTable_builder.h
        ./sstable/SSTable_builder.cpp
        ./sstable/SSTable.h
        ./sstable/SSTable.cpp

        ./iterator/iterator_trait.h
        ./iterator/iterator.h

        )

file(GLOB_RECURSE SRC_SST_CACHE
        ./Log/Log.h
        ./Log/Log.cpp
        ./db/status.h

        ./file/file_writer.h
        ./file/file_writer.cpp
        ./file/writableFile.h
        ./file/filename.h
        ./file/filename.cpp
        ./file/tinyio.h
        ./file/tinyio.cpp

        ./tests/random.h
        ./db/export.h
        ./sstable/block_builder.h
        ./sstable/block_builder.cpp
        ./sstable/block_iter.h
        ./sstable/block_iter.cpp
        ./sstable/Buf.h
        ./sstable/footer.h
        ./sstable/footer.cpp
        ./sstable/coding.h
        ./sstable/coding.cpp
        ./sstable/format.h
        ./sstable/format.cpp
        ./sstable/SSTable_builder.h
        ./sstable/SSTable_builder.cpp
        ./sstable/SSTable.h
        ./sstable/SSTable.cpp
        ./sstable/table_cache.h
        ./sstable/table_cache.cpp

        ./iterator/iterator_trait.h
        ./iterator/iterator.h


        ./cache/cache.h
        ./cache/cache.cpp

        )



file(GLOB_RECURSE SRC_WITHOUT_MAIN 
        ./Log/Log.h
        ./Log/Log.cpp
        ./db/status.h

        ./file/file_writer.h
        ./file/file_writer.cpp
        ./file/writableFile.h

        # ./SkipList/skiplist.h
        # ./SkipList/skiplist.cpp
        # ./SkipList/comparator.cpp
        # ./SkipList/comparator.h

        ./write_batch/write_batch.h
        ./write_batch/write_batch.cpp
        ./write_batch/write_batch_impl.h
        ./write_batch/write_batch_impl.cpp

        ./memtable/skiplist.cpp
        ./memtable/skiplist.h
        ./memtable/comparator.cpp
        ./memtable/comparator.h
        ./memtable/internalKeys.h
        ./memtable/internalKeys.cpp
        ./memtable/memtable.h
        ./memtable/memtable.cpp

        ./tests/random.h
        ./db/export.h
        ./sstable/block_builder.h
        ./sstable/block_builder.cpp
        ./sstable/block_iter.h
        ./sstable/block_iter.cpp
        ./sstable/Buf.h
        ./sstable/footer.h
        ./sstable/footer.cpp
#        ./sstable/block.h
#        ./sstable/block.cpp
        ./sstable/coding.h
        ./sstable/coding.cpp
        ./sstable/format.h
        ./sstable/format.cpp
        ./sstable/SSTable_builder.h
        ./sstable/SSTable_builder.cpp
        ./sstable/SSTable.h
        ./sstable/SSTable.cpp

        ./cache/cache.h
        ./cache/cache.cpp
        #./bloomfilter/bloom_filter.h
        #./bloomfilter/bloom_filter.cpp
        #./bloomfilter/murmur_hash2.h
        #./bloomfilter/murmur_hash2.cpp
        #./bloomfilter/filter_policy.h
        ./iterator/iterator_trait.h
        ./iterator/iterator.h
        )

file(GLOB_RECURSE SRC_SKIPLIST_TEST ./tests/skiplist_test.cpp)       
#file(GLOB_RECURSE SRC_SKIPLIST_TEST ./tests/test_skiplist.cpp)
file(GLOB_RECURSE SRC_STATUS_TEST ./tests/test_status.cpp)
file(GLOB_RECURSE SRC_FILE_WRITER_TEST ./tests/test_file_writer.cpp)
file(GLOB_RECURSE SRC_BLOCK_BUILDER_TEST tests/test_block_builder.cpp)
file(GLOB_RECURSE SRC_BLOCK_TEST tests/test_block.cpp)
file(GLOB_RECURSE SRC_FOOTER_TEST tests/test_footer.cpp)
#file(GLOB_RECURSE SRC_BLOOM_FILTER_TEST tests/test_bloom_filter.cpp)
file(GLOB_RECURSE SRC_ITERATOR_TRAIT_TEST tests/test_iterator_trait.cpp)
file(GLOB_RECURSE SRC_CACHE_TEST tests/test_cache.cpp)
file(GLOB_RECURSE SRC_LOG_TEST tests/test_log.cpp)
file(GLOB_RECURSE SRC_SSTABLE_TEST tests/test_SSTable.cpp)
file(GLOB_RECURSE SRC_SSTABLE_BUILD_TEST tests/test_SSTable_build.cpp)

file(GLOB_RECURSE SRC_SST_CACHE_TEST tests/test_table_cache.cpp)


file(GLOB_RECURSE SRC_MEMTABLE_TEST ./tests/test_memtable.cpp)       
file(GLOB_RECURSE SRC_WRITEBATCH_TEST ./tests/test_write_batch.cpp)

# 生成可执行文件
add_executable(skiplist_test ${SRC_WITHOUT_MAIN} ${SRC_SKIPLIST_TEST})
add_executable(status_test ${SRC_WITHOUT_MAIN} ${SRC_STATUS_TEST})
add_executable(file_writer_test ${SRC_WITHOUT_MAIN} ${SRC_FILE_WRITER_TEST})
add_executable(block_builder_test ${SRC_BLOCK} ${SRC_BLOCK_BUILDER_TEST})
add_executable(block_test ${SRC_BLOCK} ${SRC_BLOCK_TEST})
#add_executable(bloom_filter_test ${SRC_WITHOUT_MAIN} ${SRC_BLOOM_FILTER_TEST})
add_executable(iterator_trait_test ${SRC_WITHOUT_MAIN} ${SRC_ITERATOR_TRAIT_TEST})
add_executable(footer_test ${SRC_WITHOUT_MAIN} ${SRC_FOOTER_TEST})

add_executable(cache_test ${SRC_CACHE} ${SRC_CACHE_TEST})

add_executable(log_test ${SRC_WITHOUT_MAIN} ${SRC_LOG_TEST})

add_executable(sstable_test ${SRC_SST} ${SRC_SSTABLE_TEST})

add_executable(sstable_build_test ${SRC_SST} ${SRC_SSTABLE_BUILD_TEST})

add_executable(sstable_cache_test ${SRC_SST_CACHE} ${SRC_SST_CACHE_TEST})

add_executable(memtable_test ${SRC_WITHOUT_MAIN} ${SRC_MEMTABLE_TEST})

add_executable(write_batch_test ${SRC_WITHOUT_MAIN} ${SRC_WRITEBATCH_TEST})

# 链接目标库和第三方库
target_link_libraries(skiplist_test PRIVATE gtest spdlog fmt pthread)

target_link_libraries(status_test PRIVATE gtest spdlog fmt)

target_link_libraries(file_writer_test PRIVATE gtest pthread spdlog fmt)

target_link_libraries(block_builder_test PRIVATE gtest spdlog pthread)

target_link_libraries(block_test PRIVATE gtest spdlog pthread)

target_link_libraries(footer_test PRIVATE gtest spdlog pthread)

#target_link_libraries(bloom_filter_test PRIVATE GTest::GTest GTest::Main pthread nlohmann_json::nlohmann_json spdlog::spdlog absl::crc32c absl::crc_internal)

target_link_libraries(iterator_trait_test PRIVATE gtest spdlog pthread)

target_link_libraries(cache_test PRIVATE gtest spdlog pthread)

target_link_libraries(log_test PRIVATE gtest spdlog pthread)

target_link_libraries(sstable_test PRIVATE gtest spdlog pthread)

target_link_libraries(sstable_build_test PRIVATE gtest spdlog pthread)

target_link_libraries(sstable_cache_test PRIVATE gtest spdlog pthread fmt)

target_link_libraries(memtable_test PRIVATE gtest spdlog pthread fmt)

target_link_libraries(write_batch_test PRIVATE gtest spdlog pthread fmt)