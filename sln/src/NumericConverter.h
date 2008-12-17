#pragma once

#include <sstream>

#include "CharsetConverter.h"

/// 字符与字符串之间转换的工具类
class NumericConverter
{
public:
    /// 将字符串转化为数字
    /// @param OutputType 要输出的数字的类型
    /// @retval false 转换失败
    template <class OutputType>
    static void GetValue(OutputType& value, const wchar_t* str)
    {
        std::string idStr = CharsetConverter::uni_to_str(str);
        std::stringstream ss;
        ss << idStr.c_str();
        ss >> value;
    }
    
    /// 将字符串转化为数字
    /// @param OutputType 要输出的数字的类型
    /// @retval false 转换失败
    template <class OutputType>
    static void GetValue(OutputType& value, const char* str)
    {
        std::stringstream ss;
        ss << str;
        ss >> value;
    }
    
    /// 将数字等转换为字符串
    template<class InputType>
    static std::wstring to_wstr(InputType value)
    {
        std::wstringstream converter;
        converter << value;
        return converter.str();
    };
};