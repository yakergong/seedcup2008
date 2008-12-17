#include <cstring>
#include <sstream>

#include "ConfigMgr.h"
#include "CharsetConverter.h"
#include "NumericConverter.h"

using std::string;
using std::wstring;
using std::stringstream;

ConfigMgr::XmlContaniner ConfigMgr::config_;

ConfigMgr::XmlContaniner::XmlContaniner()
{
}

ConfigMgr::XmlContaniner::~XmlContaniner()
{
    xml.Clear();
    xml.ClearError();
}

bool ConfigMgr::Init( const char *fileName )
{
    config_.xml = TiXmlDocument( fileName );
    bool loadOkay = config_.xml.LoadFile();
    if( false == loadOkay )
        return false;
    else
        return true;
}

TiXmlDocument* ConfigMgr::GetConfigXml() const
{
    return &(config_.xml);
}

std::string ConfigMgr::GetStringValue( const std::string& item ) const
{  
    const TiXmlNode* itemNode = GetItemNode(item);
    
    if( NULL == itemNode)
        return string("");
          
    const TiXmlAttribute *attrib = itemNode->ToElement()->FirstAttribute();
    attrib = attrib->Next()->Next();
    return string(attrib->Value());
}

TiXmlNode* ConfigMgr::GetLevelOneItem( std::string sectionName ) const
{
    TiXmlDocument *xml = GetConfigXml();
    for(TiXmlNode *iter = xml->FirstChild(); iter != NULL; iter=iter->NextSibling())
    {
        if(iter->Type() != TiXmlNode::ELEMENT)
            continue;
            
        if( strcmp(iter->Value(),"item") == 0)
        {
            const TiXmlAttribute *attrib = iter->ToElement()->FirstAttribute();
            if( strcmp("name", attrib->Name()) == 0 )
            {
                if( strcmp(sectionName.c_str(), attrib->Value()) == 0 )
                    return iter;
            }
        }
    }   
    return NULL;
}

int ConfigMgr::GetIntegerValue( std::string item ) const
{
    string value = GetStringValue(item);
    stringstream converter;
    converter << value;
    int result;
    converter >> result;
    return result;
}

TiXmlNode* ConfigMgr::GetItemNode(const std::string& item ) const
{       
    if( IsBasicItem(item) )
    {
        return NULL;
    }
    
    string sectionName = GetTopItem(item);    // 段名
    TiXmlNode* section = GetLevelOneItem(sectionName);
    
    string subItem = GetSubItem(item);    // 构造子串

    TiXmlNode *itemNode = section;
    while( itemNode != NULL)
    {
        TiXmlNode *tempNode = itemNode;
        for(itemNode = tempNode->FirstChild(); itemNode != NULL; itemNode=itemNode->NextSibling())
        {
            if( IsItemMatch(itemNode, GetTopItem(subItem)) )
                break;
        }

        if(itemNode == NULL)
            return NULL;
            
        if( IsBasicItem(subItem) )
            break;
        else
            subItem = GetSubItem(subItem);
    } 
    
    return itemNode;
}

string ConfigMgr::GetTopItem( const std::string& item )
{
    if( IsBasicItem(item) )
        return item;
    else
    {
        string topItem;
        size_t iter = item.find('.');
        for(size_t i = 0; i < iter; ++i)
        {
            topItem.push_back(item[i]);
        }
        
        return topItem;
    }
}

std::string ConfigMgr::GetSubItem( const std::string &item )
{
    string subItem;
    size_t iter = item.find('.');
    for(size_t i = iter + 1; i < item.length(); ++i)
    {
        subItem.push_back(item[i]);
    }

    return subItem;
}

bool ConfigMgr::IsBasicItem( const std::string &item )
{
    size_t iter = item.find('.');
    if( iter == string::npos )
        return true;
    else
        return false;
}

bool ConfigMgr::IsItemMatch( const TiXmlNode * itemNode,const string &subItem ) const
{
    if( strcmp("item", itemNode->Value()) == 0 )
    {
        const TiXmlAttribute *attrib = itemNode->ToElement()->FirstAttribute();
        if( strcmp("name",attrib->Name()) == 0 )
        {
            if( strcmp(subItem.c_str(), attrib->Value()) == 0)
                return true;
            else
                return false;
        }
    }
    
    return false;
}

ConfigType::EnumCofigType ConfigMgr::GetConfigType( const std::string& item )
{
    const TiXmlNode* itemNode = GetItemNode(item);
    const TiXmlAttribute *attrib = itemNode->ToElement()->FirstAttribute();
    attrib = attrib->Next();
    if( strcmp(attrib->Value(), "integer") == 0 )
        return ConfigType::integer;
    if( strcmp(attrib->Value(), "string") == 0 )
        return ConfigType::string;
    if( strcmp(attrib->Value(), "wstring") == 0 )
        return ConfigType::wstring;
    else
        return ConfigType::unknown;
}

std::wstring ConfigMgr::GetWstringValue( const std::string& item ) const
{
    const TiXmlNode* itemNode = GetItemNode(item);

    if( NULL == itemNode)
        return wstring(TEXT(""));

    const TiXmlAttribute *attrib = itemNode->ToElement()->FirstAttribute();
    attrib = attrib->Next()->Next();
    return CharsetConverter::utf8_to_ws(attrib->Value());
}

COLORREF ConfigMgr::GetColorRef( const std::string& item ) const
{
    string colorName[3]={".red",".green",".blue"};
    int color[3];
    for(int i = 0; i < 3; ++i)
    {
        const TiXmlNode* itemNode = GetItemNode(item + colorName[i]);

        if( NULL == itemNode)
            break;

        const TiXmlAttribute *attrib = itemNode->ToElement()->FirstAttribute();
        attrib = attrib->Next()->Next();
        
        // 保存结果
        NumericConverter::GetValue(color[i], attrib->Value());
    }
    
    return RGB(color[0], color[1], color[2]);
}

void ConfigMgr::SetStringValue( const std::string& item, const char *value )
{
    TiXmlNode* itemNode = GetItemNode(item);

    if( NULL == itemNode)
        return;

    TiXmlAttribute *attrib = itemNode->ToElement()->FirstAttribute();
    attrib = attrib->Next()->Next();
    if(attrib)
        attrib->SetValue(value);
}

void ConfigMgr::SaveConfig(const char *fileName)
{
    GetConfigXml()->SaveFile( fileName );
}