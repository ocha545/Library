#pragma once

#include<windows.h>
#include<shellapi.h>
#include<iostream>
#include<string>
#include<format>
#include<algorithm>
#include<cwctype>
#include"Macros.h"
#define UsingWindow Win32_CPP::Window

namespace Win32
{
//	enum class Position : long;
//	enum class Result : long;
//	enum class Button : long;
//	enum class Icon : long;
//	inline Position operator|(Position a, Position b);
//	inline Position operator&(Position a, Position b);
//	inline long operator|(Button a, Icon b);
//	inline long operator|(Icon a, Button b);
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

	using refstr = const std::wstring&;

	LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

	namespace Core
	{
		static HICON icon;// = LoadIconW(NULL, IDC_ICON);
		static HCURSOR cursor;// = LoadCursorW(NULL, IDC_ARROW);

		//表示形式
		//[SECTION]: information
		void WriteLog(const std::wstring& section, const std::wstring& info);
	}

	class Window
	{
	private:
		int width;
		int height;
		refstr title;
		HWND window;
		HINSTANCE instance;

	public:
		Window(int w, int h, refstr title, Position position);
		Window(int w, int h, refstr title, Position position, refstr className);
		Window(int w, int h, refstr title, int x, int y);
		Window(int w, int h, refstr title, int x, int y, refstr className);
		~Window();

		bool update() const;
		void show() const;
		void close() const;

		HWND getHandle() const;
		HINSTANCE getInstance() const;
		Result showMessageBox(refstr title, refstr message, long flag) const;
		Result showMessageBoxOk(refstr title, refstr message) const;
		Result showMessageBoxYesNo(refstr title, refstr message) const;

		static void SetIcon(refstr path);
		static void SetCursor(refstr path);

		static Result ShowMessageBox(HWND handle, HINSTANCE instance, refstr title, refstr message, long flag);
		static Result ShowMessageBox(refstr title, refstr message, long flag);
//		static Result ShowToastMessage(HWND handle, HINSTANCE instance, refstr info, refstr title, refstr tip);
//		static Result ShowToastMessage(refstr info, refstr title, refstr tip);
	};
}


