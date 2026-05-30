#include"../include/Helper.h"

namespace Win32
{
	int GetWindowPosX(HWND hwnd)
	{
		static RECT rect{};
		GetClientRect(hwnd, &rect);

		return rect.left;
	}
	int GetWindowPosY(HWND hwnd)
	{
		static RECT rect{};
		GetClientRect(hwnd, &rect);

		return rect.top;
	}
	int GetWindowWidth(HWND hwnd)
	{
		static RECT rect{};
		GetClientRect(hwnd, &rect);

		return rect.right - rect.left;

	}
	int GetWindowHeight(HWND hwnd)
	{
		static RECT rect{};
		GetClientRect(hwnd, &rect);

		return rect.bottom - rect.top;
	}

	namespace String
	{
		const std::wstring WideStr(const string_view mb)
		{
			return std::filesystem::path(mb).wstring();
		}
		const std::string MultiByteStr(const wstring_view wide)
		{
			return std::filesystem::path(wide).string();
		}
		std::vector<std::wstring> Split(const wstring& text, wchar_t spl)
		{
			std::vector<std::wstring> out;
			std::wstringstream wstream{ text };
			std::wstring tmp;
			while (std::getline(wstream, tmp, spl))
			{
				out.emplace_back(tmp);
			}
			return out;
		}
	}
}
