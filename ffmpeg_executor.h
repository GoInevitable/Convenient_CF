/**
 * ffmpeg_executor.h
 * 用于执行FFmpeg命令并处理交互的头文件
 * 功能：执行FFmpeg命令，检测覆盖提示，判断执行状态，返回完整输出
 * 修复了Windows头文件中的byte冲突问题
 */

#ifndef FFMPEG_EXECUTOR_H
#define FFMPEG_EXECUTOR_H

#include <iostream>
#include <string>
#include <vector>
#include <atomic>
#include <thread>
#include <mutex>
#include <memory>
#include <algorithm>
#include <cctype>

#ifdef _WIN32
// 定义这些宏来避免Windows头文件中的一些冲突
#define NOMINMAX           // 避免min/max宏冲突
#define WIN32_LEAN_AND_MEAN // 排除不常用的Windows服务
#include <windows.h>
#undef ERROR               // 避免与日志宏冲突
#undef IGNORE              // 避免与其他代码冲突
#undef byte                // 避免byte冲突
#else
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h>
#endif

class FFmpegExecutor {
public:
    /**
     * 执行结果结构体
     */
    struct ExecuteResult {
        bool success;               // 命令是否执行成功
        int exitCode;               // 进程退出码
        std::string output;         // 完整输出
        std::string error;          // 错误信息（如果有）
        bool overwritePrompted;     // 是否检测到覆盖提示
        bool overwriteConfirmed;    // 是否自动确认覆盖
    };
    
    /**
     * 构造函数
     */
    FFmpegExecutor() : is_running_(false), auto_overwrite_(true) {}
    
    /**
     * 设置是否自动确认覆盖
     * @param auto_overwrite 是否自动确认覆盖
     */
    void setAutoOverwrite(bool auto_overwrite) {
        auto_overwrite_ = auto_overwrite;
    }
    
    /**
     * 执行FFmpeg命令
     * @param command FFmpeg命令字符串
     * @return 执行结果
     */
    ExecuteResult execute(const std::string& command) {
        ExecuteResult result;
        result.success = false;
        result.exitCode = -1;
        result.overwritePrompted = false;
        result.overwriteConfirmed = false;
        
        if (is_running_) {
            result.error = "FFmpeg命令已经在执行中";
            return result;
        }
        
        is_running_ = true;
        output_buffer_.clear();
        
        // 创建线程执行命令
        std::thread exec_thread(&FFmpegExecutor::executeInternal, this, command, std::ref(result));
        
        // 等待线程完成
        if (exec_thread.joinable()) {
            exec_thread.join();
        }
        
        is_running_ = false;
        return result;
    }
    
    /**
     * 获取是否正在运行
     * @return 运行状态
     */
    bool isRunning() const {
        return is_running_;
    }
    
    /**
     * 停止执行
     */
    void stop() {
        is_running_ = false;
        
#ifdef _WIN32
        if (process_info_.hProcess != nullptr) {
            TerminateProcess(process_info_.hProcess, 0);
        }
#else
        if (child_pid_ > 0) {
            kill(child_pid_, SIGTERM);
        }
#endif
    }
    
    /**
     * 获取最后一条错误信息
     * @return 错误信息
     */
    std::string getLastError() const {
        return last_error_;
    }
    
private:
    std::atomic<bool> is_running_;
    std::string output_buffer_;
    std::mutex output_mutex_;
    std::mutex error_mutex_;
    bool auto_overwrite_;
    std::string last_error_;
    
#ifdef _WIN32
    HANDLE stdout_read_handle_ = nullptr;
    HANDLE stdout_write_handle_ = nullptr;
    HANDLE stdin_read_handle_ = nullptr;
    HANDLE stdin_write_handle_ = nullptr;
    PROCESS_INFORMATION process_info_ = {0};
#else
    int stdout_pipe_[2] = {-1, -1};
    int stdin_pipe_[2] = {-1, -1};
    pid_t child_pid_ = -1;
#endif
    
    /**
     * 内部执行函数
     * @param command 命令字符串
     * @param result 执行结果引用
     */
    void executeInternal(const std::string& command, ExecuteResult& result) {
#ifdef _WIN32
        executeWindows(command, result);
#else
        executeUnix(command, result);
#endif
    }
    
