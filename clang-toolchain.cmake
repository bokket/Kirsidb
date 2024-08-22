set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS_INIT "-stdlib=libc++")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "-lc++ -lc++abi")
set(CMAKE_EXE_LINKER_FLAGS_INIT "-lc++ -lc++abi")
set(CMAKE_MODULE_LINKER_FLAGS_INIT "-lc++ -lc++abi")


add_compile_options(-fsanitize=address -fno-omit-frame-pointer -fsanitize=leak)
add_link_options(-fsanitize=address -fno-omit-frame-pointer -fsanitize=leak)
# 设置编译选项
set(CXX_FLAGS
    -O2
    -march=native
    -ffast-math
    )
# 添加编译选项到所有目标
add_compile_options(${CXX_FLAGS})
set(CMAKE_BUILD_TYPE Debug)