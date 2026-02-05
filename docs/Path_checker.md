# FileTypeChecker 技术文档

## 概述

`FileTypeChecker` 是一个轻量级的 C++ 文件类型检测库，用于快速识别视频、音频、目录和其他类型的文件。基于 C++17 的 `std::filesystem` 实现，提供高效的文件扩展名检测和异常安全处理。

## 头文件依赖

```cpp
#include <string>
#include <unordered_set>
#include <filesystem>
```

## 核心 API

### 文件类型枚举

```cpp
enum class FileType {
    VIDEO,      // 视频文件
    AUDIO,      // 音频文件
    DIRECTORY,  // 文件夹
    OTHER       // 其他类型文件
};
```

### 主要方法

#### `checkFileType(const std::string& path)`

**功能**：检测指定路径的文件类型

**参数**：
- `path`：文件路径字符串

**返回值**：对应的 `FileType` 枚举值

**异常处理**：内置异常捕获，遇到无效路径返回 `FileType::OTHER`

#### `fileTypeToString(FileType type)`

**功能**：将文件类型枚举转换为可读字符串

**参数**：
- `type`：文件类型枚举

**返回值**：对应的中文字符串描述

## 支持的文件格式

### 视频文件扩展名
```
.mp4, .avi, .mkv, .mov, .wmv, .flv, .webm, .m4v
.mpg, .mpeg, .3gp, .mts, .m2ts, .vob, .ogv, .qt
.rm, .rmvb, .asf, .swf, .f4v
```

### 音频文件扩展名
```
.mp3, .wav, .flac, .aac, .ogg, .wma, .m4a, .opus
.aiff, .alac, .amr, .ape, .au, .mid, .midi, .ra
.ram, .voc, .weba
```

## 使用示例

### 基本用法

```cpp
#include "FileTypeChecker.h"

// 检测文件类型
filecheck::FileType type = filecheck::FileTypeChecker::checkFileType("video.mp4");

// 获取类型描述
std::string desc = filecheck::FileTypeChecker::fileTypeToString(type);
// desc = "视频文件"
```

### 批量处理示例

```cpp
std::vector<std::string> files = {"video.mp4", "audio.mp3", "folder/"};

for (const auto& file : files) {
    auto type = filecheck::FileTypeChecker::checkFileType(file);
    std::cout << file << " -> " << filecheck::FileTypeChecker::fileTypeToString(type) << std::endl;
}
```

## 扩展指南

### 添加新的文件类型

1. 在 `FileType` 枚举中添加新类型
2. 添加对应的扩展名集合静态成员
3. 在 `checkFileType` 方法中添加检测逻辑
4. 在 `fileTypeToString` 方法中添加字符串映射

### 示例：添加图片类型支持

```cpp
// 1. 添加枚举
enum class FileType {
    VIDEO, AUDIO, IMAGE, DIRECTORY, OTHER
};

// 2. 添加静态成员
static const std::unordered_set<std::string> imageExtensions;

// 3. 在 checkFileType 中添加检测
if (imageExtensions.find(ext) != imageExtensions.end()) {
    return FileType::IMAGE;
}

// 4. 在 fileTypeToString 中添加映射
case FileType::IMAGE: return "图片文件";
```

## 编译和测试

### 编译要求
- C++17 或更高版本
- 支持 `std::filesystem` 的标准库

### 运行测试

定义 `FILE_TYPE_CHECKER_TEST` 宏来启用测试用例：

```bash
g++ -std=c++17 -DFILE_TYPE_CHECKER_TEST -o test FileTypeChecker.h
./test
```

## 性能特点

- **高效检测**：使用 `unordered_set` 进行 O(1) 复杂度的扩展名查找
- **大小写不敏感**：自动将扩展名转换为小写进行比较
- **异常安全**：内置异常处理，避免程序崩溃
- **路径兼容**：支持跨平台路径格式（Windows/Linux）

## 注意事项

1. 该检测基于文件扩展名，不是真正的文件内容验证
2. 对于符号链接，会检测链接指向的实际文件类型
3. 空路径或无效路径统一返回 `FileType::OTHER`
4. 扩展名集合为静态常量，在程序生命周期内保持不变

## 版本信息

- **当前版本**：1.0
- **最后更新**：2026-02-05
- **命名空间**：`filecheck`
- **作者**：腾讯AI助手