    /**
     * 检测是否包含覆盖提示
     * @param line 输出行
     * @return 是否包含覆盖提示
     */
    bool detectOverwritePrompt(const std::string& line) {
        // 检查常见的覆盖提示模式
        std::string line_lower = toLower(line);
        
        // 模式1: File 'xxx' already exists. Overwrite? [y/N]
        if (line_lower.find("already exists") != std::string::npos &&
            line_lower.find("overwrite") != std::string::npos) {
            return true;
        }
        
        // 模式2: Overwrite? (y/n)
        if (line_lower.find("overwrite?") != std::string::npos ||
            line_lower.find("overwrite (y/n)") != std::string::npos) {
            return true;
        }
        
        // 模式3: 文件已存在，是否覆盖？(中文提示)
        if (line.find("已存在") != std::string::npos &&
            line.find("覆盖") != std::string::npos) {
            return true;
        }
        
        return false;
    }
    
    /**
     * 检测FFmpeg错误
     * @param line 输出行
     * @return 是否包含错误信息
     */
    bool detectError(const std::string& line) {
        std::string line_lower = toLower(line);
        
        // 常见FFmpeg错误关键词
        std::vector<std::string> error_keywords = {
            "error", "failed", "invalid", "unable", "cannot", 
            "unknown", "not found", "permission denied", "access denied"
        };
        
        for (const auto& keyword : error_keywords) {
            if (line_lower.find(keyword) != std::string::npos) {
                // 排除一些误判情况
                if (line_lower.find("non-monotonous") != std::string::npos) {
                    // 忽略"non-monotonous DTS"警告
                    continue;
                }
                return true;
            }
        }
        
        return false;
    }
    
    /**
     * 检测FFmpeg成功完成
     * @param line 输出行
     * @return 是否包含成功完成信息
     */
    bool detectSuccess(const std::string& line) {
        std::string line_lower = toLower(line);
        
        // 成功完成的关键词
        if (line_lower.find("video:") != std::string::npos &&
            line_lower.find("audio:") != std::string::npos &&
            line_lower.find("subtitle:") != std::string::npos) {
            return true;
        }
        
        // 编码完成提示
        if (line_lower.find("muxing overhead") != std::string::npos) {
            return true;
        }
        
        return false;
    }
    
    /**
     * 处理输出行
     * @param line 输出行
     * @param result 执行结果引用
     */
    void processLine(const std::string& line, ExecuteResult& result) {
        {
            std::lock_guard<std::mutex> lock(output_mutex_);
            output_buffer_ += line + "\n";
        }
        
        // 检测覆盖提示
        if (detectOverwritePrompt(line)) {
            result.overwritePrompted = true;
            if (auto_overwrite_) {
                result.overwriteConfirmed = true;
                sendInput("y\n");
            }
        }
        
        // 检测错误
        if (detectError(line)) {
            std::lock_guard<std::mutex> lock(error_mutex_);
            last_error_ = line;
            result.error = line;
        }
        
        // 检测成功完成
        if (detectSuccess(line)) {
            result.success = true;
        }
    }
    
    /**
     * 发送输入到进程
     * @param input 输入字符串
     */
    void sendInput(const std::string& input) {
#ifdef _WIN32
        if (stdin_write_handle_ != nullptr) {
            DWORD written = 0;
            WriteFile(stdin_write_handle_, input.c_str(), static_cast<DWORD>(input.size()), &written, nullptr);
        }
#else
        if (stdin_pipe_[1] != -1) {
            write(stdin_pipe_[1], input.c_str(), input.size());
        }
#endif
    }
    
    /**
     * 字符串转小写
     * @param str 输入字符串
     * @return 小写字符串
     */
    std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(),
                      [](unsigned char c) { return std::tolower(c); });
        return result;
    }
    
