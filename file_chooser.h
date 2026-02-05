#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

/**
 * @brief 从控制台读取单个文件路径的函数
 * 
 * @param prompt 用户提示信息，默认"Please enter the file path:"
 * @param max_attempts 最大尝试次数，默认3次
 * @return string 用户输入的文件路径
 * 
 * 功能说明:
 * 1. 提示用户输入单个文件路径
 * 2. 验证输入是否为空
 * 3. 提供最大尝试次数限制
 * 4. 处理EOF（文件结束）情况
 */
string single_file_chooser(const string& prompt = "Please enter the file path:", int max_attempts = 3) {
    string file_path;
    int attempts = 0;
    
    // 显示主提示信息
    cout << prompt << endl;
    
    while (attempts < max_attempts) {
        // 显示输入提示符
        cout << "> ";
        getline(cin, file_path);
        
        // 检查是否输入流结束
        if (cin.eof()) {
            cout << "\nInput terminated (EOF). Returning empty string." << endl;
            return "";
        }
        
        // 移除首尾空白字符
        size_t start = file_path.find_first_not_of(" \t");
        if (start == string::npos) {
            // 空输入
            attempts++;
            if (attempts < max_attempts) {
                cout << "Input cannot be empty. Please try again. (" 
                      << max_attempts - attempts << " attempts remaining)" << endl;
            } else {
                cout << "Maximum attempts reached. Process terminated." << endl;
            }
            continue;
        }
        
        // 提取非空内容
        size_t end = file_path.find_last_not_of(" \t");
        string trimmed_input = file_path.substr(start, end - start + 1);
        
        if (!trimmed_input.empty()) {
            file_path = trimmed_input;
            cout << "File path accepted: " << file_path << endl;
            return file_path;
        }
    }
    
    return "";
}

/**
 * @brief 从控制台读取多个文件路径的函数
 * 
 * @param prompt 用户提示信息，默认"Please enter file paths:"
 * @return vector<string> 包含输入文件路径的向量
 * 
 * 功能说明:
 * 1. 允许多个文件路径输入
 * 2. 空行结束输入
 * 3. 自动移除每个输入的空白字符
 * 4. 处理EOF（文件结束）情况
 * 5. 返回输入的文件路径向量
 */
vector<string> multi_file_chooser(const string& prompt = "Please enter file paths:") {
    vector<string> file_paths;
    string input;
    
    // 显示主提示信息
    cout << prompt << endl;
    cout << "Enter file paths (one per line). Press Enter on an empty line to finish:" << endl;
    
    int count = 1;
    
    while (true) {
        // 显示带编号的输入提示
        cout << "File " << count << ": ";
        getline(cin, input);
        
        // 检查是否输入流结束
        if (cin.eof()) {
            cout << "\nInput terminated (EOF)." << endl;
            break;
        }
        
        // 移除首尾空白字符
        size_t start = input.find_first_not_of(" \t");
        if (start == string::npos) {
            // 空行或只包含空白字符，结束输入
            if (file_paths.empty()) {
                cout << "No files entered. Process terminated." << endl;
            } else {
                cout << "Finished entering " << file_paths.size() << " file(s)." << endl;
            }
            break;
        }
        
        // 提取非空内容
        size_t end = input.find_last_not_of(" \t");
        string trimmed_input = input.substr(start, end - start + 1);
        
        if (!trimmed_input.empty()) {
            file_paths.push_back(trimmed_input);
            count++;
        }
    }
    
    return file_paths;
}

/**
 * @brief 从控制台读取文件路径的通用函数
 * 
 * @param allow_multiple_inputs 是否允许多个文件输入，默认false
 * @param prompt 用户提示信息，默认"Please enter the file path:"
 * @return vector<string> 包含输入文件路径的向量
 * 
 * 功能说明:
 * 1. 当 allow_multiple_inputs 为 false 时，调用 single_file_chooser
 * 2. 当 allow_multiple_inputs 为 true 时，调用 multi_file_chooser
 * 3. 提供向后兼容性
 */
vector<string> file_chooser(bool allow_multiple_inputs = false, 
                           const string& prompt = "Please enter the file path:") {
    vector<string> result;
    
    if (allow_multiple_inputs) {
        // 调用多文件选择器
        return multi_file_chooser(prompt);
    } else {
        // 调用单文件选择器
        string file_path = single_file_chooser(prompt);
        if (!file_path.empty()) {
            result.push_back(file_path);
        }
    }
    
    return result;
}
/*
/**
 * @brief 测试单文件选择器函数
 */
void test_single_file_chooser() {
    cout << "\n=== Testing Single File Chooser ===" << endl;
    
    // 测试用例1: 默认参数
    cout << "\nTest 1: Default parameters" << endl;
    cout << "Expected: 用户输入单个文件路径" << endl;
    // 注意: 实际测试时需要用户输入
    // string file1 = single_file_chooser();
    
    // 测试用例2: 自定义提示
    cout << "\nTest 2: Custom prompt" << endl;
    cout << "Expected: 显示自定义提示信息" << endl;
    // string file2 = single_file_chooser("Enter configuration file:");
    
    // 测试用例3: 自定义最大尝试次数
    cout << "\nTest 3: Custom max attempts" << endl;
    cout << "Expected: 最多允许5次尝试" << endl;
    // string file3 = single_file_chooser("Enter data file:", 5);
}

