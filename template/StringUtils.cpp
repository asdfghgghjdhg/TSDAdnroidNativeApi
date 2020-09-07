#include "StringUtils.h"
#include "Debug.h"

std::wstring format(const std::wstring fmt, ...) {
    int size = ((int)fmt.size()) * 2 + 50;   // Use a rubric appropriate for your code
    std::wstring str;
    va_list ap;
    while (1) {     // Maximum two passes on a POSIX system...
        str.resize(size);
        va_start(ap, fmt);
        int n = vswprintf((wchar_t*)str.data(), size, fmt.c_str(), ap);
        va_end(ap);
        if (n > -1 && n < size) {  // Everything worked
            str.resize(n);
            return str;
        }
        if (n > -1)  // Needed size returned
            size = n + 1;   // For null char
        else
            size *= 2;      // Guess at a larger size (OS specific)
    }
    return str;
}

size_t getWCHARLength(const WCHAR_T* source) {
    size_t res = 0;
    WCHAR_T* tmpShort = (WCHAR_T*)source;

    while (*tmpShort++) {
       ++res;
    }

    return res;
}

size_t convToWCHAR(WCHAR_T** dest, const std::wstring source, size_t length) {
    if (!length) {
        length = source.length() + 1;
    }
    if (!*dest) {
        *dest = new WCHAR_T[length];
    }

    const wchar_t* src = source.c_str();
    WCHAR_T* tmpShort = *dest;
    wchar_t* tmpWChar = (wchar_t*)src;
    size_t res = 0;

    for (; length; --length, ++res, ++tmpWChar, ++tmpShort) {
        *tmpShort = (WCHAR_T)*tmpWChar;
    }

    return res;
}

std::wstring convFromWCHAR(const WCHAR_T* source, size_t length) {
    if (!length) {
        length = getWCHARLength(source) + 1;
    }

    wchar_t* dest = new wchar_t[length];
    wchar_t* tmpWChar = dest;
    WCHAR_T* tmpShort = (WCHAR_T*)source;
    size_t res = 0;

    for (; length; --length, ++res, ++tmpWChar, ++tmpShort) {
        *tmpWChar = (wchar_t)*tmpShort;
    }

    return std::wstring(dest);
}

std::wstring convFromJString(jstring source) {
    std::wstring ret;
    if (source) {
        JNIEnv* env = getJniEnv();
        const jchar* jChars = env->GetStringChars(source, NULL);
        jsize jLen = env->GetStringLength(source);
        ret.assign(jChars, jChars + jLen);
        env->ReleaseStringChars(source, jChars);
    }
    return ret;
}

//jstring convToJString(std::wstring source) {
//    jstring res;
//    if (source.length()) {
//        JNIEnv* env = getJniEnv();
//        res = env->NewString((jchar*)source.c_str(), source.length() + 1);
//    }

//    return res;
//}

std::wstring convFromV8String(tVariant* source) {
    std::wstring res;
    switch (source->vt) {
        case VTYPE_PSTR: {

            break;
        }
        case VTYPE_PWSTR: {
            if (source->pwstrVal && source->wstrLen) {
                res = convFromWCHAR(source->pwstrVal);
            }

            break;
        }
    }

    return res;
}

size_t convToV8String(tVariant* dest, const std::wstring source, IMemoryManager* manager) {
    size_t res = 0;
    dest->vt = VTYPE_EMPTY;
    if (source.length() && manager) {
        res = (source.length() + 1) * sizeof(WCHAR_T);
        if (manager->AllocMemory((void**)&dest->pwstrVal, (unsigned long)res)) {
            dest->vt = VTYPE_PWSTR;
            convToWCHAR(&dest->pwstrVal, source);
            dest->wstrLen = source.length();
        }
    }

    return res;
}

std::string convToString(std::wstring source) {
    size_t size = wcstombs(NULL, &source[0], 0);
    std::string res = std::string(size, 0);
    size = wcstombs(&res[0], &source[0], size);
    
    return res;
}