#ifndef FILE_TYPE_CHECKER_H
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

// 测试用例
#ifdef FILE_TYPE_CHECKER_TEST
#include <iostream>
#include <vector>

int main() {
    // 测试数据
    std::vector<std::string> testPaths = {
        "test_video.mp4",           // 视频文件
        "music/album/song.mp3",     // 音频文件
        "documents/reports/",       // 文件夹
        "image.jpg",                // 其他文件
        "C:/Videos/movie.avi",      // 视频文件（带路径）
        "audio.flac",               // 音频文件
        "D:/Projects/",             // 文件夹
        "presentation.pptx",        // 其他文件
        "video.MKV",                // 视频文件（大写扩展名）
        "audio.WAV",                // 音频文件（大写扩展名）
        "nonexistent.txt",          // 不存在的文件
        ""                          // 空路径
    };
    
    std::cout << "文件类型检测测试：" << std::endl;
    std::cout << "=========================================" << std::endl;
    
    for (const auto& path : testPaths) {
        filecheck::FileType type = filecheck::FileTypeChecker::checkFileType(path);
        std::string typeStr = filecheck::FileTypeChecker::fileTypeToString(type);
        
        std::cout << "路径: \"" << (path.empty() ? "(空)" : path) << "\""
                  << " -> 类型: " << typeStr << std::endl;
    }
    
    // 额外测试：枚举所有类型
    std::cout << "\n文件类型枚举测试：" << std::endl;
    std::cout << "=========================================" << std::endl;
    std::cout << "VIDEO: " << filecheck::FileTypeChecker::fileTypeToString(filecheck::FileType::VIDEO) << std::endl;
    std::cout << "AUDIO: " << filecheck::FileTypeChecker::fileTypeToString(filecheck::FileType::AUDIO) << std::endl;
    std::cout << "DIRECTORY: " << filecheck::FileTypeChecker::fileTypeToString(filecheck::FileType::DIRECTORY) << std::endl;
    std::cout << "OTHER: " << filecheck::FileTypeChecker::fileTypeToString(filecheck::FileType::OTHER) << std::endl;
    
    return 0;
}
#endif // FILE_TYPE_CHECKER_TEST

#endif // FILE_TYPE_CHECKER_H