/**
 * @brief 测试多文件选择器函数
 */
void test_multi_file_chooser() {
    cout << "\n=== Testing Multi File Chooser ===" << endl;
    
    // 测试用例1: 默认参数
    cout << "\nTest 1: Default parameters" << endl;
    cout << "Expected: 用户可以输入多个文件路径" << endl;
    // vector<string> files1 = multi_file_chooser();
    
    // 测试用例2: 自定义提示
    cout << "\nTest 2: Custom prompt" << endl;
    cout << "Expected: 显示自定义提示信息" << endl;
    // vector<string> files2 = multi_file_chooser("Select files to process:");
}

/**
 * @brief 测试向后兼容的 file_chooser 函数
 */
void test_file_chooser_backward_compatible() {
    cout << "\n=== Testing Backward Compatible File Chooser ===" << endl;
    
    // 测试用例1: 单文件模式
    cout << "\nTest 1: Single file mode (default)" << endl;
    // vector<string> result1 = file_chooser();  // 调用 single_file_chooser
    
    // 测试用例2: 多文件模式
    cout << "\nTest 2: Multiple file mode" << endl;
    // vector<string> result2 = file_chooser(true);  // 调用 multi_file_chooser
    
    // 测试用例3: 自定义提示
    cout << "\nTest 3: Custom prompt in single mode" << endl;
    // vector<string> result3 = file_chooser(false, "Enter input file:");
    
    cout << "\nTest 4: Custom prompt in multiple mode" << endl;
    // vector<string> result4 = file_chooser(true, "Select files:");
}

/**
 * @brief 演示如何使用重构后的函数
 */
void demonstrate_usage() {
    cout << "\n=== Demonstration of Refactored Functions ===" << endl;
    
    cout << "\n1. Using single_file_chooser():" << endl;
    cout << "   string file_path = single_file_chooser();" << endl;
    cout << "   // 或者指定提示信息和最大尝试次数" << endl;
    cout << "   string config_file = single_file_chooser(\"Enter config file:\", 5);" << endl;
    
    cout << "\n2. Using multi_file_chooser():" << endl;
    cout << "   vector<string> files = multi_file_chooser();" << endl;
    cout << "   // 或者指定提示信息" << endl;
    cout << "   vector<string> data_files = multi_file_chooser(\"Enter data files:\");" << endl;
    
    cout << "\n3. Using backward compatible file_chooser():" << endl;
    cout << "   // 单文件模式" << endl;
    cout << "   vector<string> single_file = file_chooser(false, \"Enter file:\");" << endl;
    cout << "   // 多文件模式" << endl;
    cout << "   vector<string> multiple_files = file_chooser(true, \"Select files:\");" << endl;
}

/**
 * @brief 模拟使用场景示例
 */
void example_scenarios() {
    cout << "\n=== Example Usage Scenarios ===" << endl;
    
    // 场景1: 配置文件选择
    cout << "\nScenario 1: Configuration File Selection" << endl;
    cout << "------------------------------------------" << endl;
    cout << "string config_path = single_file_chooser(\"Please enter configuration file path:\");" << endl;
    cout << "if (!config_path.empty()) {" << endl;
    cout << "    // 处理配置文件" << endl;
    cout << "    cout << \"Processing configuration file: \" << config_path << endl;" << endl;
    cout << "}" << endl;
    
    // 场景2: 批量文件处理
    cout << "\nScenario 2: Batch File Processing" << endl;
    cout << "-----------------------------------" << endl;
    cout << "vector<string> input_files = multi_file_chooser(\"Select files to process:\");" << endl;
    cout << "if (!input_files.empty()) {" << endl;
    cout << "    cout << \"Processing \" << input_files.size() << \" files:\" << endl;" << endl;
    cout << "    for (size_t i = 0; i < input_files.size(); i++) {" << endl;
    cout << "        cout << \"  \" << (i + 1) << \". \" << input_files[i] << endl;" << endl;
    cout << "    }" << endl;
    cout << "}" << endl;
}

/**
 * @brief 主函数
 */
/*
int main() {
    cout << "File Chooser Functions - Refactored Version" << endl;
    cout << "============================================" << endl;
    
    // 运行测试
    test_single_file_chooser();
    test_multi_file_chooser();
    test_file_chooser_backward_compatible();
    
    // 演示用法
    demonstrate_usage();
    
    // 示例场景
    example_scenarios();
    
    cout << "\n=== Function Overview ===" << endl;
    cout << "\nAvailable functions:" << endl;
    cout << "1. single_file_chooser() - 选择单个文件" << endl;
    cout << "2. multi_file_chooser()  - 选择多个文件" << endl;
    cout << "3. file_chooser()        - 向后兼容的函数" << endl;
    
    cout << "\nNote: To actually use these functions, call them in your code." << endl;
    cout << "The demonstration above shows how to call each function." << endl;
    
    return 0;
}*/