/*
namespace Win32_CPP
{
	namespace Window
	{
		//Windowコンストラクタで指定できます。
		//現状Centeringのみ(NoneはWindowの座標を0, 0にします)
		enum class POSITION
		{
			Centering = 0,
			None = 1,
		};

		enum class RESULT
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

		enum BUTTON
		{
			Ok = MB_OK,
			OkCancel = MB_OKCANCEL,
			AbortRetryIgnore = MB_ABORTRETRYIGNORE,
			YesNoCancel = MB_YESNOCANCEL,
			YesNo = MB_YESNO,
			RetryCancel = MB_RETRYCANCEL,
			CancelTryContinue = MB_CANCELTRYCONTINUE,
		};

		enum ICON
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

		LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
		{
			switch (msg)
			{
			case WM_DESTROY:
				PostQuitMessage(0);
				return 0;

			default:
				return DefWindowProcW(hWnd, msg, wp, lp);
			}
		}

		class Window
		{
		private:
			int width;
			int height;
			std::wstring title;
			HWND window;
			HINSTANCE instance = GetModuleHandleW(nullptr);
			HICON icon = LoadIconW(instance, IDC_ICON);
			HCURSOR cursor = LoadCursorW(NULL, IDC_ARROW);
			const UINT toastID = 1000u;
			static const UINT ToastID = 1001u;

		public:
			Window(int width, int height, const std::wstring& title, POSITION pos = POSITION::Centering)
				: width(width), height(height), title(title)
			{
				int x, y;
				//座標の設定
				{
					x = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
					y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

					if (pos != POSITION::Centering)
					{
						x = 0;
						y = 0;
					}
				}

				//ウィンドウの登録
				{
					WNDCLASSEXW	wndClass{};

					wndClass.cbSize = sizeof(WNDCLASSEXW);
					wndClass.style = CS_HREDRAW | CS_VREDRAW;
					wndClass.lpfnWndProc = WndProc;
					wndClass.hInstance = instance;
					wndClass.hIcon = icon;
					wndClass.hCursor = cursor;
					wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
					wndClass.lpszClassName = L"MAIN_WINDOW";

					RegisterClassExW(&wndClass);
				}

				//ウィンドウの作成
				{
					RECT windowRect{};
					windowRect.left = 0;
					windowRect.top = 0;
					windowRect.right = width;
					windowRect.bottom = height;
					AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);
					int windowWidth = windowRect.right - windowRect.left;
					int windowHeight = windowRect.bottom - windowRect.top;

					window = CreateWindowExW(
						NULL, L"MAIN_WINDOW", title.c_str(), WS_OVERLAPPEDWINDOW,
						x, y, windowWidth, windowHeight,
						NULL, FALSE, instance, NULL);
				}
			}

			virtual ~Window()
			{
				//通知の為の設定を削除
				std::wcout << L"~Window" << std::endl;
				NOTIFYICONDATAW nid{};
				nid.cbSize = sizeof(NOTIFYICONDATAW);
				nid.hWnd = this->window;
				nid.uID = this->toastID;
				std::wcout << ((Shell_NotifyIconW(NIM_DELETE, &nid) == TRUE) ? L"TRUE" : L"FALSE") << std::endl;
			}

			void setIcon(const std::wstring& path)
			{
				icon = (HICON)LoadImageW(instance, path.c_str(), IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
				SendMessageW(window, WM_SETICON, ICON_BIG, (LPARAM)icon);
				SendMessageW(window, WM_SETICON, ICON_SMALL, (LPARAM)icon);
			}

			void setCursor(const std::wstring& path)
			{
				cursor = LoadCursorFromFileW(path.c_str());
				SetClassLongPtrW(window, GCLP_HCURSOR, (LONG_PTR)cursor);
				::SetCursor(cursor);
			}

			RETURN HWND getHandle() const
			{
				return window;
			}

			RETURN HINSTANCE getInstance() const
			{
				return instance;
			}

			bool update()
			{
				MSG msg{};
				while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessageW(&msg);
				}

				return !(msg.message == WM_QUIT);
			}

			void show() const
			{
				ShowWindow(window, SW_SHOW);
			}

			void close() const
			{
				PostQuitMessage(0);
				PostMessageW(window, WM_CLOSE, 0, 0);
			}

			// こっからはWin32の機能集
			// テキトーに作ったのでもしかすると動かないかも？
			RESULT messageBox(const std::wstring& text, const std::wstring& title, long flag = BUTTON::Ok) const
			{
				return ShowMessageBox(window, text.c_str(), title.c_str(), flag);
			}

			void toastMessage(const std::wstring& info, const std::wstring& title, const std::wstring& tip) const
			{
				std::wcout << L"Ctor" << std::endl;
				NOTIFYICONDATAW toastIcon{};
				toastIcon.cbSize = sizeof(NOTIFYICONDATAW);
				toastIcon.hWnd = this->window;
				toastIcon.hIcon = this->icon;
				toastIcon.uID = this->toastID;
				toastIcon.uFlags = NIF_ICON | NIF_TIP;
				lstrcpyW(toastIcon.szTip, L"NotifyMessage...");
				std::wcout << ((Shell_NotifyIconW(NIM_ADD, &toastIcon) == TRUE) ? L"TRUE" : L"FALSE") << std::endl;


				std::wcout << L"Func" << std::endl;
				NOTIFYICONDATAW toast{};
				toast.cbSize = sizeof(NOTIFYICONDATAW);
				toast.hWnd = this->window;
				toast.hIcon = this->icon;
				toast.hBalloonIcon = icon;
				toast.uFlags = NIF_INFO;
				toast.uID = this->toastID;
				toast.dwInfoFlags = NIIF_INFO;
//				memcpy(toast.szInfo, L"いんふぉ", sizeof(L"いんふぉ"));
//				memcpy(toast.szInfoTitle, L"たいとる", sizeof(L"たいとる"));
//				memcpy(toast.szTip, L"ちっぷ", sizeof(L"ちっぷ"));
				lstrcpyW(toast.szInfo, info.c_str());
				lstrcpyW(toast.szInfoTitle, title.c_str());
				lstrcpyW(toast.szTip, tip.c_str());
				std::wcout << toast.szInfo << std::endl;
				std::wcout << toast.szInfoTitle << std::endl;
				std::wcout << toast.szTip << std::endl;
				std::wcout << ((Shell_NotifyIconW(NIM_MODIFY, &toast) == TRUE) ? L"TRUE" : L"FALSE") << std::endl;
			}

			static RESULT ShowMessageBox(HWND handle, const std::wstring& text, const std::wstring& title, long flag = BUTTON::Ok)
			{
				MSGBOXPARAMSW params{};

				params.cbSize = sizeof(MSGBOXPARAMSW);
				params.hwndOwner = NULL;
				params.hInstance = NULL;
				params.lpszText = text.c_str();
				params.lpszCaption = title.c_str();
				params.dwStyle = flag;
				switch (MessageBoxIndirectW(&params))
				{
				case IDOK:			return RESULT::Ok;
				case IDCANCEL:		return RESULT::Cancel;
				case IDABORT:		return RESULT::Abort;
				case IDRETRY:		return RESULT::Retry;
				case IDIGNORE:		return RESULT::Ignore;
				case IDYES:			return RESULT::Yes;
				case IDNO:			return RESULT::No;
				case IDTRYAGAIN:	return RESULT::TryAgain;
				case IDCONTINUE:	return RESULT::Continue;
				}
				return RESULT::Null;
			}

			static void ToastMessage(const std::wstring& info, const std::wstring& title, const std::wstring& tip)
			{
				std::wcout << L"Ctor" << std::endl;
				NOTIFYICONDATAW toastIcon{};
				toastIcon.cbSize = sizeof(NOTIFYICONDATAW);
				toastIcon.hWnd = NULL;
				toastIcon.uID = ToastID;
				toastIcon.uFlags = NIF_ICON | NIF_TIP;
				lstrcpyW(toastIcon.szTip, L"NotifyMessage...");
				std::wcout << ((Shell_NotifyIconW(NIM_ADD, &toastIcon) == TRUE) ? L"TRUE" : L"FALSE") << std::endl;

				std::wcout << L"Func" << std::endl;
				NOTIFYICONDATAW toast{};
				toast.cbSize = sizeof(NOTIFYICONDATAW);
				toast.hWnd = NULL;
				toast.uFlags = NIF_INFO;
				toast.uID = ToastID;
				toast.dwInfoFlags = NIIF_INFO;
				lstrcpyW(toast.szInfo, info.c_str());
				lstrcpyW(toast.szInfoTitle, title.c_str());
				lstrcpyW(toast.szTip, tip.c_str());
				std::wcout << toast.szInfo << std::endl;
				std::wcout << toast.szInfoTitle << std::endl;
				std::wcout << toast.szTip << std::endl;
				std::wcout << ((Shell_NotifyIconW(NIM_MODIFY, &toast) == TRUE) ? L"TRUE" : L"FALSE") << std::endl;

				//通知の為の設定を削除
				std::wcout << L"~Window" << std::endl;
				NOTIFYICONDATAW nid{};
				nid.cbSize = sizeof(NOTIFYICONDATAW);
				nid.hWnd = NULL;
				nid.uID = ToastID;
				std::wcout << ((Shell_NotifyIconW(NIM_DELETE, &nid) == TRUE) ? L"TRUE" : L"FALSE") << std::endl;
			}
		};
	}
}
*/