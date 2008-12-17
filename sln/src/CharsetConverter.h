#pragma once

#include <string>

/**
    字符集转换工具 
*/
class CharsetConverter 
{
public:
    /// 将unicode字符串(utf-16格式)转化为string字符串
    /// @note unicode字符串中内容必须全部为ASCII字符(如英语字母,符号等)
    static std::string  uni_to_str(const wchar_t *unicode);
    
    /// 将utf-8编码的字符串转换为wstring字符串
    /// @param[in] utf8 utf-8编码的待转换字符串
    static std::wstring utf8_to_ws(const char  *utf8);
};
