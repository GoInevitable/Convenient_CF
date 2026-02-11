#include <iostream>
#include "SettingsManager.h"
#include <vector>
#include <string>
#include <filesystem>
#include "Path_checker.h"

using namespace std;
SettingsManager settings;

std::vector<std::string> videoFiles;
int GetViedoPath() {
    // 获取工作目录
    std::string videoPath = settings.getString("work_path.video_path");
    
    // 检查目录是否存在
    if (!std::filesystem::exists(videoPath)) {
        std::cout << "错误：视频目录不存在 - " << videoPath << std::endl;
        return -1;
    }
    
    if (!std::filesystem::is_directory(videoPath)) {
        std::cout << "错误：路径不是目录 - " << videoPath << std::endl;
        return -1;
    }
    
    
    
    try {
        // 遍历目录中的所有文件
        for (const auto& entry : std::filesystem::directory_iterator(videoPath)) {
            if (entry.is_regular_file()) {
                std::string filePath = entry.path().string();
                
                // 使用Path_checker检测文件类型
                filecheck::FileType fileType = filecheck::FileTypeChecker::checkFileType(filePath);
                
                // 如果是视频文件，添加到数组
                if (fileType == filecheck::FileType::VIDEO) {
                    videoFiles.push_back(filePath);
                    std::cout << "找到视频文件: " << entry.path().filename() << std::endl;
                }
            }
        }
        
        // 输出结果统计
        std::cout << "共找到 " << videoFiles.size() << " 个视频文件" << std::endl;
        
        // 这里可以添加后续的视频格式转换逻辑
        // for (const auto& videoFile : videoFiles) {
        //     // 转换视频格式的代码
        // }
        
    } catch (const std::filesystem::filesystem_error& ex) {
        std::cout << "文件系统错误: " << ex.what() << std::endl;
        return -1;
    } catch (const std::exception& ex) {
        std::cout << "错误: " << ex.what() << std::endl;
        return -1;
    }
    
    return 0;
}

int Converting_video_format()
{
    
    return 0;
}
void dividing_line(int length = 0)
{
    if (length <= 0)
    {
        cout << "------------------------------------------------------------------";
    }
    else
    {
        for (int i = 0; i < length; i++)
        {
            cout << "-";
        }
    }
    cout << endl;
}
int check_ffmpeg_version(bool full_output = false)
{
    cout << "正在检查 ffmpeg 版本..." << endl;
    const char *command = "ffmpeg -version";
    FILE *pipe = _popen(command, "r");
    if (!pipe)
    {
        std::cerr << "Failed to execute command." << std::endl;
        return 1;
    }
    char buffer[128];
    std::string result;
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
    {
        result += buffer;
    }
    int exitStatus = _pclose(pipe);
    if (exitStatus != 0)
    {
        cerr << "命令执行失败，退出状态: " << exitStatus << endl;
        return 1;
    }
    if (full_output)
    {
        cout << "ffmpeg 版本完整输出：" << endl;
        dividing_line(100);
        cout << result << endl;
        dividing_line(100);
    }
    else
    {
        // 只打印第一行版本信息，直到换行
        for (size_t i = 0; i < result.size(); ++i)
        {
            cout << result[i];
            if (result[i] == '\n')
                break;
        }
    }
    return 0;
}

int ffmpeg_tools()
{
    if (check_ffmpeg_version() != 0)
    {
        cout << "Error: ffmpeg is not installed or not accessible." << endl;
        return 1;
    }
    if (settings.getString("work_path.video_path") == "HAVE_NOT_SETTING")
    {
        cout << "视频工作路径未设置，请先在配置文件中设置 work_path.video_path 的值。" << endl;
        string work_path;
        cout << "请输入视频工作路径：";
        //cin >> work_path;
        cin.ignore();
        getline(cin, work_path);
        
        cout<<work_path<<endl;
        settings.setString("work_path.video_path", work_path);
        settings.save();
    }
    cout << "视频工作路径已设置为: " << settings.getString("work_path.video_path") << endl;
    if(GetViedoPath() != 0)
    {
        cout << "获取视频路径失败，请检查配置文件中的 work_path.video_path 设置。" << endl;
        return 1;
    }
    
    cout << "1. 查看 ffmpeg 版本" << endl
         << "2. 转换视频格式" << endl
         << "3. 从视频中提取音频" << endl
         << "4. 合并视频" << endl
         << "5. 返回主菜单" << endl;
    cout << "请输入你的选择（1-5）：";
    int choice;
    cin >> choice;
    dividing_line();
    switch (choice)
    {
    case 1:
        cout << "查看 ffmpeg 版本" << endl;
        if (check_ffmpeg_version(true) != 0)
        {
            cout << "错误：找不到 ffmpeg 或不可访问。" << endl;
            return 1;
        }
        break;
    case 2:
        cout << "正在转换视频格式..." << endl;
        if (Converting_video_format() != 0)
        {
            cout << "视频格式转换过程中发生错误。" << endl;
            return 1;
        }
        // Converting_video_format();
        break;
    case 3:
        cout << "正在从视频中提取音频...（功能待实现）" << endl;
        // 在此处添加音频提取功能
        break;
    case 4:
        cout << "正在合并视频...（功能待实现）" << endl;
        // 在此处添加视频合并功能
        break;
    case 5:
        cout << "扫描并处理 BiliBili 视频...（功能待实现）" << endl;
        // Process_Merging_BiliBili_videos();
        break;
    case 6:
        cout << "返回主菜单..." << endl;
        break;
    default:
        cout << "Invalid choice. Please try again." << endl;
    }
    // 在子菜单操作完成后，等待用户按回车以返回上一级菜单
    // wait_return_to_previous_menu();
    return 0;
}
