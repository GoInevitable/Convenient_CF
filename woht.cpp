std::map<std::string, std::map<std::string, std::string>> Merging_BiliBili_videos()
{
    //弃用！！！！！
    cout<<"Warning: This function is deprecated and may not work correctly with all BiliBili .m4s files. Use with caution."<<endl;
    using NestedMap = std::map<std::string, std::map<std::string, std::string>>;
    NestedMap result;

    std::string video_path = settings.getString("BiliBili.video_path");

    // 未设置则询问用户是否手动输入路径
    if (video_path == "HAVE_NOT_SETTING")
    {
        cout << "BiliBili video path is not set. Do you want to set it? [y/N]" << endl;
        char choice = 'N';
        cin >> choice;
        if (choice != 'Y' && choice != 'y')
        {
            cout << "Skipping BiliBili scan." << endl;
            return result;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        video_path = single_file_chooser("Please enter the BiliBili root directory path:");
    }

    namespace fs = std::filesystem;
    fs::path root(video_path);

    if (!fs::exists(root) || !fs::is_directory(root))
    {
        cout << "Error: path does not exist or is not a directory: " << video_path << endl;
        return result;
    }
    settings.setString("BiliBili.video_path",video_path);
    settings.save();
    // 遍历根目录下的子目录
    for (const auto &entry : fs::directory_iterator(root))
    {
        if (!entry.is_directory())
            continue;

        std::string folder_name = entry.path().filename().string();

        // 只处理全数字命名的文件夹
        if (!std::all_of(folder_name.begin(), folder_name.end(), [](unsigned char c) { return std::isdigit(c); }))
            continue;

        std::vector<fs::path> matches;

        for (const auto &f : fs::directory_iterator(entry.path()))
        {
            if (!f.is_regular_file())
                continue;

            std::string ext = f.path().extension().string();
            std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return std::tolower(c); });
            if (ext != ".m4s")
                continue;

            std::string fname = f.path().filename().string();
            if (fname.rfind(folder_name, 0) == 0)
            {
                matches.push_back(f.path());
            }
        }

        if (matches.size() < 2)
        {
            cout << "Warning: folder '" << folder_name << "' has fewer than 2 matching .m4s files; skipping." << endl;
            continue;
        }

        std::sort(matches.begin(), matches.end(), [](const fs::path &a, const fs::path &b) {
            return a.filename().string() < b.filename().string();
        });

        std::map<std::string, std::string> inner;
        inner["folder_path"] = entry.path().string();
        inner["file1"] = matches[0].string();
        inner["file2"] = matches[1].string();

        result[folder_name] = std::move(inner);
    }

    return result;
}

// 将扫描结果导出为 JSON 文件
bool ExportBiliScanResultToJson(const std::map<std::string, std::map<std::string, std::string>> &scanResult, const std::filesystem::path &outFile)
{
    std::ofstream ofs(outFile);
    if (!ofs)
        return false;

    ofs << "{" << std::endl;
    bool firstFolder = true;
    for (const auto &kv : scanResult)
    {
        if (!firstFolder) ofs << ",\n";
        firstFolder = false;
        const auto &folder = kv.first;
        const auto &inner = kv.second;
        ofs << "  \"" << folder << "\": {\n";
        ofs << "    \"folder_path\": \"" << inner.at("folder_path") << "\"," << "\n";
        ofs << "    \"file1\": \"" << inner.at("file1") << "\"," << "\n";
        ofs << "    \"file2\": \"" << inner.at("file2") << "\"\n";
        ofs << "  }";
    }
    ofs << "\n}" << std::endl;
    return true;
}

