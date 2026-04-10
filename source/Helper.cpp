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

	namespace Convert
	{
		const std::wstring WideStr(const string_view mb)
		{
			return std::filesystem::path(mb).wstring();
		}
		const std::string MultiByteStr(const wstring_view wide)
		{
			return std::filesystem::path(wide).string();
		}
	}
}
