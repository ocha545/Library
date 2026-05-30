#include"../include/Window.h"
using namespace Win32;

int main()
{
	Window window{ 960, 720, L"ウィンドウで入力チェック" };

	while (window.update())
	{
		if (window.down(Key::A))
		{
			window.showMessageBox(L"入力！", L"Aキーが押されました！", Icon::Information | Button::Ok);
		}
	}

	return 0;
}