#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <string>

#ifdef __ANDROID__
#include <android/log.h>
#endif

typedef enum DebugLevel {
	LEVEL_VERBOSE = 2,
	LEVEL_DEBUG,
	LEVEL_INFO,
	LEVEL_WARN,
	LEVEL_ERROR,
	LEVEL_FATAL,
	LEVEL_SILENT,
} DebugLevel;

class Debug {
public:
	static std::wstring debugFile;

	static void v(const std::wstring tag, const std::wstring text);
	static void d(const std::wstring tag, const std::wstring text);
	static void i(const std::wstring tag, const std::wstring text);
	static void w(const std::wstring tag, const std::wstring text);
	static void e(const std::wstring tag, const std::wstring text);
	static void f(const std::wstring tag, const std::wstring text);

private:
	static void debug(const DebugLevel level, const std::wstring tag, const std::wstring text);
};

#endif