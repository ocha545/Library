#ifdef _DEBUG
//#define AUDIO_CODE
//#define GRAPHICS_CODE
//#define WINDOW_CODE
//#define KEYBOARD_CODE
//#define HELPER_TEST
//#define FILE_TEST
//#define AudioPlayer

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
	Window::SetCursor(L"images/cursor.cur");
	Window window{ 960, 720, L"たいとる", 10, 10 };
	GraphicsXI::ClearColor(Color(0, 0, 60));
	GraphicsXI gfx{ window.getHandle(), window.getInstance() };
	Input input{ window.getHandle(), window.getInstance() };
	Audio audio{ L"music/Alexandrite.mp3" };
//	Audio audio{ L"music/Blue.mp3" };
//	Audio audio{ L"music/True-Peak.flac" };
//	Audio audio{ L"music/audio.ogg" };
//	Audio audio{ L"music/AIZO.opus" };

	window.show();
	input.Acquire();
	audio.Play();
	auto coverImage = Texture{ audio.MetaData().jacket };

	Rect rect{ 480, 360 };
	int r = 0;

	while (window.update())
	{
		input.Check();
		if (input.pressed(Key::UP))
		{
			r++;
		}
		else if (input.pressed(Key::DOWN))
		{
			r--;
		}

		gfx.clear();
		gfx.draw(coverImage.scaled(0.5f).get());
//		gfx.draw(rect.color(std::clamp(r, 0, 255), 0, 0).position(0.0f, 0.0f).get());
		gfx.present();
	}

	window.close();

	return 0;
}





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
#include<random>

using namespace UsingAudio;
using namespace UsingWindow;
using namespace UsingGraphics;
using namespace UsingDevice;

int main()
{
	std::random_device random;

	Window window{ 960, 720, L"たいとる☺" };
//	GraphicsXI::ClearColor(Color(0, 0, 60));
	GraphicsXI::ClearColor(Color(160));
	GraphicsXI graphics{ window.getHandle(), window.getInstance() };

	window.setIcon(L"images/icon.ico");
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
	Image text = Text::GetTextImage(moji);
	Texture texture1{ image };
	Texture texture2{ text };
	Texture texture3{ Text::GetTextImage(L"!?:;<>{}[]()'&%$#,.|~+-*/=") };
	texture3.scaled(3.0f);
	Texture texture4{ Text::GetTextImage(L"abcdefghijklmnopqrstuvwxyz") };
	texture4.scaled(3.0f);
	Texture texture5{ Text::GetTextImage(L"ABCDEFGHIJKLMNOPQRSTUVWXYZ") };
	texture5.scaled(3.0f);

	texture2.position(100.0f, 600.0f);

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

		graphics.draw(rect.position(x, y).get());
		graphics.draw(rect2.rotationZ(angle * 3).get());
		graphics.draw(circle.get());
		graphics.draw(texture2.scaled(5.0f).get());
		graphics.draw(texture3.position(10.0f, 0.0f).get());
		graphics.draw(texture4.position(10.0f, 21.0f).get());
		graphics.draw(texture5.position(10.0f, 52.0f).get());
		graphics.present();
	}
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

#ifdef AudioPlayer

using namespace UsingAudio;
using namespace UsingWindow;
using namespace UsingGraphics;

const std::wstring audiofile = L"music/Blue.mp3";
const int loopCount = 0;

int main()
{
	std::locale::global(std::locale("ja_JP.UTF-8"));
	std::wcout.imbue(std::locale());
	Window window{ 800, 600, L"Audio Player!" };
	GraphicsXI::ClearColor(Win32_CPP::Color(0));
	GraphicsXI gfx{ window.getHandle(), window.getInstance() };
	Audio audio{ audiofile, loopCount };
	AudioMetaData audioMetaData = audio.MetaData();
	window.show();
	audio.Play();

	Texture audioInfo[] =
	{
		Text::GetTextImage(L"けいしき   " + audio.FormatString()),
		Text::GetTextImage(L"あるばむ   " + audioMetaData.album),
		Text::GetTextImage(L"あーてぃすと " + audioMetaData.artist),
		Text::GetTextImage(L"こめんと   " + audioMetaData.comment),
		Text::GetTextImage(L"じゃんる   " + audioMetaData.genre),
		Text::GetTextImage(L"たいとる   " + audioMetaData.title),
		Text::GetTextImage(L"とらっく   " + std::to_wstring(audioMetaData.track)),
		Text::GetTextImage(L"いやー    " + std::to_wstring(audioMetaData.year))
	};
	for (auto& info : audioInfo)
	{
		info.scaled(5.0f).color(Color(255));
	}

	while (window.update())
	{
		gfx.clear();

		for (int i = 0; i < _countof(audioInfo); i++)
		{
			gfx.draw(
				audioInfo[i].position(10.0f, 10.0f + i * audioInfo[i].height() * 5.0f + i * 8.0f).get()
			);
		}

		gfx.present();
	}
}

#endif

#endif