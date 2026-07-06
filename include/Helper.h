#pragma once
#include<windows.h>
#include<string>
#include<vector>
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
	POINT GetWindowPos(HWND hwnd);

	POINT operator-(POINT a, POINT b);
	POINT operator+(POINT a, POINT b);

	namespace String
	{
		const wstring WideStr(const string_view mb);
		const string MultiByteStr(const wstring_view wide);
		std::vector<std::wstring> Split(const wstring& text, wchar_t spl);
	}
}
