#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <windows.h>
#include <limits>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <map>
#include <cctype>
#include <fstream>
#include <iomanip>

#include "Path_checker.h"
#include "file_chooser.h"
#include "SettingsManager.h"
#include "ffmpeg_executor.h"
using namespace std;

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

/**
 * @brief 删除指定路径的文件
 * @param filePath 文件路径（UTF-8编码）
 * @return 成功返回true，失败返回false
 */
bool DeleteFileSafe(const std::string &filePath)
{
    // 将UTF-8字符串转换为UTF-16
    int wideLen = MultiByteToWideChar(CP_UTF8, 0, filePath.c_str(), -1, nullptr, 0);
    if (wideLen <= 0)
        return false;

    wchar_t *widePath = new wchar_t[wideLen];
    MultiByteToWideChar(CP_UTF8, 0, filePath.c_str(), -1, widePath, wideLen);

    // 使用DeleteFileW删除文件
    BOOL result = DeleteFileW(widePath);
    delete[] widePath;

    if (!result)
    {
        // 获取错误信息（可选，用于调试）
        DWORD error = GetLastError();
        if (error != ERROR_FILE_NOT_FOUND)
        { // 文件不存在不算错误
            std::cerr << "删除文件失败 (错误码: " << error << "): " << filePath << std::endl;
        }
        return false;
    }

    return true;
}

bool FileExists(const std::string &filePath)
{
    // 将UTF-8/ANSI字符串转换为Windows API所需的UTF-16
    int wideLen = MultiByteToWideChar(CP_UTF8, 0, filePath.c_str(), -1, nullptr, 0);
    if (wideLen <= 0)
        return false;

    wchar_t *widePath = new wchar_t[wideLen];
    MultiByteToWideChar(CP_UTF8, 0, filePath.c_str(), -1, widePath, wideLen);

    // 使用GetFileAttributesW（最稳定的Windows API方案）
    DWORD attributes = GetFileAttributesW(widePath);
    delete[] widePath;

    // 检查文件存在且不是目录
    return (attributes != INVALID_FILE_ATTRIBUTES &&
            !(attributes & FILE_ATTRIBUTE_DIRECTORY));
}

SettingsManager settings;

template <typename... Args>
string buildCmd(const Args &...args)
{
    ostringstream oss;
    ((oss << args << " "), ...);
    string result = oss.str();
    if (!result.empty())
    {
        result.pop_back(); // 移除最后一个多余的空格
    }
    return result;
}
// 前置声明：确保在本头文件中可以在定义之前调用该函数
void wait_return_to_previous_menu();

void about_this()
{
    cout << "Convenient_CF - ffmpeg 工具 v0.0.1，作者：Jane Smith" << endl;
    cout << "本工具提供若干 ffmpeg 相关功能，例如：格式转换、提取音频、合并视频等。" << endl;
    // 等待用户按回车以便阅读完信息后返回上一级菜单
    wait_return_to_previous_menu();
}
/**
 * @brief 等待用户按回车以返回上一级菜单的辅助函数。
 * 
 * 目的：任何展示了信息并与用户交互的“菜单/子功能”在返回调用者前
 * 应调用此函数，提示用户按回车并等待，以便用户能看到结果并准备好
 * 回到上一级菜单。
 */
void wait_return_to_previous_menu()
{
    // 在提示前清理任何错误状态，但不要在无缓冲数据时阻塞地吞掉输入，
    // 否则会导致需要按两次回车的问题。
    cout << "\n按回车返回上一级菜单..." << flush;
    cin.clear();
    // 仅当有可用输入（例如之前留下的换行）时，才消耗到行尾，这个检测是非阻塞的。
    std::streambuf *buf = cin.rdbuf();
    if (buf->in_avail() > 0)
    {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    string _tmp;
    // 等待用户按回车（此处会阻塞直到用户输入一行）
    getline(cin, _tmp);
}
/*
 * @brief 视频格式转换主函数
 * @return int 0表示成功，非0表示失败
 *
 *
 */
int Converting_video_format()
{
    cout << "单文件转换(1) 还是多文件批量转换(2)？" << endl;
    int choice;
    cin >> choice;
    if (choice == 2)
    {
    }
    else
    {
        string input_file_path = single_file_chooser("请输入要转换的视频文件路径：");
        string output_file_path = single_file_chooser("请输入输出视频文件路径（含文件名）：");
        if (filecheck::FileTypeChecker::checkFileType(input_file_path) != filecheck::FileType::VIDEO)
        {
            cout << "错误：输入文件不是有效的视频文件。" << endl;
            return 1;
        }
        //  获取ffmpeg路径，默认值为“ffmpeg”
        // 检测输出文件是否存在
        if (FileExists(output_file_path))
        {
            cout << "文件 '" << output_file_path << "' 已存在，是否覆盖？[y/N]" << endl;
            char choice1 = 'N';
            cin >> choice1;
            if (choice1 != 'Y' && choice1 != 'y')
            {
                cout << "用户已取消操作。" << endl;
                return 0;
            }
            if (DeleteFileSafe(output_file_path))
            {
                cout << "已删除已存在的文件：" << output_file_path << endl;
            }
            else
            {
                cout << "删除已存在文件失败：" << output_file_path << endl;
                return 1;
            }
        }

        string cmd = buildCmd(settings.getString("ffmpeg.path"), "-i", input_file_path, output_file_path);
        if (settings.getBool("isExecutionConfirmed"))
        {
            cout << "将要执行命令：" << cmd << endl
                 << "是否确认执行？[Y/n]" << endl;
            char choice2;
            cin >> choice2;
            if (choice2 != 'Y' && choice2 != 'y')
            {
                cout << "用户已取消操作。" << endl;
                return 0;
            }
        }
        // 执行命令
        FFmpegExecutor executor;
        executor.setAutoOverwrite(true);
        FFmpegExecutor::ExecuteResult result = executor.execute(cmd);
        if (settings.getBool("full_output")) {
            cout << "ffmpeg 命令完整输出：" << endl;
            dividing_line(100);
            cout << result.output << endl;
            dividing_line(100);
        }

        if (result.success) {
            cout << "视频格式转换完成。" << endl;
        } else {
            cout << "视频格式转换失败。" << endl;
            if (!result.error.empty()) {
                cout << "错误信息：" << result.error << endl;
            }
        }
    }
    return 0;
}

