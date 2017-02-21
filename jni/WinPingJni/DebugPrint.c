#pragma once

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

#ifdef _DEBUG

#include <Windows.h>

#include <stdarg.h>
#include <strsafe.h>
#include <malloc.h>

void dbgprint(const wchar_t *func, int line, const wchar_t *fmt, ...) {
	INT cbFormatString = 0;
	va_list args;
	PWCHAR wszDebugString = NULL;
	size_t st_Offset = 0;

	va_start(args, fmt);

	cbFormatString = _scwprintf(L"[%s:%d] ", func, line) * sizeof(WCHAR);
	cbFormatString += _vscwprintf(fmt, args) * sizeof(WCHAR) + 2;

	/* Depending on the size of the format string, allocate space on the stack or the heap. */
	wszDebugString = (PWCHAR)_malloca(cbFormatString);

	/* Populate the buffer with the contents of the format string. */
	StringCbPrintfW(wszDebugString, cbFormatString, L"[%s:%d] ", func, line);
	StringCbLengthW(wszDebugString, cbFormatString, &st_Offset);
	StringCbVPrintfW(&wszDebugString[st_Offset / sizeof(WCHAR)], cbFormatString - st_Offset, fmt, args);

	OutputDebugStringW(wszDebugString);

	_freea(wszDebugString);
	va_end(args);
}

#endif
