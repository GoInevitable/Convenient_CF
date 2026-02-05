/**
 * @file settings_manager.h
 * @brief 设置管理器 - 用于配置的读取、保存和管理
 * @details 提供简单的INI格式配置文件的读取和保存功能，支持硬编码的默认配置
 * @author AI Assistant
 */

#ifndef SETTINGS_MANAGER_H
#define SETTINGS_MANAGER_H

#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <vector>
using namespace std;
class SettingsManager
{
private:
    std::string configFile;                             // 配置文件路径
    std::map<std::string, std::string> settings;        // 当前设置
    std::map<std::string, std::string> defaultSettings; // 硬编码的默认设置

    // 辅助函数：去除字符串两端的空白字符
    static std::string trim(const std::string &str)
    {
        size_t first = str.find_first_not_of(" \t\n\r");
        if (first == std::string::npos)
        {
            return "";
        }
        size_t last = str.find_last_not_of(" \t\n\r");
        return str.substr(first, (last - first + 1));
    }

    // 辅助函数：将字符串转换为小写
    static std::string toLower(const std::string &str)
    {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(),
                       [](unsigned char c)
                       { return std::tolower(c); });
        return result;
    }

    // 初始化默认配置
    void initDefaultSettings()
    {
        // 这里可以硬编码所有的默认配置
        /*
        defaultSettings["app.name"] = "MyApplication";
        defaultSettings["app.version"] = "1.0.0";
        defaultSettings["window.width"] = "800";
        defaultSettings["window.height"] = "600";
        defaultSettings["window.fullscreen"] = "false";
        defaultSettings["graphics.quality"] = "high";
        defaultSettings["audio.volume"] = "80";
        defaultSettings["audio.mute"] = "false";
        defaultSettings["network.timeout"] = "30";
        defaultSettings["language"] = "zh_CN";
        defaultSettings["auto_save"] = "true";
        defaultSettings["save_interval"] = "300";
        defaultSettings["max_history"] = "50";
        defaultSettings["theme"] = "dark";
        defaultSettings["font_size"] = "14";
        */
        defaultSettings["full_output"] = "false";
        
    }

public:
    /**
     * @brief 构造函数
     * @param filename 配置文件的路径
     */
    explicit SettingsManager(const std::string &filename = "config.ini")
        : configFile(filename)
    {
        initDefaultSettings();
        load();
    }

    /**
     * @brief 加载配置文件
     * @return 是否成功加载
     */
    bool load()
    {
        // 先清空当前设置
        settings.clear();

        // 用默认设置填充
        settings = defaultSettings;

        std::ifstream file(configFile);
        if (!file.is_open())
        {
            // 如果文件不存在，使用默认设置并保存
            std::cout << "配置文件不存在，使用默认设置并创建新文件: " << configFile << std::endl;
            return save();
        }

        std::string line;
        int lineNum = 0;

        while (std::getline(file, line))
        {
            lineNum++;
            line = trim(line);

            // 跳过空行和注释
            if (line.empty() || line[0] == '#' || line[0] == ';')
            {
                continue;
            }

            // 解析键值对
            size_t equalPos = line.find('=');
            if (equalPos != std::string::npos)
            {
                std::string key = trim(line.substr(0, equalPos));
                std::string value = trim(line.substr(equalPos + 1));

                if (!key.empty())
                {
                    settings[key] = value;
                }
            }
        }

        file.close();
        return true;
    }

    /**
     * @brief 保存配置到文件
     * @return 是否成功保存
     */
    bool save()
    {
        std::ofstream file(configFile);
        if (!file.is_open())
        {
            std::cerr << "无法打开配置文件进行写入: " << configFile << std::endl;
            return false;
        }

        // 写入文件头
        file << "# 应用程序配置文件" << std::endl;
        file << "# 自动生成，请勿手动编辑" << std::endl;
        file << std::endl;

        // 写入所有设置
        for (const auto &pair : settings)
        {
            file << pair.first << " = " << pair.second << std::endl;
        }

        file.close();
        return true;
    }

    /**
     * @brief 获取字符串类型的设置值
     * @param key 设置键
     * @param defaultValue 默认值（如果键不存在）
     * @return 设置值
     */
    std::string getString(const std::string &key, const std::string &defaultValue = "")
    {
        auto it = settings.find(key);
        if (it != settings.end())
        {
            return it->second;
        }
        return defaultValue;
    }

    /**
     * @brief 获取整数类型的设置值
     * @param key 设置键
     * @param defaultValue 默认值（如果键不存在或转换失败）
     * @return 设置值
     */
    int getInt(const std::string &key, int defaultValue = 0)
    {
        auto it = settings.find(key);
        if (it != settings.end())
        {
            try
            {
                return std::stoi(it->second);
            }
            catch (const std::exception &)
            {
                return defaultValue;
            }
        }
        return defaultValue;
    }

    /**
     * @brief 获取浮点数类型的设置值
     * @param key 设置键
     * @param defaultValue 默认值（如果键不存在或转换失败）
     * @return 设置值
     */
    double getDouble(const std::string &key, double defaultValue = 0.0)
    {
        auto it = settings.find(key);
        if (it != settings.end())
        {
            try
            {
                return std::stod(it->second);
            }
            catch (const std::exception &)
            {
                return defaultValue;
            }
        }
        return defaultValue;
    }

    /**
     * @brief 获取布尔类型的设置值
     * @param key 设置键
     * @param defaultValue 默认值（如果键不存在）
     * @return 设置值
     */
    bool getBool(const std::string &key, bool defaultValue = false)
    {
        auto it = settings.find(key);
        if (it != settings.end())
        {
            std::string value = toLower(trim(it->second));
            return (value == "true" || value == "1" || value == "yes" || value == "on");
        }
        return defaultValue;
    }

    /**
     * @brief 设置字符串类型的值
     * @param key 设置键
     * @param value 设置值
     */
    void setString(const std::string &key, const std::string &value)
    {
        settings[key] = value;
    }

    /**
     * @brief 设置整数类型的值
     * @param key 设置键
     * @param value 设置值
     */
    void setInt(const std::string &key, int value)
    {
        settings[key] = std::to_string(value);
    }

    /**
     * @brief 设置浮点数类型的值
     * @param key 设置键
     * @param value 设置值
     */
    void setDouble(const std::string &key, double value)
    {
        settings[key] = std::to_string(value);
    }

    /**
     * @brief 设置布尔类型的值
     * @param key 设置键
     * @param value 设置值
     */
    void setBool(const std::string &key, bool value)
    {
        settings[key] = value ? "true" : "false";
    }

    /**
     * @brief 检查设置是否存在
     * @param key 设置键
     * @return 是否存在
     */
    bool hasKey(const std::string &key) const
    {
        return settings.find(key) != settings.end();
    }

    /**
     * @brief 删除设置
     * @param key 设置键
     * @return 是否成功删除
     */
    bool removeKey(const std::string &key)
    {
        return settings.erase(key) > 0;
    }

    /**
     * @brief 获取所有设置的键
     * @return 键的向量
     */
    vector<string> getAllKeys() const
    {
        std::vector<std::string> keys;
        for (const auto &pair : settings)
        {
            keys.push_back(pair.first);
        }
        return keys;
    }

    /**
     * @brief 恢复到默认设置
     */
    void restoreDefaults()
    {
        settings = defaultSettings;
    }

    /**
     * @brief 获取配置文件的路径
     * @return 配置文件路径
     */
    std::string getConfigFilePath() const
    {
        return configFile;
    }

    /**
     * @brief 设置配置文件的路径
     * @param filename 新的配置文件路径
     */
    void setConfigFilePath(const std::string &filename)
    {
        configFile = filename;
    }

    /**
     * @brief 获取默认设置的映射
     * @return 默认设置的常量引用
     */
    const std::map<std::string, std::string> &getDefaultSettings() const
    {
        return defaultSettings;
    }

    /**
     * @brief 获取当前设置的映射
     * @return 当前设置的常量引用
     */
    const std::map<std::string, std::string> &getAllSettings() const
    {
        return settings;
    }
};

