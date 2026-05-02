#include"../include/Window.h"

namespace Win32
{
	void Core::WriteLog(const std::wstring& section, const std::wstring& info)
	{
#ifdef ENABLE_WRITE_LOGS
		std::wstring tmp;
		for (const auto& sect : section)
		{
			tmp.push_back((wchar_t)std::towupper(sect));
		}
		std::wcout << std::format(L"[{}] - {}", tmp, info);
#endif
	}

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
/*
	Window::Window(int w, int h, const wstring& title)
		: width(w), height(h), title(title),
		  window(nullptr),
		  instance(GetModuleHandleW(nullptr))
	{
		int x = (::GetSystemMetrics(SM_CXSCREEN) - width) >> 1;
		int y = (::GetSystemMetrics(SM_CYSCREEN) - height) >> 1;
		const wchar_t className[] = L"Win32Window by otya!";

		//ウィンドウの登録
		{
			WNDCLASSEXW windowClass{};
			windowClass.cbSize = sizeof(WNDCLASSEXW);
			windowClass.style = CS_HREDRAW | CS_VREDRAW;
			windowClass.lpfnWndProc = WndProc;
			windowClass.hInstance = instance;
			if (!Core::icon.empty()) {
				windowClass.hIcon = (HICON)LoadImageW(instance, Core::icon.c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
			}
			else {
				windowClass.hIcon = LoadIconW(NULL, IDC_ICON);
			}

			if (!Core::cursor.empty()) {
				windowClass.hCursor = (HICON)LoadImageW(instance, Core::cursor.c_str(), IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE);
			}
			else {
				windowClass.hCursor = LoadCursorW(NULL, IDC_ARROW);
			}
			windowClass.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
			windowClass.lpszClassName = className;
			RegisterClassExW(&windowClass);
		}
		//ウィンドウの作成
		{
			RECT windowRect{};
			windowRect.left = 0;
			windowRect.top = 0;
			windowRect.right = width;
			windowRect.bottom = height;
			AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);
			//ウィンドウの使える領域にする
			//タイトルバーも考慮されたウィンドウサイズになる
			int windowWidth = windowRect.right - windowRect.left;
			int windowHeight = windowRect.bottom - windowRect.top;

			window = CreateWindowExW(
				NULL, className, title.c_str(), WS_OVERLAPPEDWINDOW,
				x, y, windowWidth, windowHeight,
				NULL, FALSE, instance, NULL
			);
		}
	}
*/
	Window::Window(int w, int h, const wstring& title, bool startup)
		: width(w), height(h), title(title),
		window(nullptr),
		instance(GetModuleHandleW(nullptr))
	{
		int x = (::GetSystemMetrics(SM_CXSCREEN) - width) >> 1;
		int y = (::GetSystemMetrics(SM_CYSCREEN) - height) >> 1;
		const wchar_t className[] = L"Win32Window by otya!";

		//ウィンドウの登録
		{
			WNDCLASSEXW windowClass{};
			windowClass.cbSize = sizeof(WNDCLASSEXW);
			windowClass.style = CS_HREDRAW | CS_VREDRAW;
			windowClass.lpfnWndProc = WndProc;
			windowClass.hInstance = instance;
			if (!Core::icon.empty()) {
				windowClass.hIcon = (HICON)LoadImageW(instance, Core::icon.c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
			}
			else {
				windowClass.hIcon = LoadIconW(NULL, IDC_ICON);
			}

			if (!Core::cursor.empty()) {
				windowClass.hCursor = (HICON)LoadImageW(instance, Core::cursor.c_str(), IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE);
			}
			else {
				windowClass.hCursor = LoadCursorW(NULL, IDC_ARROW);
			}
			windowClass.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
			windowClass.lpszClassName = className;
			RegisterClassExW(&windowClass);
		}
		//ウィンドウの作成
		{
			RECT windowRect{};
			windowRect.left = 0;
			windowRect.top = 0;
			windowRect.right = width;
			windowRect.bottom = height;
			AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);
			//ウィンドウの使える領域にする
			//タイトルバーも考慮されたウィンドウサイズになる
			int windowWidth = windowRect.right - windowRect.left;
			int windowHeight = windowRect.bottom - windowRect.top;

			window = CreateWindowExW(
				NULL, className, title.c_str(), WS_OVERLAPPEDWINDOW,
				x, y, windowWidth, windowHeight,
				NULL, FALSE, instance, NULL
			);
		}

		if (startup)
		{
			show();
		}
	}
	Window::Window(int w, int h, const wstring& title, Position position)
		: width(w), height(h), title(title),
		  window(nullptr),
		  instance(GetModuleHandleW(nullptr))
	{
		int x = 0;
		int y = 0;
		const wchar_t className[] = L"Win32Window by otya!";
		if ((position & Position::Centering) == Position::Centering)
		{
			x = (::GetSystemMetrics(SM_CXSCREEN) - width) >> 1;
			y = (::GetSystemMetrics(SM_CYSCREEN) - height) >> 1;
		}
		else if ((position & Position::Right) == Position::Right)
		{
			x = ::GetSystemMetrics(SM_CXSCREEN) - width;
		}
		else if ((position & Position::Bottom) == Position::Bottom)
		{
			y = ::GetSystemMetrics(SM_CYSCREEN) - height;
		}

		//ウィンドウの登録
		{
			WNDCLASSEXW windowClass{};
			windowClass.cbSize = sizeof(WNDCLASSEXW);
			windowClass.style = CS_HREDRAW | CS_VREDRAW;
			windowClass.lpfnWndProc = WndProc;
			windowClass.hInstance = instance;
			if (!Core::icon.empty()) {
				windowClass.hIcon = (HICON)LoadImageW(instance, Core::icon.c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
			}
			else {
				windowClass.hIcon = LoadIconW(NULL, IDC_ICON);
			}

			if (!Core::cursor.empty()) {
				windowClass.hCursor = (HICON)LoadImageW(instance, Core::cursor.c_str(), IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE);
			}
			else {
				windowClass.hCursor = LoadCursorW(NULL, IDC_ARROW);
			}
			windowClass.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
			windowClass.lpszClassName = className;
			RegisterClassExW(&windowClass);
		}
		//ウィンドウの作成
		{
			RECT windowRect{};
			windowRect.left = 0;
			windowRect.top = 0;
			windowRect.right = width;
			windowRect.bottom = height;
			AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);
			//ウィンドウの使える領域にする
			//タイトルバーも考慮されたウィンドウサイズになる
			int windowWidth  = windowRect.right - windowRect.left;
			int windowHeight = windowRect.bottom - windowRect.top;

			window = CreateWindowExW(
				NULL, className, title.c_str(), WS_OVERLAPPEDWINDOW,
				x, y, windowWidth, windowHeight,
				NULL, FALSE, instance, NULL
			);
		}
	}
	Window::Window(int w, int h, const wstring& title, Position position, const wstring& className)
		: width(w), height(h), title(title),
		  window(nullptr),
		  instance(GetModuleHandleW(nullptr))
	{
		int x = 0;
		int y = 0;
		if ((position & Position::Centering) == Position::Centering)
		{
			x = (::GetSystemMetrics(SM_CXSCREEN) - width) >> 1;
			y = (::GetSystemMetrics(SM_CYSCREEN) - height) >> 1;
		}
		else if ((position & Position::Right) == Position::Right)
		{
			x = ::GetSystemMetrics(SM_CXSCREEN) - width;
		}
		else if ((position & Position::Bottom) == Position::Bottom)
		{
			y = ::GetSystemMetrics(SM_CYSCREEN) - height;
		}

		//ウィンドウの登録
		{
			WNDCLASSEXW windowClass{};
			windowClass.cbSize = sizeof(WNDCLASSEXW);
			windowClass.style = CS_HREDRAW | CS_VREDRAW;
			windowClass.lpfnWndProc = WndProc;
			windowClass.hInstance = instance;
			if (!Core::icon.empty()) {
				windowClass.hIcon = (HICON)LoadImageW(instance, Core::icon.c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
			}
			else {
				windowClass.hIcon = LoadIconW(NULL, IDC_ICON);
			}

			if (!Core::cursor.empty()) {
				windowClass.hCursor = (HICON)LoadImageW(instance, Core::cursor.c_str(), IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE);
			}
			else {
				windowClass.hCursor = LoadCursorW(NULL, IDC_ARROW);
			}
			windowClass.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
			windowClass.lpszClassName = className.c_str();
			RegisterClassExW(&windowClass);
		}
		//ウィンドウの作成
		{
			RECT windowRect{};
			windowRect.left = 0;
			windowRect.top = 0;
			windowRect.right = width;
			windowRect.bottom = height;
			AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);
			//ウィンドウの使える領域にする
			//タイトルバーも考慮されたウィンドウサイズになる
			int windowWidth = windowRect.right - windowRect.left;
			int windowHeight = windowRect.bottom - windowRect.top;

			window = CreateWindowExW(
				NULL, className.c_str(), title.c_str(), WS_OVERLAPPEDWINDOW,
				x, y, windowWidth, windowHeight,
				NULL, FALSE, instance, NULL
			);
		}
	}
	Window::Window(int w, int h, const wstring& title, int x, int y)
		: width(w), height(h), title(title),
		  window(nullptr),
		  instance(GetModuleHandleW(nullptr))
	{

		Core::WriteLog(L"SecTion", L"Info");
		const wchar_t className[] = L"Win32Window by otya!";
		//ウィンドウの登録
		{
			WNDCLASSEXW windowClass{};
			windowClass.cbSize = sizeof(WNDCLASSEXW);
			windowClass.style = CS_HREDRAW | CS_VREDRAW;
			windowClass.lpfnWndProc = WndProc;
			windowClass.hInstance = instance;
			if (!Core::icon.empty()) {
				windowClass.hIcon = (HICON)LoadImageW(instance, Core::icon.c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
			}
			else {
				windowClass.hIcon = LoadIconW(NULL, IDC_ICON);
			}

			if (!Core::cursor.empty()) {
				windowClass.hCursor = (HICON)LoadImageW(instance, Core::cursor.c_str(), IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE);
			}
			else {
				windowClass.hCursor = LoadCursorW(NULL, IDC_ARROW);
			}
			windowClass.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
			windowClass.lpszClassName = className;
			RegisterClassExW(&windowClass);
		}
		//ウィンドウの作成
		{
			RECT windowRect{};
			windowRect.left = 0;
			windowRect.top = 0;
			windowRect.right = width;
			windowRect.bottom = height;
			AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);
			//ウィンドウの使える領域にする
			//タイトルバーも考慮されたウィンドウサイズになる
			int windowWidth = windowRect.right - windowRect.left;
			int windowHeight = windowRect.bottom - windowRect.top;

			window = CreateWindowExW(
				NULL, className, title.c_str(), WS_OVERLAPPEDWINDOW,
				x, y, windowWidth, windowHeight,
				NULL, FALSE, instance, NULL
			);
		}
	}
	Window::Window(int w, int h, const wstring& title, int x, int y, const wstring& className)
		: width(w), height(h), title(title),
		  window(nullptr),
		  instance(GetModuleHandleW(nullptr))
	{
		//ウィンドウの登録
		{
			WNDCLASSEXW windowClass{};
			windowClass.cbSize = sizeof(WNDCLASSEXW);
			windowClass.style = CS_HREDRAW | CS_VREDRAW;
			windowClass.lpfnWndProc = WndProc;
			windowClass.hInstance = instance;
			if (!Core::icon.empty()) {
				windowClass.hIcon = (HICON)LoadImageW(instance, Core::icon.c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
			}
			else {
				windowClass.hIcon = LoadIconW(NULL, IDC_ICON);
			}

			if (!Core::cursor.empty()) {
				windowClass.hCursor = (HICON)LoadImageW(instance, Core::cursor.c_str(), IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE);
			}
			else {
				windowClass.hCursor = LoadCursorW(NULL, IDC_ARROW);
			}
			windowClass.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
			windowClass.lpszClassName = className.c_str();
			RegisterClassExW(&windowClass);
		}
		//ウィンドウの作成
		{
			RECT windowRect{};
			windowRect.left = 0;
			windowRect.top = 0;
			windowRect.right = width;
			windowRect.bottom = height;
			AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);
			//ウィンドウの使える領域にする
			//タイトルバーも考慮されたウィンドウサイズになる
			int windowWidth = windowRect.right - windowRect.left;
			int windowHeight = windowRect.bottom - windowRect.top;

			window = CreateWindowExW(
				NULL, className.c_str(), title.c_str(), WS_OVERLAPPEDWINDOW,
				x, y, windowWidth, windowHeight,
				NULL, FALSE, instance, NULL
			);
		}
	}
	Window::~Window()
	{
	}

