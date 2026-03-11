# 设置 CMake 最低版本要求
cmake_minimum_required(VERSION 3.10)

# 设置项目名称
project(MyCProject C)

# 设置 C 标准
set(CMAKE_C_STANDARD 11)

# 包含头文件目录
include_directories(${PROJECT_SOURCE_DIR}/App
                    ${PROJECT_SOURCE_DIR})
                    

# 定义源文件
set(SOURCES
    main.c
    App/binSignature.c
    App/winHal.c
)

# 创建可执行文件
add_executable(signature ${SOURCES})
