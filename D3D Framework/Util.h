#pragma once

inline wchar_t* CharToWChar(char *mbString)
{
    int len = 0;
    len = (int)strlen(mbString) + 1;
    wchar_t *ucString = new wchar_t[len];
    mbstowcs(ucString, mbString, len);
    return ucString;
}

inline std::wstring intToStr(int i)
{
    wchar_t str[255];
    swprintf(str, 255, L"%d", i);
    return str;
}
