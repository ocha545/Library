#include"../../Window.h"
#include"../../Graphics.h"

using namespace Win32;

int main()
{
	Window window{ 960, 720, L"たいとる" };
	GraphicsXI gfx{ window.getHandle(), window.getInstance() };
	GraphicsXI::SetClearColor(Color(30, 30, 60));
	Texture texts[] =
	{
		Text::GetTextImage(L"Normal").swapped(Color(0), Color(255)).scaled(4),
		Text::GetTextImage(L"Gray Scale").swapped(Color(0), Color(255)).scaled(4)
	};
	Texture images[] =
	{
		Image{L"images/sm64.png"}.filtered(Filter::Invert).flipV(),
		Image{L"images/sm64.png"}.filtered(Filter::GrayScale)
	};

//	Image{ L"images/sm64.png" }.filtered(Filter::GrayScale).encodePNG(L"SuperTest.png");
//	Image{ L"images/sm64.png" }.filtered(Filter::GrayScale).encodeJPG(L"SuperTest.jpg");
//	Image{ L"images/sm64.png" }.filtered(Filter::GrayScale).encodeBMP(L"SuperTest.bmp");
//	Image{ L"images/sm64.png" }.filtered(Filter::GrayScale).encodeTGA(L"SuperTest.tga");

	window.show();

	while (window.update())
	{
		gfx.clear();

		texts[0].position(100, 50);
		images[0].position(25, 100);
		texts[1].position(500, 50);
		images[1].position(480, 100);

		gfx.draw(texts[0].get());
		gfx.draw(images[0].get());
		gfx.draw(texts[1].get());
		gfx.draw(images[1].get());
		gfx.present();
	}
}