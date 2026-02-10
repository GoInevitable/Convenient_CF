#include <iostream>
#include <cstdio>
#include <string>
#include "ffmpeg_tool.h"
//#include "SettingsManager.h"
// 配置管理已在头文件中定义为 `SettingsManager settings;`
using namespace std;

int mingw_tools()
{
    // Placeholder for MinGW tools functionality
    // 等待用户查看信息并返回上一级菜单
    // wait_return_to_previous_menu();
    return 0;
}
int other_tools()
{
    cout << "Other tools functionality is under development." << endl;
    // 等待用户查看信息并返回上一级菜单
    // wait_return_to_previous_menu();
    return 0;
}
int main()
{
    cout << "Convenient_CF v0.0.1 by Jane Smith" << endl
         << "1.ffmpeg tools" << endl
         << "2.MinGW tools" << endl
         << "3.Other tools" << endl
         << "4.about" << endl
         << "5.exit" << endl;
    cout << "Please enter your choice (1-5): ";
    int choice;
    cin >> choice;
    dividing_line();
    switch (choice)
    {
    case 1:
        cout << "You selected ffmpeg tools." << endl;
        if (ffmpeg_tools() != 0)
        {
            cout << "ffmpeg tools encountered an error." << endl;
            return 1;
        }
        break;
    case 2:
        cout << "You selected MinGW tools." << endl;
        // Add MinGW tools functionality here
        break;
    case 3:
        cout << "You selected Other tools." << endl;
        // Add other tools functionality here
        break;
    case 4:
        //about_this();
        // cout << "Convenient_CF is a toolset developed by Jane Smith to simplify various tasks." << endl;
        break;
    case 5:
        cout << "Exiting the program. Goodbye!" << endl;
        break;
    default:
        cout << "Invalid choice. Please run the program again and select a valid option." << endl;
    }
    return 0;
}