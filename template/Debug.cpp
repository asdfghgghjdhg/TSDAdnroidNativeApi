#include "Debug.h"

std::wstring Debug::debugFile = L"";

void Debug::debug(const DebugLevel level, const std::wstring tag, const std::wstring text) {
#ifdef __ANDROID__
	const wchar_t* w_tag = tag.c_str();
	const wchar_t* w_text = text.c_str();
	size_t tag_size = (wcslen(w_tag) + 1) * sizeof(wchar_t);
	size_t text_size = (wcslen(w_text) + 1) * sizeof(wchar_t);
	char* c_tag = new char[tag_size];
	char* c_text = new char[text_size];

	std::wcstombs(c_tag, w_tag, tag_size);
	std::wcstombs(c_text, w_text, text_size);

	__android_log_write(level, c_tag, c_text);

	delete c_tag;
	delete c_text;
#endif
#ifdef _WINDOWS
	if (debugFile.length()) {
		FILE* stream = NULL;
		errno_t err = _wfopen_s(&stream, debugFile.c_str(), L"a");
		if (err == 0) {
			switch (level) {
				case LEVEL_VERBOSE:
					fwprintf(stream, L"%s/%s: %s\n", L"V", tag.c_str(), text.c_str());
					break;
				case LEVEL_DEBUG:
					fwprintf(stream, L"%s/%s: %s\n", L"D", tag.c_str(), text.c_str());
					break;
				case LEVEL_INFO:
					fwprintf(stream, L"%s/%s: %s\n", L"I", tag.c_str(), text.c_str());
					break;
				case LEVEL_WARN:
					fwprintf(stream, L"%s/%s: %s\n", L"W", tag.c_str(), text.c_str());
					break;
				case LEVEL_ERROR:
					fwprintf(stream, L"%s/%s: %s\n", L"E", tag.c_str(), text.c_str());
					break;
				case LEVEL_FATAL:
					fwprintf(stream, L"%s/%s: %s\n", L"F", tag.c_str(), text.c_str());
					break;
			}
			fclose(stream);
		}
	}
#endif
}

void Debug::v(const std::wstring tag, const std::wstring text) {
	debug(LEVEL_VERBOSE, tag, text);
}

void Debug::d(const std::wstring tag, const std::wstring text) {
	debug(LEVEL_DEBUG, tag, text);
}

void Debug::i(const std::wstring tag, const std::wstring text) {
	debug(LEVEL_INFO, tag, text);
}

void Debug::w(const std::wstring tag, const std::wstring text) {
	debug(LEVEL_WARN, tag, text);
}

void Debug::e(const std::wstring tag, const std::wstring text) {
	debug(LEVEL_ERROR, tag, text);
}

void Debug::f(const std::wstring tag, const std::wstring text) {
	debug(LEVEL_FATAL, tag, text);
}