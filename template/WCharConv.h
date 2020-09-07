#ifndef __WCHARCONV_H__
#define __WCHARCONV_H__

#include <string>
#include "../include/types.h"

std::wstring format(const std::wstring fmt, ...);

class WCharConv {
public:
    static size_t getWCHARLength(const WCHAR_T* source);
    static size_t convToWCHAR(WCHAR_T** dest, const std::wstring source, size_t length = 0);
    static std::wstring convFromWCHAR(const WCHAR_T* source, size_t length = 0);
};

#endif