#pragma once
#include<windows.h>
#include<vector>
#include<memory>
#include<string>
#include<fstream>
#include"Utility.h"

namespace Win32
{
	struct AudioDeviceInfo
	{
		std::wstring deviceId;
		std::wstring deviceName;

		AudioDeviceInfo() = default;
		AudioDeviceInfo(const std::wstring& id, const std::wstring& name) :
			deviceId(id), deviceName(name) {
		}
	};

	struct AudioMetaData
	{
		std::wstring title = L"";
		std::wstring artist = L"";
		std::wstring album = L"";
		std::wstring genre = L"";
		std::wstring comment = L"";
		unsigned int track = 0;
		unsigned int year = 0;
		Image jacket{};
	};

	struct AudioInfo
	{
		unsigned short FormatTag = NULL;
		unsigned short Channels = NULL;
		unsigned long SampleRate = NULL;
		unsigned long AvgBytesPerSec = NULL;
		unsigned short BlockAlign = NULL;
		unsigned short BitsPerSample = NULL;
	};
}