	bool Window::update() const
	{
		MSG message{};
		while (PeekMessageW(&message, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessageW(&message);
		}

		return !(message.message == WM_QUIT);
	}
	void Window::show() const
	{
		::ShowWindow(window, SW_SHOW);
	}
	void Window::close() const
	{
		PostQuitMessage(0);
		PostMessageW(window, WM_CLOSE, 0, 0);
	}

	HWND Window::getHandle() const
	{
		return this->window;
	}
	HINSTANCE Window::getInstance() const
	{
		return this->instance;
	}

	Result Window::showMessageBox(const wstring& title, const wstring& message, long flag) const
	{
		return Window::ShowMessageBox(this->window, this->instance, title, message, flag);
	}
	Result Window::showMessageBoxOk(const wstring& title, const wstring& message) const
	{
		return Window::ShowMessageBox(this->window, this->instance, title, message, Button::Ok | Icon::Information);
	}
	Result Window::showMessageBoxYesNo(const wstring& title, const wstring& message) const
	{
		return Window::ShowMessageBox(this->window, this->instance, title, message, Button::YesNo | Icon::Information);
	}

	void Window::SetIcon(const wstring& path)
	{
		Core::icon = path;
	}
	void Window::SetCursor(const wstring& path)
	{
		Core::cursor = path;
	}

	Result Window::ShowMessageBox(HWND handle, HINSTANCE instance, const wstring& title, const wstring& message, long flag)
	{
		MSGBOXPARAMSW param{};
		param.cbSize = sizeof(MSGBOXPARAMSW);
		param.hwndOwner = handle;
		param.hInstance = instance;
		param.lpszCaption = title.c_str();
		param.lpszText = message.c_str();
		param.dwStyle = flag;

		switch (MessageBoxIndirectW(&param))
		{
		case IDOK:			return Result::Ok;
		case IDCANCEL:		return Result::Cancel;
		case IDABORT:		return Result::Abort;
		case IDRETRY:		return Result::Retry;
		case IDIGNORE:		return Result::Ignore;
		case IDYES:			return Result::Yes;
		case IDNO:			return Result::No;
		case IDTRYAGAIN:	return Result::TryAgain;
		case IDCONTINUE:	return Result::Continue;
		}
		return Result::Null;
	}
	Result Window::ShowMessageBox(const wstring& title, const wstring& message, long flag)
	{
		MSGBOXPARAMSW param{};
		param.cbSize = sizeof(MSGBOXPARAMSW);
		param.hwndOwner = NULL;
		param.hInstance = NULL;
		param.lpszCaption = title.c_str();
		param.lpszText = message.c_str();
		param.dwStyle = flag;

		switch (MessageBoxIndirectW(&param))
		{
		case IDOK:			return Result::Ok;
		case IDCANCEL:		return Result::Cancel;
		case IDABORT:		return Result::Abort;
		case IDRETRY:		return Result::Retry;
		case IDIGNORE:		return Result::Ignore;
		case IDYES:			return Result::Yes;
		case IDNO:			return Result::No;
		case IDTRYAGAIN:	return Result::TryAgain;
		case IDCONTINUE:	return Result::Continue;
		}
		return Result::Null;
	}
}