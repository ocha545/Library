#include"../include/Window.h"
#include"../include/Keyboard.h"
using namespace Win32;

int main()
{
	Window window{ 960, 720, L"キーボード" };
	Input input{ window.getHandle(), window.getInstance() };
	
	window.show();
	input.Acquire();

	while (window.update())
	{
		input.Check();

		if(input.down(Key::A))
		{
			window.showMessageBoxOk(L"たいとる", L"めっさげ");
		}
	}
}