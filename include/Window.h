#pragma once

#include<windows.h>
#include<shellapi.h>
#include<iostream>
#include<string>
#include<format>
#include<algorithm>
#include<cwctype>
#include<unordered_set>
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
	enum class Key : UINT
	{
		Backspace = VK_BACK,
		Tab = VK_TAB,
		Clear = VK_CLEAR,
		Enter = VK_RETURN,
		Shift = VK_SHIFT,
		Ctrl = VK_CONTROL,
		Alt,
		Pause,
		CapsLock,
		KANA,
		IME_HANGUL = KANA,
		IME_ON,
		IME_JUNJA,
		IME_Final,
		IME_HANJA,
		IME_KANJI = IME_HANJA,
		IME_OFF = VK_IME_OFF,
		Escape,
		IME_CONVERT,
		IME_NONCONVERT,
		IME_ACCEPT,
		IME_MODE_CHANGE,
		Space,
		PageUp,
		PageDown,
		End,
		Home,
		Left,
		Up,
		Right,
		Down,
		Select,
		Print,
		Execute,
		Snapshot,
		Insert,
		Delete,
		Help,
		NUM_0 = 0x30,
		NUM_1,
		NUM_2,
		NUM_3,
		NUM_4,
		NUM_5,
		NUM_6,
		NUM_7,
		NUM_8,
		NUM_9,
		A = 0x41,
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		I,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,
		S,
		T,
		U,
		V,
		W,
		X,
		Y,
		Z,
		Win,
		RWin,
		Apps,
		Sleep = VK_SLEEP,
		NUMPAD_0,
		NUMPAD_1,
		NUMPAD_2,
		NUMPAD_3,
		NUMPAD_4,
		NUMPAD_5,
		NUMPAD_6,
		NUMPAD_7,
		NUMPAD_8,
		NUMPAD_9,
		Multiply,
		Add,
		Separator,
		Subtract,
		Decimal,
		Divide,
		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
		F13,
		F14,
		F15,
		F16,
		F17,
		F18,
		F19,
		F20,
		F21,
		F22,
		F23,
		F24,
		NumLock = VK_NUMLOCK,
		Scroll,
		LShift = VK_LSHIFT,
		RShift,
		LCtrl,
		RCtrl,
		LAlt,
		RAlt,
		BROWSER_BACK,
		BROWSER_FORWARD,
		BROWSER_REFRESH,
		BROWSER_STOP,
		BROWSER_SEARCH,
		BROWSER_FAVORITES,
		BROWSER_HOME,
		VOLUMME_Mute,
		VOLUMME_Down,
		VOLUMME_Up,
		MEDIA_NextTrack,
		MEDIA_PrevTrack,
		MEDIA_STOP,
		MEDIA_PLAY_PAUSE,
		LAUNCH_MAIL,
		LAUNCH_MEDIA_SELECT,
		LAUNCH_APP1,
		LAUNCH_APP2,
		//ここから先は不要っぽいので書きません
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
	inline Key operator|(Key a, Key b)
	{
		return (Key)((UINT)a | (UINT)b);
	}

	using std::wstring;

	LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

	namespace Core
	{
		//path
		static wstring icon;// = LoadIconW(NULL, IDC_ICON);
		static wstring cursor;// = LoadCursorW(NULL, IDC_ARROW);
		static HDROP drop{};
		static bool isDrop = false;
		static std::unordered_set<UINT> downKeys;
		static std::unordered_set<UINT> prevKeys;


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
		bool down(Key key);
		bool up(Key key);
		bool press(Key key);

		HWND getHandle() const;
		HINSTANCE getInstance() const;
		bool isDropFile() const;
		std::vector<wstring> getDropFileName() const;

		// Windowクラスのインスタンスを親としてメッセージボックスを表示します
		Result showMessageBox(const wstring& title, const wstring& message, long flag) const;

		// Windowクラスのインスタンスを親としてメッセージボックスを表示します
		// flagはIcon::Information | Button::Okとしています
		Result showMessageBoxOk(const wstring& title, const wstring& message) const;

		// Windowクラスのインスタンスを親としてメッセージボックスを表示します
		// flagはIcon::Information | Button::YesNoとしています
		Result showMessageBoxYesNo(const wstring& title, const wstring& message) const;


		// カーソル画像をico形式で設定します
		static void SetIcon(const wstring& path);

		// カーソル画像をcur形式で設定します
		static void SetCursor(const wstring& path);

		static Result ShowMessageBox(HWND handle, HINSTANCE instance, const wstring& title, const wstring& message, long flag);
		static Result ShowMessageBox(const wstring& title, const wstring& message, long flag);
	};
}
