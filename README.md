# RFSuite — 射频测量仪器上位机

基于 Qt 6 / C++17 的仪器监控上位机，用于接收测量设备回传的频谱数据，
完成实时显示、瀑布图回溯、门限报警、历史数据存储与仪器程控。

> 当前状态：**开发中**。本仓库按功能迭代逐次提交，每次提交对应一个可编译的节点。

## 功能设计

| 模块 | 说明 |
| --- | --- |
| 采集层 | 独立 `QThread` worker-object，模拟/真实设备数据帧持续入队 |
| 缓冲层 | 线程安全环形缓冲（`QMutex` + `QWaitCondition`），满时按策略丢帧并计数 |
| 显示层 | `QPainter` 自绘频谱轨迹、瀑布图、门限仪表，双缓冲 + 限帧刷新 |
| 通信层 | TCP 长连接、自定义长度前缀帧协议，处理粘包/半包/超长包/断线重连 |
| 存储层 | SQLite 批量事务写入、历史区间查询与回放 |
| 程控层 | SCPI 风格文本命令控制采集启停与参数下发 |

设计约束：组件之间只通过信号槽通信，UI 线程不触碰采集缓冲；
所有跨线程连接显式指定 `Qt::QueuedConnection`；模块可单独编译为静态控件库复用。

## 目录结构

```
RFSuite/
├── CMakeLists.txt        # 顶层构建脚本（Qt6 + CMake）
├── src/                  # core / widgets / app 分层源码
├── resources/            # QSS 主题与资源文件
├── docs/                 # 架构设计、通信协议、联调记录
├── labs/                 # 各模块独立实验窗口（--lab=N 启动）
└── tests/                # 自测入口（--selftest=N）
```

> 上表是**目标结构**。当前仓库仅有 `README.md`、`.gitignore` 和 `docs/开发计划.md`；
> `CMakeLists.txt`、`src/` 等随迭代逐步补齐。

## 构建

环境：Qt 6.11.2 MinGW 64-bit、CMake ≥ 3.20、GCC ≥ 11、Ninja。

> `CMAKE_PREFIX_PATH` 是本机 Qt 安装目录，换机器时改成自己的路径；
> 也可以先设置同名环境变量，命令行里就不必写死绝对路径。

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_PREFIX_PATH=D:/Qt/6.11.2/mingw_64
cmake --build build --parallel 4
./build/RFSuite.exe                 # 主窗口
./build/RFSuite.exe --selftest=6   # 环形缓冲并发压测（10 万帧）
./build/RFSuite.exe --selftest=16  # 帧协议粘包/半包/异常恢复
```

`build/` 与 `release/` 不纳入版本管理；发布时用 `windeployqt` 补齐运行库。

## 开发约定

- 提交信息用 `dayNN: <做了什么>` / `fix:` / `docs:` / `refactor:`，一条提交只做一件事；
- 每个节点必须能编译通过才提交；
- 阶段结束打 `git tag`，交付版本打 `v1.0.0`；
- 文件头写明职责与设计边界，成员 `m_` 前缀、常量 `k` 前缀，注释随代码同步更新。

详见 [docs/开发计划.md](docs/开发计划.md)。