// 使用 FFmpeg 的 concat demuxer 合并两个 .m4s 文件到输出路径
bool MergeTwoM4s(const std::filesystem::path &file1, const std::filesystem::path &file2, const std::filesystem::path &output)
{
    namespace fs = std::filesystem;
    // Helper: 检查文件开头是否包含 MP4 box（ftyp/moov），以判断是否为可独立播放的 MP4
    auto hasMp4Box = [](const fs::path &p) {
        std::ifstream ifs(p, std::ios::binary);
        if (!ifs)
            return false;
        const size_t N = 8192;
        std::string buf;
        buf.resize(N);
        ifs.read(&buf[0], N);
        size_t readn = (size_t)ifs.gcount();
        buf.resize(readn);
        if (buf.find("ftyp") != std::string::npos) return true;
        if (buf.find("moov") != std::string::npos) return true;
        if (buf.find("mdat") != std::string::npos) return true;
        return false;
    };

    // 很多 B 站的 .m4s 是 fMP4 片段（仅包含媒体片段，没有 init segment），
    // 这类文件不能直接用 ffmpeg concat 或 remux，需要 init.mp4 或其他初始化段。
    if (!hasMp4Box(file1) || !hasMp4Box(file2))
    {
        cout << "Skipping merge: one or both .m4s segments appear to be fragmented (no ftyp/moov)." << endl;
        cout << "Hint: fragmented fMP4 segments require an init segment (init.mp4) or use a tool like MP4Box to assemble them." << endl;
        return false;
    }
    fs::path listFile = output.parent_path() / (output.stem().string() + "_concat_list.txt");
    std::ofstream ofs(listFile);
    if (!ofs)
        return false;
    ofs << "file '" << file1.string() << "'\n";
    ofs << "file '" << file2.string() << "'\n";
    ofs.close();

    // 首先尝试直接使用 concat demuxer（如果 .m4s 可直接被 demuxer 识别）
    std::string cmd = buildCmd(settings.getString("ffmpeg.path"), "-f", "concat", "-safe", "0", "-i", listFile.string(), "-c", "copy", output.string());

    FFmpegExecutor executor;
    executor.setAutoOverwrite(true);
    FFmpegExecutor::ExecuteResult res = executor.execute(cmd);

    if (res.success)
    {
        try { fs::remove(listFile); } catch (...) {}
        return true;
    }

    // 如果直接 concat 失败，打印调试输出并尝试备用流程：先将每个 .m4s 重封装为临时 mp4，再 concat
    cout << "Direct concat failed, attempting remux+concat fallback. ffmpeg output:\n" << res.output << endl;
    cout << "ffmpeg error: " << res.error << endl;

    fs::path tmp1 = output.parent_path() / (output.stem().string() + "_part1_tmp.mp4");
    fs::path tmp2 = output.parent_path() / (output.stem().string() + "_part2_tmp.mp4");

    // 重封装命令（不重新编码）
    std::string remux1 = buildCmd(settings.getString("ffmpeg.path"), "-i", file1.string(), "-c", "copy", tmp1.string());
    std::string remux2 = buildCmd(settings.getString("ffmpeg.path"), "-i", file2.string(), "-c", "copy", tmp2.string());

    FFmpegExecutor::ExecuteResult r1 = executor.execute(remux1);
    if (!r1.success)
    {
        cout << "Remux part1 failed: " << r1.error << endl;
        try { fs::remove(listFile); } catch (...) {}
        return false;
    }
    FFmpegExecutor::ExecuteResult r2 = executor.execute(remux2);
    if (!r2.success)
    {
        cout << "Remux part2 failed: " << r2.error << endl;
        try { fs::remove(listFile); } catch (...) {}
        try { fs::remove(tmp1); } catch (...) {}
        return false;
    }

    // 使用 concat demuxer 合并临时 mp4
    std::ofstream ofs2(listFile);
    if (!ofs2)
    {
        try { fs::remove(tmp1); } catch (...) {}
        try { fs::remove(tmp2); } catch (...) {}
        return false;
    }
    ofs2 << "file '" << tmp1.string() << "'\n";
    ofs2 << "file '" << tmp2.string() << "'\n";
    ofs2.close();

    std::string concatTmpCmd = buildCmd(settings.getString("ffmpeg.path"), "-f", "concat", "-safe", "0", "-i", listFile.string(), "-c", "copy", output.string());
    FFmpegExecutor::ExecuteResult r3 = executor.execute(concatTmpCmd);

    bool finalOk = r3.success;

    // 清理临时文件
    try { fs::remove(listFile); } catch (...) {}
    try { fs::remove(tmp1); } catch (...) {}
    try { fs::remove(tmp2); } catch (...) {}

    if (!finalOk)
    {
        cout << "Fallback concat failed: " << r3.error << endl;
    }

    return finalOk;
}

// 扫描 -> 导出 JSON -> 针对每个文件夹合并两个 m4s
void Process_Merging_BiliBili_videos()
{
    namespace fs = std::filesystem;
    auto scan = Merging_BiliBili_videos();
    if (scan.empty())
    {
        cout << "No valid BiliBili folders found to process." << endl;
        return;
    }

    // 导出 JSON 到根目录或第一个 folder 的上级
    fs::path root = fs::path(settings.getString("BiliBili.video_path"));
    if (root.empty() || root == "HAVE_NOT_SETTING")
    {
        // 取第一个文件夹路径作为基准
        root = fs::path(scan.begin()->second.at("folder_path")).parent_path();
    }
    fs::path jsonOut = root / "bili_scan_result.json";
    if (ExportBiliScanResultToJson(scan, jsonOut))
    {
        cout << "Exported scan result to " << jsonOut.string() << endl;
    }
    else
    {
        cout << "Failed to export scan result to " << jsonOut.string() << endl;
    }

    // 对每个文件夹执行合并，输出到 folder/merged_<folder>.mp4
    for (const auto &kv : scan)
    {
        const std::string &folderName = kv.first;
        const auto &inner = kv.second;
        fs::path folderPath = inner.at("folder_path");
        fs::path file1 = inner.at("file1");
        fs::path file2 = inner.at("file2");
        fs::path outFile = folderPath / (std::string("merged_") + folderName + ".mp4");

        cout << "Merging for folder " << folderName << " -> " << outFile.string() << " ..." << endl;
        bool ok = MergeTwoM4s(file1, file2, outFile);
        if (ok)
            cout << "  Success: " << outFile.string() << endl;
        else
            cout << "  Failed to merge files in " << folderName << endl;
    }
}