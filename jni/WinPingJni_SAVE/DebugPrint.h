#pragma once

void dbgprint(const wchar_t *func, int line, const wchar_t *fmt, ...);
#define DBGPRINT(fmt, ...) dbgprint(L"func", __LINE__, fmt, __VA_ARGS__)