#ifdef _WIN32
    /**
     * Windows平台执行
     */
    void executeWindows(const std::string& command, ExecuteResult& result) {
        SECURITY_ATTRIBUTES saAttr = {sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE};
        
        // 创建标准输出管道
        if (!CreatePipe(&stdout_read_handle_, &stdout_write_handle_, &saAttr, 0)) {
            result.error = "创建输出管道失败";
            is_running_ = false;
            return;
        }
        
        // 创建标准输入管道
        if (!CreatePipe(&stdin_read_handle_, &stdin_write_handle_, &saAttr, 0)) {
            result.error = "创建输入管道失败";
            cleanupWindowsHandles();
            is_running_ = false;
            return;
        }
        
        // 设置进程启动信息
        STARTUPINFOA startup_info = {0};
        startup_info.cb = sizeof(STARTUPINFOA);
        startup_info.hStdError = stdout_write_handle_;
        startup_info.hStdOutput = stdout_write_handle_;
        startup_info.hStdInput = stdin_read_handle_;
        startup_info.dwFlags |= STARTF_USESTDHANDLES;
        
        // 创建命令行字符串
        std::string cmd_str = command;
        
        // 创建进程
        if (!CreateProcessA(nullptr, const_cast<LPSTR>(cmd_str.c_str()), nullptr, nullptr, 
                           TRUE, 0, nullptr, nullptr, &startup_info, &process_info_)) {
            DWORD error_code = GetLastError();
            result.error = "创建进程失败: 错误代码 " + std::to_string(error_code);
            cleanupWindowsHandles();
            is_running_ = false;
            return;
        }
        
        // 关闭不需要的句柄
        CloseHandle(stdout_write_handle_);
        CloseHandle(stdin_read_handle_);
        stdout_write_handle_ = nullptr;
        stdin_read_handle_ = nullptr;
        
        // 读取输出
        char buffer[4096];
        DWORD bytes_read = 0;
        std::string remaining_output;
        
        while (is_running_) {
            // 检查进程是否已退出
            DWORD exit_code = 0;
            if (GetExitCodeProcess(process_info_.hProcess, &exit_code)) {
                if (exit_code != STILL_ACTIVE) {
                    result.exitCode = static_cast<int>(exit_code);
                    break;
                }
            }
            
            // 读取输出
            if (PeekNamedPipe(stdout_read_handle_, nullptr, 0, nullptr, &bytes_read, nullptr) && bytes_read > 0) {
                if (ReadFile(stdout_read_handle_, buffer, sizeof(buffer) - 1, &bytes_read, nullptr) && bytes_read > 0) {
                    buffer[bytes_read] = '\0';
                    processOutput(buffer, remaining_output, result);
                }
            }
            
            // 避免CPU占用过高
            Sleep(10);
        }
        
        // 读取剩余输出
        while (ReadFile(stdout_read_handle_, buffer, sizeof(buffer) - 1, &bytes_read, nullptr) && bytes_read > 0) {
            buffer[bytes_read] = '\0';
            processOutput(buffer, remaining_output, result);
        }
        
        // 获取最终退出码
        DWORD exit_code = 0;
        if (GetExitCodeProcess(process_info_.hProcess, &exit_code)) {
            result.exitCode = static_cast<int>(exit_code);
            // 如果之前没有检测到成功，但退出码为0，也认为是成功
            if (result.exitCode == 0 && !result.success) {
                result.success = true;
            }
        }
        
        // 清理
        cleanupWindowsHandles();
        
        // 设置最终输出
        {
            std::lock_guard<std::mutex> lock(output_mutex_);
            result.output = output_buffer_;
        }
    }
    
    /**
     * 清理Windows句柄
     */
    void cleanupWindowsHandles() {
        if (stdout_read_handle_ != nullptr) {
            CloseHandle(stdout_read_handle_);
            stdout_read_handle_ = nullptr;
        }
        if (stdout_write_handle_ != nullptr) {
            CloseHandle(stdout_write_handle_);
            stdout_write_handle_ = nullptr;
        }
        if (stdin_read_handle_ != nullptr) {
            CloseHandle(stdin_read_handle_);
            stdin_read_handle_ = nullptr;
        }
        if (stdin_write_handle_ != nullptr) {
            CloseHandle(stdin_write_handle_);
            stdin_write_handle_ = nullptr;
        }
        if (process_info_.hProcess != nullptr) {
            CloseHandle(process_info_.hProcess);
            process_info_.hProcess = nullptr;
        }
        if (process_info_.hThread != nullptr) {
            CloseHandle(process_info_.hThread);
            process_info_.hThread = nullptr;
        }
    }
