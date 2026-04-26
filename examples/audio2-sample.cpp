#include"../include/Audio2.h"
using namespace Win32;

int main()
{
	std::locale::global(std::locale("ja_JP.UTF-8"));
	std::wcout.imbue(std::locale());
	AudioMonitor adm{};
	// シャイニングスター : 魔王魂(MaouDamashii)
	// タグ情報の改変をしています。
//	AudioMaster data{ L"music/maou_14_shining_star.mp3" };
	AudioMaster data{ L"music/melissa.mp3" };
	std::vector<AudioDeviceInfo> devices = adm.enumerateDevices();
	constexpr wchar_t lbrk = L'\n';

	bool continueApp = adm.init(data);

	if (continueApp)
	{
		std::wcout << L"--- information ---" << lbrk;
		std::wcout << L"Channels:   " << data.getInfo().Channels << lbrk;
		std::wcout << L"SampleRate: " << data.getInfo().SampleRate << lbrk;
		std::wcout << L"Title:   " << data.getMetaData().title() << lbrk;
		std::wcout << L"Album:   " << data.getMetaData().album() << lbrk;
		std::wcout << L"Artist:  " << data.getMetaData().artist() << lbrk;
		std::wcout << L"Comment: " << data.getMetaData().comment() << lbrk;
		std::wcout << L"Genre:   " << data.getMetaData().genre() << lbrk;
		std::wcout << L"Track:   " << data.getMetaData().track() << lbrk;
		std::wcout << L"Year:    " << data.getMetaData().year() << lbrk;
		std::wcout << L"Length:  " << adm.length(data) << lbrk << lbrk;
		std::wcout << L"--- Audio Device Information --- " << lbrk;
		for(int i = 0; i < devices.size(); i++)
		{
			std::wcout << i << L") Device Name: " << devices[i].deviceName << lbrk;
		}
		std::wstring selectDevice;
		std::wcout << L"Select Device Number: ";
		std::wcin >> selectDevice;
		adm.changeDevice(devices[std::stoi(selectDevice)], data);
		adm.volume(data, 0.01f);
		adm.play(data);
	}
	while (adm.isPlaying(data) && continueApp)
	{
		std::wcout << adm.time(data) << lbrk;
	}

	std::wcout << L"End" << lbrk;
	std::wstring tmp;
	std::wcin >> tmp;
}