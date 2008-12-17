#pragma once

#include <windows.h>
#include <TinyXml/TinyXml.h>
#include <string>

/// �����������Ե�ö������
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

/// ���ù�����
class ConfigMgr
{
public:
    static bool Init(const char *fileName);
    
    /// �����Ҫ��ȡ�����ò��������Ƿ����
    bool IsConfigStrValid(const std::string& item) const;
    
    //// ��ȡ�ַ�����ʽ������ֵ
    /// @param[in] item for example item("ui.welcome.width")
    std::string GetStringValue(const std::string& item) const;

    /// ���ýڵ��ֵ
    void SetStringValue(const std::string& item, const char *value);

    /// ��������
    void SaveConfig(const char *fileName);

    std::wstring GetWstringValue(const std::string& item) const;

    COLORREF GetColorRef(const std::string& item) const;

    /// ��ȡ��������ֵ
    int GetIntegerValue(std::string item) const;
    
    /// ��ȡ��������ֵ������
    ConfigType::EnumCofigType GetConfigType(const std::string& item);
private:
    /// �ж�һ���ڵ�������Ƿ��������ַ�����ͬ
    bool IsItemMatch( const TiXmlNode * itemNode,const std::string& subItem ) const;
    
    static std::string GetTopItem(const std::string &item);
    
    static std::string GetSubItem(const std::string &item);
    
    /// �Ƿ���������
    /// @retval true  û��������(����"width")
    /// @retval false ��������(����"ui.width")
    static bool IsBasicItem(const std::string &item);
    
    /// ��ȡһ�����öε�ָ��
    TiXmlNode* GetLevelOneItem(std::string sectionName) const;
    
    TiXmlNode* GetItemNode(const std::string& item) const;

    /// ��ȡ���õ�xml doc
    TiXmlDocument* GetConfigXml() const;
    
    /// XML�ļ�������,��������˳�ʱ�Զ��ͷ���Դ
    class XmlContaniner
    {
    public:
        XmlContaniner();
        ~XmlContaniner();
        TiXmlDocument xml;
    };
    
    /// XmlContaniner�ľ�̬ʵ��,�����Ƴ�ǰ�����������ᱻ����
    static XmlContaniner config_;
};