#else
    /**
     * Unix/Linux平台执行
     */
    void executeUnix(const std::string& command, ExecuteResult& result) {
        // 创建标准输出管道
        if (pipe(stdout_pipe_) == -1) {
            result.error = "创建输出管道失败";
            is_running_ = false;
            return;
        }
        
        // 创建标准输入管道
        if (pipe(stdin_pipe_) == -1) {
            result.error = "创建输入管道失败";
            close(stdout_pipe_[0]);
            close(stdout_pipe_[1]);
            is_running_ = false;
            return;
        }
        
        // 创建子进程
        child_pid_ = fork();
        
        if (child_pid_ == -1) {
            result.error = "创建子进程失败";
            cleanupUnixPipes();
            is_running_ = false;
            return;
        }
        
        if (child_pid_ == 0) {
            // 子进程
            close(stdout_pipe_[0]); // 关闭读取端
            close(stdin_pipe_[1]);  // 关闭写入端
            
            // 重定向标准输出和错误
            dup2(stdout_pipe_[1], STDOUT_FILENO);
            dup2(stdout_pipe_[1], STDERR_FILENO);
            
            // 重定向标准输入
            dup2(stdin_pipe_[0], STDIN_FILENO);
            
            // 关闭不需要的文件描述符
            close(stdout_pipe_[1]);
            close(stdin_pipe_[0]);
            
            // 执行命令
            execl("/bin/sh", "sh", "-c", command.c_str(), nullptr);
            
            // 如果exec失败
            exit(EXIT_FAILURE);
        } else {
            // 父进程
            close(stdout_pipe_[1]); // 关闭写入端
            close(stdin_pipe_[0]);  // 关闭读取端
            
            // 设置为非阻塞读取
            int flags = fcntl(stdout_pipe_[0], F_GETFL, 0);
            fcntl(stdout_pipe_[0], F_SETFL, flags | O_NONBLOCK);
            
            // 读取输出
            char buffer[4096];
            ssize_t bytes_read = 0;
            std::string remaining_output;
            
            while (is_running_) {
                // 检查子进程是否已退出
                int status = 0;
                pid_t wait_result = waitpid(child_pid_, &status, WNOHANG);
                
                if (wait_result != 0) {
                    // 子进程已退出
                    if (WIFEXITED(status)) {
                        result.exitCode = WEXITSTATUS(status);
                        // 如果退出码为0，认为是成功
                        if (result.exitCode == 0 && !result.success) {
                            result.success = true;
                        }
                    } else {
                        result.exitCode = -1;
                    }
                    break;
                }
                
                // 读取输出
                bytes_read = read(stdout_pipe_[0], buffer, sizeof(buffer) - 1);
                
                if (bytes_read > 0) {
                    buffer[bytes_read] = '\0';
                    processOutput(buffer, remaining_output, result);
                } else if (bytes_read < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
                    // 读取错误
                    break;
                }
                
                // 短暂休眠，避免CPU占用过高
                usleep(10000); // 10ms
            }
            
            // 读取剩余输出
            while ((bytes_read = read(stdout_pipe_[0], buffer, sizeof(buffer) - 1)) > 0) {
                buffer[bytes_read] = '\0';
                processOutput(buffer, remaining_output, result);
            }
            
            // 清理
            cleanupUnixPipes();
            
            // 设置最终输出
            {
                std::lock_guard<std::mutex> lock(output_mutex_);
                result.output = output_buffer_;
            }
        }
    }
    
    /**
     * 清理Unix管道
     */
    void cleanupUnixPipes() {
        if (stdout_pipe_[0] != -1) {
            close(stdout_pipe_[0]);
            stdout_pipe_[0] = -1;
        }
        if (stdout_pipe_[1] != -1) {
            close(stdout_pipe_[1]);
            stdout_pipe_[1] = -1;
        }
        if (stdin_pipe_[0] != -1) {
            close(stdin_pipe_[0]);
            stdin_pipe_[0] = -1;
        }
        if (stdin_pipe_[1] != -1) {
            close(stdin_pipe_[1]);
            stdin_pipe_[1] = -1;
        }
    }
#endif
    
    /**
     * 处理原始输出，分割为行
     * @param output 原始输出
     * @param remaining 剩余未处理的字符串
     * @param result 执行结果引用
     */
    void processOutput(const char* output, std::string& remaining, ExecuteResult& result) {
        std::string full_output = remaining + output;
        std::string line;
        size_t start_pos = 0;
        size_t newline_pos;
        
        while ((newline_pos = full_output.find('\n', start_pos)) != std::string::npos) {
            line = full_output.substr(start_pos, newline_pos - start_pos);
            
            // 移除回车符
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            
            processLine(line, result);
            start_pos = newline_pos + 1;
        }
        
        // 保存剩余部分
        remaining = full_output.substr(start_pos);
    }
};

#endif // FFMPEG_EXECUTOR_H
