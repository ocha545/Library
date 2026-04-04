//実際には"include/Window.h"で良い
#include"../include/Window.h"
using namespace Win32;

int main()
{
	// width, height, title, position
	Window::SetCursor(L"images/cursor.cur");
	Window::SetIcon(L"images/icon.ico");
	Window window{ 960, 720, L"Title", Position::Centering };

	window.show();
	Result res = window.showMessageBox(L"こんにちは！", L"元気ですか？", Icon::Information | Button::YesNo);

	if (res == Result::Yes)
	{
		window.showMessageBoxOk(L"どうも！", L"元気でよろしい！");
	}
	else
	{
		window.showMessageBoxOk(L"あぁ", L"そう。");
	}

	while (window.update())
	{
	}
}