#include <Windows.h>
#include "CharsetConverter.h"

using namespace std;

std::wstring CharsetConverter::utf8_to_ws( const char *utf8 )
{
    int len = ::MultiByteToWideChar(CP_UTF8,0,utf8,(int)strlen(utf8),NULL,0);
    if (len == 0)
        return wstring(TEXT(""));
    TCHAR *unicode = new TCHAR[len+1];
    unicode[len]=L'\0';
    ::MultiByteToWideChar(CP_UTF8,0,utf8,(int)strlen(utf8),unicode,len+1);
    wstring ws(unicode);
    delete[] unicode;
    return ws;
}

std::string CharsetConverter::uni_to_str( const wchar_t* unicode )
{
    int nLen = ::WideCharToMultiByte(CP_ACP, 0, unicode, -1, NULL, 0, NULL, NULL);

    if (nLen<= 0) 
        return string("");

    char* pszDst = new char[nLen];
    if (NULL == pszDst) 
        return string("");

    ::WideCharToMultiByte(CP_ACP, 0, unicode, -1, pszDst, nLen, NULL, NULL);
    pszDst[nLen -1] = 0;

    string strTemp(pszDst);
    delete[] pszDst;

    return strTemp;
}