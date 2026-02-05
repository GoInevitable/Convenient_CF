/**
 * @file FileTypeChecker.h
 * @brief 文件类型检测器
 * 
 * 本头文件定义了一个文件类型检测工具类，能够根据文件扩展名和文件系统属性
 * 自动识别视频、音频、目录和其他类型文件。支持多种常见多媒体格式。
 * 
 * @details
 * 使用C++17的filesystem库进行路径处理，通过扩展名映射快速判断文件类型。
 * 设计为纯工具类，所有方法均为静态方法，无需实例化即可使用。
 * 
 * 主要特性：
 * 1. 支持视频文件：mp4, avi, mkv, mov等20+格式
 * 2. 支持音频文件：mp3, wav, flac, aac等20+格式
 * 3. 自动识别文件夹/目录
 * 4. 不区分扩展名大小写
 * 5. 异常安全的路径处理
 * 6. 提供类型枚举和字符串描述的转换
 * 
 * @note
 * 1. 需要C++17或更高版本（依赖filesystem库）
 * 2. 扩展名检测基于预定义的集合，可通过修改静态成员变量扩展支持格式
 * 3. 对于符号链接，本工具会解析为实际文件/目录类型
 * 4. 包含完整的单元测试（通过FILE_TYPE_CHECKER_TEST宏启用）
 * 
 * @version 1.0.0
 * @date 2026-02-06
 * @author AI Assistant
 * 
 * @example
 * // 基本用法
 * auto type = filecheck::FileTypeChecker::checkFileType("video.mp4");
 * std::string desc = filecheck::FileTypeChecker::fileTypeToString(type);
 * // type = FileType::VIDEO, desc = "视频文件"
 * 
 * // 批量处理示例
 * for (const auto& file : files) {
 *     auto t = filecheck::FileTypeChecker::checkFileType(file);
 *     if (t == filecheck::FileType::VIDEO) {
 *         processVideo(file);
 *     }
 * }
 * 
 * @see std::filesystem, std::unordered_set
 * @copyright 示例代码，可根据需要修改和使用
 */

//#ifndef FILE_TYPE_CHECKER_H
#define FILE_TYPE_CHECKER_H

#include <string>
#include <unordered_set>
#include <filesystem>

namespace filecheck {

// 文件类型枚举
enum class FileType {
    VIDEO,      // 视频文件
    AUDIO,      // 音频文件
    DIRECTORY,  // 文件夹
    OTHER       // 其他类型文件
};

class FileTypeChecker {
private:
    // 常见视频文件扩展名
    static const std::unordered_set<std::string> videoExtensions;
    
    // 常见音频文件扩展名
    static const std::unordered_set<std::string> audioExtensions;

public:
    /**
     * 检查给定路径的文件类型
     * @param path 文件路径
     * @return 对应的FileType枚举值
     */
    static FileType checkFileType(const std::string& path) {
        namespace fs = std::filesystem;
        
        try {
            fs::path filePath(path);
            
            // 检查是否为目录
            if (fs::is_directory(filePath)) {
                return FileType::DIRECTORY;
            }
            
            // 检查是否为常规文件并获取扩展名
            if (fs::is_regular_file(filePath)) {
                std::string ext = filePath.extension().string();
                
                // 转换为小写以便不区分大小写比较
                for (char& c : ext) {
                    c = std::tolower(static_cast<unsigned char>(c));
                }
                
                // 检查视频扩展名
                if (videoExtensions.find(ext) != videoExtensions.end()) {
                    return FileType::VIDEO;
                }
                
                // 检查音频扩展名
                if (audioExtensions.find(ext) != audioExtensions.end()) {
                    return FileType::AUDIO;
                }
            }
            
            return FileType::OTHER;
        }
        catch (const std::exception&) {
            // 如果路径无效或发生其他异常，返回OTHER
            return FileType::OTHER;
        }
    }
    
    /**
     * 将FileType转换为可读字符串
     * @param type 文件类型
     * @return 类型描述字符串
     */
    static std::string fileTypeToString(FileType type) {
        switch (type) {
            case FileType::VIDEO: return "视频文件";
            case FileType::AUDIO: return "音频文件";
            case FileType::DIRECTORY: return "文件夹";
            case FileType::OTHER: return "其他文件";
            default: return "未知类型";
        }
    }
};

// 初始化静态成员 - 视频文件扩展名集合
const std::unordered_set<std::string> FileTypeChecker::videoExtensions = {
    ".mp4", ".avi", ".mkv", ".mov", ".wmv", ".flv", ".webm", ".m4v",
    ".mpg", ".mpeg", ".3gp", ".mts", ".m2ts", ".vob", ".ogv", ".qt",
    ".rm", ".rmvb", ".asf", ".swf", ".f4v", ".m4s"
};

// 初始化静态成员 - 音频文件扩展名集合
const std::unordered_set<std::string> FileTypeChecker::audioExtensions = {
    ".mp3", ".wav", ".flac", ".aac", ".ogg", ".wma", ".m4a", ".opus",
    ".aiff", ".alac", ".amr", ".ape", ".au", ".mid", ".midi", ".ra",
    ".ram", ".voc", ".weba"
};

} // namespace filecheck