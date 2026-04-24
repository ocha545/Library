#include"../include/Audio.h"
#include"../include/AudioResource.h"
using namespace Win32;

int main()
{
	std::locale::global(std::locale("ja_JP.UTF-8"));
	std::wcout.imbue(std::locale());

	MP3 mp3{ L"music/Blue.mp3", true };
	Audio audio{ mp3.getPCM(), mp3.getInformation() };

	std::vector<AudioDeviceInfo> devices;
	HRESULT hr = audio.EnumerateDevicesInternal(devices);
	bool continueApp = true;
	if (FAILED(hr))
	{
		continueApp = false;
	}

	for (const auto& device : devices)
	{
		std::wcout << L"Device Id:  " << device.deviceId << "\n";
		std::wcout << L"Device Name:" << device.deviceName << "\n\n";
	}

	audio.Play();

	while (audio.State().BuffersQueued && continueApp)
	{
	}
}