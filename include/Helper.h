#pragma once
#include<windows.h>
#include<string>
#include<format>
#include<cstdlib>
#include<filesystem>

namespace Win32
{
	using std::string;
	using std::wstring;
	using std::string_view;
	using std::wstring_view;

	int GetWindowPosX(HWND hwnd);
	int GetWindowPosY(HWND hwnd);
	int GetWindowWidth(HWND hwnd);
	int GetWindowHeight(HWND hwnd);

	namespace Convert
	{
		const wstring WideStr(const string_view mb);
		const string MultiByteStr(const wstring_view wide);
	}
}
