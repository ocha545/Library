#ifdef _DEBUG
//#define AUDIO_CODE
#define GRAPHICS_CODE
//#define WINDOW_CODE
//#define KEYBOARD_CODE
//#define HELPER_TEST
//#define FILE_TEST
#if 0

#include<iostream>
#include"include/Window.h"
#include"include/Graphics.h"
#include"include/Keyboard.h"
#include"include/Audio.h"
using namespace Win32;

int main()
{
	std::locale::global(std::locale("ja_JP.UTF-8"));
	std::wcout.imbue(std::locale());

	Window::SetIcon(L"images/icon.ico");
//	Window::SetCursor(L"images/cursor.cur");
	Window window{ 960, 720, L"たいとる", 10, 10 };
	GraphicsXI::ClearColor(Color(0, 0, 60));
	GraphicsXI gfx{ window.getHandle(), window.getInstance() };
	Input input{ window.getHandle(), window.getInstance() };
//	Audio audio{ L"music/Alexandrite-artworks.mp3" };
	Audio audio{ L"music/Blue.mp3" };
//	Audio audio{ L"music/True-Peak.flac" };
//	Audio audio{ L"music/audio.ogg" };
//	Audio audio{ L"music/AIZO.opus" };

	window.show();
	input.Acquire();
//	audio.Play();
	auto coverImage = Texture{ audio.MetaData().jacket };
	coverImage.scaled(0.5f);

	int r = 0;
	float theta = 0.0f;

	Texture image{ Image{L"images/yno.png"}.scaled(2) };

	while (window.update())
	{
		theta += 0.01f;
		input.Check();
		if (input.pressed(Key::UP))
		{
			r += 2;
		}
		else if (input.pressed(Key::DOWN))
		{
			r -= 2;
		}

		gfx.clear();
		gfx.draw(image.rotationX(theta).rotationZ(theta).get());
		gfx.present();
	}

	window.close();

	return 0;
}
#endif

#ifdef AUDIO_CODE
#include<crtdbg.h>

using namespace UsingAudio;
constexpr auto std_endl = '\n';

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF);
	std::locale::global(std::locale("ja_JP.UTF-8"));
	std::wcout.imbue(std::locale());
//  現在 wavファイル、mp3ファイル、flacファイルしか対応してないです。
// 	↑OGGファイルもサポートしました。
//  ↑Opusファイルもサポートしました。
//  DRライブラリがwav,mp3,flacしか対応してないんです。
//  これからイコライザー機能付けます
// 	↑XAudio2の方でできそう？
//	Audio music{ L"music/Blue.mp3" };
//	Audio music{ L"music/Prismatic Lollipops.mp3" };
//	Audio music{ L"music/Hacking to the Gate.mp3" };
//	Audio music{ L"music/higasimaru.mp3" };
//	Audio music{ L"music/Where Our Blue Is.mp3" };
	Audio music{ L"music/melissa.mp3" };
//	Audio music{ L"music/test.wav" };
//	Audio music{ L"music/Alexandrite.wav"};
//	Audio music{ L"music/nifu.wav" };
//	Audio music{ L"music/audio.ogg" };
//	Audio music{ L"music/True-Peak.flac" };
//	Audio music{ L"music/AIZO.opus" };

//	music.EnableReverb();
	music.Play();

	std::wcout << music.MetaData().album;
	std::wcout << L", Length: " << music.GetTimeW();
	const AudioMetaData& meta = music.MetaData();
	const AudioInfo& info = music.Information();

	while (music.State().BuffersQueued)
	{
		Sleep(16);
		system("cls");
		std::wcout << L"[Information]" << std_endl;
		std::wcout << L"  Album   :" << meta.album << std_endl;
		std::wcout << L"  Artist  :" << meta.artist << std_endl;
		std::wcout << L"  Length  :" << music.GetTimeW() << std_endl;
		std::wcout << L"  Comment :" << meta.comment << std_endl;
		std::wcout << L"  Genre   :" << meta.genre << std_endl;
		std::wcout << L"  Title   :" << meta.title << std_endl;
		std::wcout << L"  Track   :" << meta.track << std_endl;
		std::wcout << L"  Year    :" << meta.year << std_endl << std_endl;
		std::wcout << L"[Format Info]" << std_endl;
		std::wcout << L"  Format        :" << music.FormatString() << std_endl;
		std::wcout << L"  SampleRate    :" << info.SampleRate << std_endl;
		std::wcout << L"  Channels      :" << info.Channels << std_endl;
		std::wcout << L"  BitsPerSample :" << info.BitsPerSample << std_endl;
		std::wcout << L"  BlockAlign    :" << info.BlockAlign << std_endl;
		std::wcout << L"  AvgBytePerSec :" << info.nAvgBytesPerSec << std_endl << std_endl;
		std::wcout << L"[Voice State]" << std_endl;
		std::wcout << L"  Buffers Queued          :" << music.State().BuffersQueued << std_endl;
		std::wcout << L"  Current Buffer Context  :" << music.State().pCurrentBufferContext << std_endl;
		std::wcout << L"  Samples Played          :" << music.State().SamplesPlayed << std_endl;
		std::wcout << L"  Current Time            :" << music.GetNowTimeW() << std_endl << std_endl;
		std::wcout << std::flush;
	}

	return 0;
}
#endif

