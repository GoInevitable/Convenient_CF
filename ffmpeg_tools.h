#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include "Path_checker.h"
#include "single_file_chooser.h"

using namespace std;
void about_this()
{
    cout << "Convenient_CF ffmpeg tools v0.0.1 by Jane Smith" << endl;
    cout << "This tool provides various ffmpeg functionalities such as format conversion, audio extraction, and video merging." << endl;
}
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
        string input_file_path =single_file_chooser("Please enter the video file path to convert:");
        string output_file_path =single_file_chooser("Please enter the output video file path:");
        if(filecheck::FileTypeChecker::checkFileType(input_file_path)!=filecheck::FileType::VIDEO)
        {
            cout<<"Error: The input file is not a valid video file."<<endl;
            return 1;
        }
        if(filecheck::FileTypeChecker::checkFileType(output_file_path)!=filecheck::FileType::DIRECTORY)
        {
            cout<<"Error: The output file path is not a valid output file path."<<endl;
            return 1;
        }
        

    }
    return 0;
}