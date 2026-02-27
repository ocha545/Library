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
		[[deprecated]]
		const wstring Old_WideStr(const string& mbstr, const wstring& emptyTxt)
		{
			if (mbstr.empty())
			{
				return emptyTxt;
			}
			int length = MultiByteToWideChar(
				CP_OEMCP,								//コードページ
				NULL,									//フラグ
				mbstr.c_str(),							//変換元のポインタ
				-1,										//サイズ指定(-1で自動計算)
				nullptr,								//変換先のポインタ
				0										//サイズ指定(0でバッファサイズが貰える)
			);
			if (length < 0)
			{
#if _MSVC_LANG >= 201402L
				return std::format(L"Length is 0...[{}],[{}]", GetLastError(), length);
#else
				return L"length is Zero...";
#endif
			}

			wstring tmp;
			tmp.resize(length);

#if _MSVC_LANG >= 201402L
			//			mbstowcs_s(&length, tmp.data(), tmp.size(), mbstr.c_str(), _TRUNCATE);
			MultiByteToWideChar(CP_OEMCP, NULL, mbstr.c_str(), -1, tmp.data(), length);
#else
			//			mbstowcs_s(&length, &tmp[0], tmp.size(), mbstr, _TRUNCATE);
			MultiByteToWideChar(CP_OEMCP, NULL, mbstr.c_str(), -1, &tmp[0], length);
#endif
			tmp.resize(length - 1);

			return tmp;
		}
		[[deprecated]]
		const string Old_MultiByteStr(const wstring& widestr, const string& emptyTxt)
		{
			if (widestr.empty())
			{
				return emptyTxt;
			}

			int length = WideCharToMultiByte(
				CP_OEMCP,			//コードページ
				NULL,				//フラグ
				widestr.c_str(),	//変換元のポインタ
				-1,					//サイズ指定 (-1で自動計算)
				nullptr,			//変換先のポインタ
				NULL,				//サイズ指定 (0で長さが帰ってくる)
				NULL,
				NULL
			);

			if (length < 0)
			{
#if _MSVC_LANG >= 201402L
				return std::format("Length is 0...[{}],[{}]", GetLastError(), length);
#else
				return "length is Zero...";
#endif
			}

			string tmp;
			tmp.resize(length);

#if _MSVC_LANG >= 201402L
			//			wcstombs_s(&length, tmp.data(), tmp.size(), widestr.c_str(), _TRUNCATE);
			WideCharToMultiByte(CP_OEMCP, NULL, widestr.c_str(), -1, tmp.data(), length, NULL, NULL);
#else
			//			wcstombs_s(&length, &tmp[0], tmp.size(), widestr, _TRUNCATE);
			WideCharToMultiByte(CP_OEMCP, NULL, widestr.c_str(), -1, &tmp[0], length, NULL, NULL);
#endif
			tmp.resize(length - 1);

			return tmp;
		}

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
