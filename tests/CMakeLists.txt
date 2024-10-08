file(GLOB_RECURSE SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/../file/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Log/*.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/../sstable/*.cpp"
    # "${CMAKE_CURRENT_SOURCE_DIR}/../iterator/*.cpp"
    # "${CMAKE_CURRENT_SOURCE_DIR}/../memtable/*.cpp"
)

# 添加可执行目标和链接库
add_library(bokedb ${SOURCES})

target_include_directories(bokedb PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../Log)
target_include_directories(bokedb PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../file)
target_include_directories(bokedb PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../sstable)
# target_include_directories(bokedb PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../iterator)
# target_include_directories(bokedb PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../memtable)

file(GLOB test-sources "test_*.cpp")
foreach(file-path ${test-sources})
    string( REPLACE ".cpp" "" file-path-without-ext ${file-path} )
    get_filename_component(file-name ${file-path-without-ext} NAME)
    add_executable( ${file-name} ${file-path})
    target_link_libraries(${file-name} PRIVATE bokedb gtest spdlog pthread)

    add_test(NAME "test-${file-name}" COMMAND ${file-name})
endforeach()