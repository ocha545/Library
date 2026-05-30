#include"../include/Window.h"
#include"../include/Graphics.h"
using namespace Win32;

int main()
{
	Window window{ 960, 720,L"Game Input Sample" };
	GraphicsXI::SetClearColor(Color(0));
	GraphicsXI gfx{ window.getHandle(), window.getInstance() };
	Rect player{ 100,100 };
	int x = 0, y = 0;

	while (window.update())
	{
		if (window.press(Key::Left))
		{
			x -= 6;
		}
		if (window.press(Key::Right))
		{
			x += 6;
		}
		if (window.press(Key::Up))
		{
			y -= 6;
		}
		if (window.press(Key::Down))
		{
			y += 6;
		}


		gfx.clear();

		gfx.draw(player.position(x, y).color(255).get());

		gfx.present();
	}
}