// 测试代码
#ifdef SETTINGS_MANAGER_TEST
#include <vector>

int main()
{
    std::cout << "=== SettingsManager 测试 ===" << std::endl;

    // 创建设置管理器
    SettingsManager settings("test_config.ini");

    // 测试获取默认值
    std::cout << "1. 获取默认设置:" << std::endl;
    std::cout << "   应用名称: " << settings.getString("app.name") << std::endl;
    std::cout << "   窗口大小: " << settings.getInt("window.width")
              << "x" << settings.getInt("window.height") << std::endl;
    std::cout << "   全屏模式: " << (settings.getBool("window.fullscreen") ? "是" : "否") << std::endl;
    std::cout << "   音量: " << settings.getInt("audio.volume") << "%" << std::endl;

    // 测试修改设置
    std::cout << "\n2. 修改设置:" << std::endl;
    settings.setInt("window.width", 1024);
    settings.setInt("window.height", 768);
    settings.setBool("window.fullscreen", true);
    settings.setInt("audio.volume", 90);
    settings.setString("theme", "light");

    std::cout << "   窗口大小改为: " << settings.getInt("window.width")
              << "x" << settings.getInt("window.height") << std::endl;
    std::cout << "   全屏模式改为: " << (settings.getBool("window.fullscreen") ? "是" : "否") << std::endl;
    std::cout << "   音量改为: " << settings.getInt("audio.volume") << "%" << std::endl;
    std::cout << "   主题改为: " << settings.getString("theme") << std::endl;

    // 测试保存
    std::cout << "\n3. 保存设置到文件..." << std::endl;
    if (settings.save())
    {
        std::cout << "   保存成功到: " << settings.getConfigFilePath() << std::endl;
    }
    else
    {
        std::cout << "   保存失败!" << std::endl;
    }

    // 测试获取所有键
    std::cout << "\n4. 所有设置键:" << std::endl;
    auto keys = settings.getAllKeys();
    for (size_t i = 0; i < keys.size(); ++i)
    {
        std::cout << "   " << (i + 1) << ". " << keys[i] << std::endl;
    }

    // 测试恢复到默认设置
    std::cout << "\n5. 恢复到默认设置..." << std::endl;
    settings.restoreDefaults();
    std::cout << "   窗口大小恢复为: " << settings.getInt("window.width")
              << "x" << settings.getInt("window.height") << std::endl;

    // 测试删除键
    std::cout << "\n6. 删除主题设置..." << std::endl;
    settings.removeKey("theme");
    std::cout << "   主题是否存在: " << (settings.hasKey("theme") ? "是" : "否") << std::endl;

    std::cout << "\n=== 测试完成 ===" << std::endl;

    return 0;
}
#endif // SETTINGS_MANAGER_TEST

#endif // SETTINGS_MANAGER_H