#ifdef GRAPHICS_CODE
#include"include/Window.h"
#include"include/Graphics.h"
#include<random>
using namespace Win32;

int main()
{
	std::random_device random;

	Window::SetIcon(L"images/icon.ico");
	Window window{ 960, 720, L"たいとる☺", Position::Centering };
	GraphicsXI::ClearColor(Color(40));
	GraphicsXI graphics{ window.getHandle(), window.getInstance() };

	window.show();

	Rect rect{ 100, 100 };
	rect.color(Color(255));
	int x = 0;
	int y = 0;

	Rect rect2{ 50, 50 };
	rect2.color(0xff, 0x80, 0x80).position(480.0f, 360.0f);
	float angle = 0.0f;

	Circle circle{ 80 };
	circle.position(600, 500).color(255, 0, 0);

	Image image{ L"images/yno.png" };
	std::wstring moji = L"やったぜ ぴょぴょぉん!";
	Texture texture1{ image };
	Texture text{ Text::GetTextImage(moji).swapped(Color(0), Color(255)).scaled(4)};

	while (window.update())
	{
		angle += 0.01f;
		graphics.clear();
		x++;
		y++;

		if (rect2.intersect(rect.get()))
		{
			std::cout << "Intersect";
		}

//		graphics.draw(rect.position(x, y).get());
//		graphics.draw(rect2.rotationZ(angle * 3).get());
//		graphics.draw(circle.get());
		graphics.draw(texture1.get());
		graphics.draw(text.color(255, 0, 0).get());
		graphics.present();
	}

	window.close();
}

#endif

#ifdef WINDOW_CODE

using namespace UsingWindow;
using namespace UsingDevice;

int main()
{
	Window window{ 960, 720, L"うぃんどう" };
	Input input{ window.getHandle(), window.getInstance() };

	std::locale::global(std::locale("ja_JP.UTF-8"));
	std::wcout.imbue(std::locale());

	window.setIcon(L"images/icon.ico");
	window.show();

	while (window.update())
	{
		input.Check();

		if (input.down(Key::Q))
		{
			window.messageBox(L"長い文章やってみるわよ！\n案外出来なかったりして。まったくWindowsは世話が焼ける", L"みってるー？", ICON::Information | BUTTON::YesNo);
//			window.toastMessage(L"いんひょ", L"たいっとっるる", L"チップスター");
		}
	}
}

#endif

#ifdef KEYBOARD_CODE

using namespace UsingDevice;
using namespace UsingGraphics;
using namespace UsingWindow;

int main()
{
	Window window{ 960, 720, L"キーボード" };
	GraphicsXI graphics{ window.getHandle(), window.getInstance() };
	Input kbd{ window.getHandle(), window.getInstance() };

	Rect rect{ 100, 100 };
	Circle mousePoint{ 10 };

	Color hexColor = Color::HEX(L"#Ff7aC1");
	std::wcout << L"R:" << hexColor.r << std::endl;
	std::wcout << L"G:" << hexColor.g << std::endl;
	std::wcout << L"B:" << hexColor.b << std::endl;
	std::wcout << L"A:" << hexColor.a << std::endl;

	window.show();
	kbd.Acquire();

	while (window.update())
	{
		graphics.clear(0, 0, 0);
		kbd.Check();

		auto [mx, my] = kbd.pos();

		if (kbd.down(Key::ENTER))
		{
			break;
		}
		graphics.draw(rect.position(100, 100).color(hexColor).get());
		graphics.draw(mousePoint.position(mx, my).color(Color{ 255, 0, 0 }).get());

		graphics.present();
	}
}

#endif

#ifdef HELPER_TEST

#include"include/Helper.h"

using namespace Win32_CPP;

int main()
{
	std::locale::global(std::locale("ja_JP.UTF-8"));
	std::wcout.imbue(std::locale());

	std::string multi = "Multiまるちマルチ丸値！";
	std::wstring wide = L"Wideわいどワイド賄度！";

	std::wcout << L"Conv Wide : " << Convert::WideStr(multi) << std::endl;
	std::cout << "Conv Multibyte : " << Convert::MultiByteStr(wide) << std::endl;

	system("pause");
}

#endif

#ifdef FILE_TEST
using Win32_CPP::DataFormat::CSV;

int main()
{
	CSV read{ "テキスト.csv"};

	system("pause");

	return 0;
}

#endif

#endif