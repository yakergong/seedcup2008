#pragma once

#include <sstream>

#include "CharsetConverter.h"

/// �ַ����ַ���֮��ת���Ĺ�����
class NumericConverter
{
public:
    /// ���ַ���ת��Ϊ����
    /// @param OutputType Ҫ��������ֵ�����
    /// @retval false ת��ʧ��
    template <class OutputType>
    static void GetValue(OutputType& value, const wchar_t* str)
    {
        std::string idStr = CharsetConverter::uni_to_str(str);
        std::stringstream ss;
        ss << idStr.c_str();
        ss >> value;
    }
    
    /// ���ַ���ת��Ϊ����
    /// @param OutputType Ҫ��������ֵ�����
    /// @retval false ת��ʧ��
    template <class OutputType>
    static void GetValue(OutputType& value, const char* str)
    {
        std::stringstream ss;
        ss << str;
        ss >> value;
    }
    
    /// �����ֵ�ת��Ϊ�ַ���
    template<class InputType>
    static std::wstring to_wstr(InputType value)
    {
        std::wstringstream converter;
        converter << value;
        return converter.str();
    };
};