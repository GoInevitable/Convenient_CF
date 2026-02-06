#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <windows.h>

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
bool DeleteFileSafe(const std::string& filePath) {
    // 将UTF-8字符串转换为UTF-16
    int wideLen = MultiByteToWideChar(CP_UTF8, 0, filePath.c_str(), -1, nullptr, 0);
    if (wideLen <= 0) return false;
    
    wchar_t* widePath = new wchar_t[wideLen];
    MultiByteToWideChar(CP_UTF8, 0, filePath.c_str(), -1, widePath, wideLen);
    
    // 使用DeleteFileW删除文件
    BOOL result = DeleteFileW(widePath);
    delete[] widePath;
    
    if (!result) {
        // 获取错误信息（可选，用于调试）
        DWORD error = GetLastError();
        if (error != ERROR_FILE_NOT_FOUND) {  // 文件不存在不算错误
            std::cerr << "删除文件失败 (错误码: " << error << "): " << filePath << std::endl;
        }
        return false;
    }
    
    return true;
}

bool FileExists(const std::string& filePath) {
    // 将UTF-8/ANSI字符串转换为Windows API所需的UTF-16
    int wideLen = MultiByteToWideChar(CP_UTF8, 0, filePath.c_str(), -1, nullptr, 0);
    if (wideLen <= 0) return false;
    
    wchar_t* widePath = new wchar_t[wideLen];
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
void about_this()
{
    cout << "Convenient_CF ffmpeg tools v0.0.1 by Jane Smith" << endl;
    cout << "This tool provides various ffmpeg functionalities such as format conversion, audio extraction, and video merging." << endl;
}
/*
 *@brief 视频格式转换主函数
 *@return int 0表示成功，非0表示失败
 *
 *
 */
int Converting_video_format()
{
    cout << "single file conversion(1) or multiple file conversion(2)?" << endl;
    int choice;
    cin >> choice;
    if (choice == 2)
    {
    }
    else
    {
        string input_file_path = single_file_chooser("Please enter the video file path to convert:");
        string output_file_path = single_file_chooser("Please enter the output video file path:");
        if (filecheck::FileTypeChecker::checkFileType(input_file_path) != filecheck::FileType::VIDEO)
        {
            cout << "Error: The input file is not a valid video file." << endl;
            return 1;
        }
        // if (filecheck::FileTypeChecker::checkFileType(output_file_path) != filecheck::FileType::VIDEO)
        //{
        //     cout << "Error: The output file path is not a valid video file path." << endl;
        //    return 1;
        // }
        //  获取ffmpeg路径，默认值为“ffmpeg”

        //检测输出文件是否存在
        if(FileExists(output_file_path)){
            cout<<"File '" << output_file_path << "' already exists. Overwrite? [y/N]" << endl;
            char choice1 = 'N';
            cin >> choice1;
            if (choice1 != 'Y' && choice1 != 'y')
            {
                cout << "Operation cancelled by user." << endl;
                return 0;
            }
            if(DeleteFileSafe(output_file_path)){
                cout << "Deleted existing file: " << output_file_path << endl;
            }else{
                cout << "Failed to delete existing file: " << output_file_path << endl;
                return 1;
            }
        }

        string cmd = buildCmd(settings.getString("ffmpeg.path"), "-i", input_file_path, output_file_path);
        if (settings.getBool("isExecutionConfirmed"))
        {
            cout << "Executing command:" << cmd << endl
                 << "Y or n" << endl;
            char choice2;
            cin >> choice2;
            if (choice2 != 'Y' && choice2 != 'y')
            {
                cout << "Operation cancelled by user." << endl;
                return 0;
            }
            

        }
        // 执行命令
        FFmpegExecutor executor;
        executor.setAutoOverwrite(true);
        FFmpegExecutor::ExecuteResult result = executor.execute(cmd);
        if (settings.getBool("full_output"))
        {
            cout << "Full output of ffmpeg command:" << endl;
            dividing_line(100);
            cout << result.output << endl;
            dividing_line(100);
        }

        if (result.success)
        {
            cout << "Video format conversion completed successfully." << endl;
        }
        else
        {
            cout << "Video format conversion failed." << endl;
            if (!result.error.empty())
            {
                cout << "Error: " << result.error << endl;
            }
        }
    }
    return 0;
}
