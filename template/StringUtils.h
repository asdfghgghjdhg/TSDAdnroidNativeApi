#ifndef __STRINGUTILS_H__
#define __STRINGUTILS_H__

#include <string>
#include "../include/types.h"
#include "../jni/jnienv.h"
#include "../include/IMemoryManager.h"

std::wstring format(const std::wstring fmt, ...);

size_t getWCHARLength(const WCHAR_T* source);
size_t convToWCHAR(WCHAR_T** dest, const std::wstring source, size_t length = 0);
std::wstring convFromWCHAR(const WCHAR_T* source, size_t length = 0);

std::wstring convFromJString(jstring jstr);

std::wstring convFromV8String(tVariant* source);
size_t convToV8String(tVariant* dest, const std::wstring source, IMemoryManager* manager);

std::string convToString(std::wstring source);

#endif