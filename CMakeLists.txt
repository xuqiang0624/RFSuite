# ============================================================================
# RFSuite - Qt 6 / CMake 顶层构建脚本
#
# 职责：查找 Qt 依赖、收集源码、定义可执行目标与链接库。
# 约定：新增源文件放在 src/ 下即可被自动收集，无需修改本文件；
#       含 Q_OBJECT 的头文件必须纳入目标源文件，AUTOMOC 才会为其生成 moc。
# ============================================================================
cmake_minimum_required(VERSION 3.20)

project(RFSuite VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# AUTOMOC / AUTOUIC / AUTORCC 分别处理 Q_OBJECT、.ui 与 .qrc；
# 当前使用代码布局，AUTOUIC 为后续表单类界面预留能力。
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

add_executable(RFSuite ${PROJECT_SOURCES})

# 统一包含根：源码使用 #include "core/xxx.h" / "app/xxx.h" 形式
target_include_directories(RFSuite PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(RFSuite PRIVATE Qt6::Widgets)

# 警告保持严格档位，新增代码需清零后提交
if(MSVC)
    target_compile_options(RFSuite PRIVATE /W4 /utf-8)
else()
    target_compile_options(RFSuite PRIVATE -Wall -Wextra)
endif()
