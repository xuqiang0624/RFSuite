# ============================================================================
# RFSuite - Qt 6 / CMake 顶层构建脚本
#
# 职责：查找 Qt 依赖、收集源码、定义可执行目标与链接库。
# 约定：
#   1. 新增源文件放在 src/ 下即可被递归收集，resources/ 下的 .qrc 同样纳入；
#      含 Q_OBJECT 的头文件必须列入目标源文件，AUTOMOC 才会生成 moc。
#   2. Qt 组件按模块规划预先链接，避免每个模块接入时反复改构建脚本。
#   3. 同一份脚本在 Windows / Linux 下使用，路径由 CMake 推导。
# ============================================================================
cmake_minimum_required(VERSION 3.20)

project(RFSuite VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 1. Qt 的三把自动化开关
#    AUTOMOC：扫描 Q_OBJECT 生成 moc_*.cpp
#    AUTOUIC：把 .ui 编译成 ui_*.h（当前为代码布局，保留该能力）
#    AUTORCC：把 .qrc 编译成 qrc_*.cpp
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# 2. 按模块规划预先链接所需组件，后续通信与存储模块接入时无需改脚本。
find_package(Qt6 REQUIRED COMPONENTS Widgets Network Sql)

# 3. 递归收集源码；CONFIGURE_DEPENDS 在文件增删时自动触发重新配置。
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/resources/*.qrc"
)

# 4. 保留控制台输出以便查看运行日志；发布时再切换 WIN32 子系统。
add_executable(RFSuite ${PROJECT_SOURCES})

# 统一包含根：源码使用 #include "core/xxx.h" / "app/xxx.h" 形式
target_include_directories(RFSuite PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(RFSuite PRIVATE
    Qt6::Widgets
    Qt6::Network
    Qt6::Sql
)

# 5. 警告保持严格档位，新增代码需清零后提交
#    MSVC 的 /utf-8 很关键：源码里有大量中文注释，不加会在部分代码页下报错。
if(MSVC)
    target_compile_options(RFSuite PRIVATE /W4 /utf-8)
else()
    target_compile_options(RFSuite PRIVATE -Wall -Wextra)
endif()
