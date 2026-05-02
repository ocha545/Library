#pragma once

#include<windows.h>
#include<shellapi.h>
#include<iostream>
#include<string>
#include<format>
#include<algorithm>
#include<cwctype>
#include"Macros.h"
#include"Helper.h"

namespace Win32
{
	enum class Position : long
	{
		None = 0,
		Centering = 1,
		Left = 1 << 1,
		Right = 1 << 2,
		Top = 1 << 3,
		Bottom = 1 << 4,
	};
	enum class Result : long
	{
		Ok = IDOK,
		Cancel = IDCANCEL,
		Abort = IDABORT,
		Retry = IDRETRY,
		Ignore = IDIGNORE,
		Yes = IDYES,
		No = IDNO,
		TryAgain = IDTRYAGAIN,
		Continue = IDCONTINUE,
		Null = NULL,
	};
	enum class Button : long
	{
		Ok = MB_OK,
		OkCancel = MB_OKCANCEL,
		AbortRetryIgnore = MB_ABORTRETRYIGNORE,
		YesNoCancel = MB_YESNOCANCEL,
		YesNo = MB_YESNO,
		RetryCancel = MB_RETRYCANCEL,
		CancelTryContinue = MB_CANCELTRYCONTINUE,
	};
	enum class Icon : long
	{
		Hand = MB_ICONERROR,
		Stop = MB_ICONERROR,
		Error = MB_ICONERROR,
		Question = MB_ICONQUESTION,
		Exclamation = MB_ICONWARNING,
		Warning = MB_ICONWARNING,
		Asterisk = MB_ICONINFORMATION,
		Information = MB_ICONINFORMATION,
	};

	inline Position operator|(Position a, Position b)
	{
		return static_cast<Position>(
			static_cast<long>(a) | static_cast<long>(b)
			);
	}
	inline Position operator&(Position a, Position b)
	{
		return static_cast<Position>(
			static_cast<long>(a) & static_cast<long>(b)
			);
	}
	inline long operator|(Button a, Icon b)
	{
		return static_cast<long>(a) | static_cast<long>(b);
	}
	inline long operator|(Icon a, Button b)
	{
		return static_cast<long>(a) | static_cast<long>(b);
	}

	using std::wstring;

	LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

	namespace Core
	{
		//path
		static wstring icon;// = LoadIconW(NULL, IDC_ICON);
		static wstring cursor;// = LoadCursorW(NULL, IDC_ARROW);
		
		//表示形式 (未使用)
		//[SECTION]: information
		void WriteLog(const std::wstring& section, const std::wstring& info);
	}

	class Window
	{
	private:
		const wstring& title;
		HWND window;
		HINSTANCE instance;

	public:
		int width;
		int height;

		Window(int w, int h, const wstring& title, bool startup = true);
		Window(int w, int h, const wstring& title, Position position);
		Window(int w, int h, const wstring& title, Position position, const wstring& className);
		Window(int w, int h, const wstring& title, int x, int y);
		Window(int w, int h, const wstring& title, int x, int y, const wstring& className);
		~Window();

		bool update() const;
		void show() const;
		void close() const;

		HWND getHandle() const;
		HINSTANCE getInstance() const;

		/// Windowクラスのインスタンスを親としてメッセージボックスを表示します
		Result showMessageBox(const wstring& title, const wstring& message, long flag) const;

		/// Windowクラスのインスタンスを親としてメッセージボックスを表示します
		/// flagはIcon::Information | Button::Okとしています
		Result showMessageBoxOk(const wstring& title, const wstring& message) const;

		/// Windowクラスのインスタンスを親としてメッセージボックスを表示します
		/// flagはIcon::Information | Button::YesNoとしています
		Result showMessageBoxYesNo(const wstring& title, const wstring& message) const;


		/// カーソル画像をico形式で設定します
		static void SetIcon(const wstring& path);

		/// カーソル画像をcur形式で設定します
		static void SetCursor(const wstring& path);

		static Result ShowMessageBox(HWND handle, HINSTANCE instance, const wstring& title, const wstring& message, long flag);
		static Result ShowMessageBox(const wstring& title, const wstring& message, long flag);
	};
}
