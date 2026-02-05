SettingsManager 技术文档
====================

文件信息
----

* **文件名称**: `settings_manager.h`

* **功能描述**: 设置管理器 - 用于配置的读取、保存和管理

* **文件格式**: C++ 头文件

* **最后更新**: 2026年2月6日

概述
--

`SettingsManager`是一个轻量级的C++配置管理类，用于处理应用程序的配置数据。它支持INI格式的配置文件，提供类型安全的配置值访问，并包含默认值回退机制。该类设计简单、自包含，适合中小型项目使用。
主要特性

----

1. **INI格式支持**: 使用简单的键值对格式（`key = value`）

2. **硬编码默认值**: 内置默认配置，确保程序始终有有效配置

3. **类型安全访问**: 提供多种数据类型的获取和设置方法

4. **自动文件创建**: 配置文件不存在时自动创建并填充默认值

5. **配置恢复**: 支持一键恢复到默认设置

6. **简单API**: 直观的接口设计，易于使用

类设计
---

### 成员变量

* `configFile`: 配置文件路径

* `settings`: 当前配置的键值对映射

* `defaultSettings`: 硬编码的默认配置映射

### 核心方法

#### 构造与初始化

* `SettingsManager(const std::string& filename)`: 构造函数，接受配置文件路径

* `initDefaultSettings()`: 初始化硬编码的默认配置

#### 文件操作

* `bool load()`: 从文件加载配置，文件不存在时使用默认值并创建文件

* `bool save()`: 保存当前配置到文件

#### 配置访问

* `getString()`, `getInt()`, `getDouble()`, `getBool()`: 获取不同类型的配置值

* `setString()`, `setInt()`, `setDouble()`, `setBool()`: 设置不同类型的配置值

* `hasKey()`: 检查配置项是否存在

* `removeKey()`: 删除配置项

* `getAllKeys()`: 获取所有配置键的列表

#### 辅助功能

* `restoreDefaults()`: 恢复到默认配置

* `getConfigFilePath()`: 获取配置文件路径

* `setConfigFilePath()`: 设置配置文件路径

* `getDefaultSettings()`: 获取默认设置的映射

* `getAllSettings()`: 获取当前设置的映射

### 辅助函数

* `trim()`: 去除字符串两端空白字符

* `toLower()`: 将字符串转换为小写

配置文件格式
------

配置文件采用INI格式：
    # 注释以#或;开头
    key1 = value1
    key2 = value2
    section.key = value
默认配置项

-----

类中硬编码的默认配置包括：

* 应用程序信息：名称、版本

* 窗口设置：宽度、高度、全屏模式

* 图形设置：质量等级

* 音频设置：音量、静音

* 网络设置：超时时间

* 界面设置：语言、主题、字体大小

* 功能设置：自动保存、保存间隔、历史记录数量

使用示例
----

### 基本使用

```c++
#include "settings_manager.h"

int main() {
    // 创建管理器（默认使用config.ini）
    SettingsManager settings;

    // 读取配置
    std::string appName = settings.getString("app.name");
    int width = settings.getInt("window.width");
    bool fullscreen = settings.getBool("window.fullscreen");

    // 修改配置
    settings.setInt("window.width", 1024);
    settings.setBool("window.fullscreen", true);

    // 保存配置
    settings.save();

    return 0;
}
```

### 自定义配置文件路径

```c++
SettingsManager settings("myapp_config.ini");
```

### 获取所有配置

    auto keys = settings.getAllKeys();
    for (const auto& key : keys) {
        std::cout << key << " = " << settings.getString(key) << std::endl;
    }

错误处理
----

1. **文件读取失败**: 如果配置文件无法打开，将使用默认配置并尝试创建新文件

2. **类型转换失败**: 数值转换失败时返回指定的默认值

3. **键不存在**: 返回调用时指定的默认值

线程安全性
-----

⚠️ **注意**: 当前实现不是线程安全的。如果在多线程环境中使用，需要在外部添加同步机制。
限制与注意事项

-------

1. **不支持配置节**: 标准的INI格式支持`[section]`语法，但本实现将所有键视为扁平结构

2. **简单的字符串处理**: 配置值不支持包含换行符或等号的复杂字符串

3. **内存存储**: 所有配置加载到内存中，不适合非常大的配置文件

4. **编码**: 假设配置文件使用系统默认编码

编译选项
----

定义`SETTINGS_MANAGER_TEST`宏可以启用内置测试代码：
    #define SETTINGS_MANAGER_TEST
    #include "settings_manager.h"

---
