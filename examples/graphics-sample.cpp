#include"../include/Window.h"
#include"../include/Graphics.h"
using namespace Win32;

int main()
{
	Window window{ 960, 720, L"ぐらふぃくす", Position::Centering };
	GraphicsXI::SetClearColor(Color(0));
	GraphicsXI gfx{ window.getHandle(), window.getInstance() };

	window.show();

	while (window.update())
	{
		gfx.clear();

		gfx.present();
	}
}