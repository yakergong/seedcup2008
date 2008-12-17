#pragma once

#include <windows.h>
#include <TinyXml/TinyXml.h>
#include <string>

/// 包含配置属性的枚举类型
class ConfigType
{
public:
    enum EnumCofigType
    {
        integer,
        string,
        wstring,
        unknown,
    };
};

/// 配置管理器
class ConfigMgr
{
public:
    static bool Init(const char *fileName);
    
    /// 检查所要获取的配置参数参数是否存在
    bool IsConfigStrValid(const std::string& item) const;
    
    //// 获取字符串形式的配置值
    /// @param[in] item for example item("ui.welcome.width")
    std::string GetStringValue(const std::string& item) const;

    /// 设置节点的值
    void SetStringValue(const std::string& item, const char *value);

    /// 保存设置
    void SaveConfig(const char *fileName);

    std::wstring GetWstringValue(const std::string& item) const;

    COLORREF GetColorRef(const std::string& item) const;

    /// 获取整数配置值
    int GetIntegerValue(std::string item) const;
    
    /// 获取所需配置值的类型
    ConfigType::EnumCofigType GetConfigType(const std::string& item);
private:
    /// 判断一个节点的名称是否与所给字符串相同
    bool IsItemMatch( const TiXmlNode * itemNode,const std::string& subItem ) const;
    
    static std::string GetTopItem(const std::string &item);
    
    static std::string GetSubItem(const std::string &item);
    
    /// 是否有子属性
    /// @retval true  没有子属性(例如"width")
    /// @retval false 有子属性(例如"ui.width")
    static bool IsBasicItem(const std::string &item);
    
    /// 获取一个配置段的指针
    TiXmlNode* GetLevelOneItem(std::string sectionName) const;
    
    TiXmlNode* GetItemNode(const std::string& item) const;

    /// 获取配置的xml doc
    TiXmlDocument* GetConfigXml() const;
    
    /// XML文件的容器,负责程序退出时自动释放资源
    class XmlContaniner
    {
    public:
        XmlContaniner();
        ~XmlContaniner();
        TiXmlDocument xml;
    };
    
    /// XmlContaniner的静态实例,程序推出前其析构函数会被调用
    static XmlContaniner config_;
};