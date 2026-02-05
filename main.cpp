#include <iostream>
#include <cstdio>
#include <string>
#include "ffmpeg_tools.h"

#include "Path_checker.h"
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
int check_ffmpeg_version(bool full_output = false)
{
    cout << "Checking ffmpeg version..." << endl;
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
        cerr << "Command execution failed with exit status: " << exitStatus << endl;
        return 1;
    }
    if (full_output)
    {
        cout << "Full output of ffmpeg version command:" << endl;
        dividing_line(100);
        cout << result << endl;
        dividing_line(100);
    }
    else
    {
        for (int i = 0;; i++)
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
    cout << "1.ffmpeg version" << endl
         << "2.convert video format" << endl
         << "3.extract audio from video" << endl
         << "4.merge videos" << endl
         << "5.return to main menu" << endl;
    cout << "Please enter your choice (1-5): ";
    int choice;
    cin >> choice;
    dividing_line();
    switch (choice)
    {
    case 1:
        cout << "ffmpeg version 4.4.1" << endl;
        if (check_ffmpeg_version(true) != 0)
        {
            cout << "Error: ffmpeg is not installed or not accessible." << endl;
            return 1;
        }
        break;
    case 2:
        cout << "Converting video format..." << endl;
        Converting_video_format();
        break;
    case 3:
        cout << "Extracting audio from video..." << endl;
        // Add audio extraction functionality here
        break;
    case 4:
        cout << "Merging videos..." << endl;
        // Add video merging functionality here
        break;
    case 5:
        cout << "Returning to main menu..." << endl;
        break;
    default:
        cout << "Invalid choice. Please try again." << endl;
    }
    return 0;
}
int mingw_tools()
{
    // Placeholder for MinGW tools functionality
    return 0;
}
int other_tools()
{
    // Placeholder for other tools functionality
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
        if (ffmpeg_tools() != 0)
        {
            cout << "ffmpeg tools encountered an error." << endl;
            return 1;
        }
        cout << "You selected ffmpeg tools." << endl;
        // Add ffmpeg tools functionality here
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
        about_this();
        //cout << "Convenient_CF is a toolset developed by Jane Smith to simplify various tasks." << endl;
        break;
    case 5:
        cout << "Exiting the program. Goodbye!" << endl;
        break;
    default:
        cout << "Invalid choice. Please run the program again and select a valid option." << endl;
    }
    return 0;
}