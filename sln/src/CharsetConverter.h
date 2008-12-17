#pragma once

#include <string>

/**
    �ַ���ת������ 
*/
class CharsetConverter 
{
public:
    /// ��unicode�ַ���(utf-16��ʽ)ת��Ϊstring�ַ���
    /// @note unicode�ַ��������ݱ���ȫ��ΪASCII�ַ�(��Ӣ����ĸ,���ŵ�)
    static std::string  uni_to_str(const wchar_t *unicode);
    
    /// ��utf-8������ַ���ת��Ϊwstring�ַ���
    /// @param[in] utf8 utf-8����Ĵ�ת���ַ���
    static std::wstring utf8_to_ws(const char  *utf8);
};
