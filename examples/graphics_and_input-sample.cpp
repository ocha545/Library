#include"../include/Window.h"
#include"../include/Graphics.h"

using namespace Win32;

// キー入力によって四角形の色が変わるサンプルです
// A, W, S, E, D, R, F キーでボタンが押されたような表示になります

int main()
{
	Window window{ 480, 320, L"Draw Test" };
	GraphicsXI::SetClearColor(Color(0));
	GraphicsXI gfx{ window.getHandle(), window.getInstance() };

	Color upColor = Color(255);
	Color dnColor = Color(0, 0, 128);
	Color colors[7]{
		upColor,
		upColor,
		upColor,
		upColor,
		upColor,
		upColor,
		upColor,
	};

	Rect keys[7]{
		Rect{ 30, 60 },
		Rect{ 30, 60 },
		Rect{ 30, 60 },
		Rect{ 30, 60 },
		Rect{ 30, 60 },
		Rect{ 30, 60 },
		Rect{ 30, 60 },
	};

	while (window.update())
	{
		for (auto& color : colors)
		{
			color = upColor;
		}

		if (window.press(Key::A))
		{
			colors[0] = dnColor;
		}
		if (window.press(Key::W))
		{
			colors[1] = dnColor;
		}
		if (window.press(Key::S))
		{
			colors[2] = dnColor;
		}
		if (window.press(Key::E))
		{
			colors[3] = dnColor;
		}
		if (window.press(Key::D))
		{
			colors[4] = dnColor;
		}
		if (window.press(Key::R))
		{
			colors[5] = dnColor;
		}
		if (window.press(Key::F))
		{
			colors[6] = dnColor;
		}

		gfx.clear();

		for (int i = 0; i < 4; i++)
		{
			int index = i * 2;
			gfx.draw(keys[index].position(
				25 * index,
				70
			).color(colors[index]));
		}
		for (int i = 0; i < 3; i++)
		{
			int index = i * 2 + 1;
			gfx.draw(keys[index].position(
				25 * index,
				10
			).color(colors[index]));
		}
		gfx.present();
	}
}