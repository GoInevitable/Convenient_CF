Convenient_CF 工具集
=================

项目简介
----

Convenient_CF 是一个正在早期开发阶段的 C++ 多媒体处理工具集，目前主要提供 FFmpeg 相关功能。该项目旨在简化视频格式转换、音频提取等常见多媒体处理任务。

**重要说明：本项目目前处于早期开发阶段，部分头文件（如 `ffmpeg_executor.h`）由 AI 辅助编写，可能存在不完善之处，欢迎社区贡献和改进。**
当前功能

----

### FFmpeg 工具

* FFmpeg 版本检测

* 视频格式转换（单文件）

* 音频提取（规划中）

* 视频合并（规划中）

### 核心特性

* 跨平台支持（Windows/Linux）

* 交互式文件覆盖处理

* 完整的执行输出显示

* 配置文件管理

项目结构
----

    Convenient_CF/
    ├── ffmpeg_executor.h      # FFmpeg 命令执行器（AI 编写）
    ├── SettingsManager.h      # 配置管理器
    ├── file_chooser.h         # 文件选择器
    ├── Path_checker.h         # 路径检查器
    └── main.cpp               # 主程序入口

快速开始
----

### 系统要求

* C++11 或更高版本

* FFmpeg（系统 PATH 或配置指定路径）

* Windows 或 Linux 系统

### 编译运行

```
g++ -std=c++11 main.cpp -o convenient_cf
./convenient_cf
```

使用说明
----

1. 启动程序后选择 FFmpeg 工具

2. 按照提示输入源文件和目标文件路径

3. 程序会自动处理文件覆盖确认

4. 查看完整的 FFmpeg 执行输出

配置选项
----

程序支持通过 `config.ini`文件配置：

* `ffmpeg.path`: FFmpeg 可执行文件路径

* `full_output`: 是否显示完整输出

* `isExecutionConfirmed`: 是否要求执行确认

注意事项
----

⚠️ **早期开发阶段警告**

* 功能尚不完整，仅支持基础视频转换

* 多文件处理和批量操作尚未实现

* 错误处理机制需要完善

* 部分代码由 AI 生成，需要人工审查和优化

贡献指南
----

欢迎提交 Issue 和 Pull Request 来帮助改进项目，特别是：

* 测试和修复 AI 生成代码的问题

* 添加新的多媒体处理功能

* 改进用户界面和交互体验

* 优化代码结构和性能

许可证
---

待定 - 项目早期阶段暂未确定具体许可证。

* * *

_项目创建者: Jane Smith_

_版本: v0.0.1_

_最后更新: 2026年2月_
