#pragma once
#include"Utility.h"
#include"third_party/taglib/tag.h"
#include"third_party/taglib/fileref.h"
#include"third_party/taglib/mpeg/mpegfile.h"
#include"third_party/taglib/toolkit/tlist.h"
#include<windows.h>
#include<vector>
#include<memory>
#include<string>
#include<fstream>
namespace Win32
{
	enum class AudioFormat
	{
		NONE = NULL,
		WAVE = 1,
		MP3 = 2,
		FLAC = 3,
		OGG = 4,
		OPUS = 5,
		MAX,
	};

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

	class AudioMetaData2
	{
	private:
		static const wchar_t SEPARATOR = L'\u001F';
		std::wstring allTag;
		unsigned int t = 0;
		unsigned int y = 0;
		Image j{};

	public:
		AudioMetaData2()
			: allTag(L"") {
		};
		AudioMetaData2(const TagLib::Tag* tag, bool disableImage = false);
		std::wstring title() const;
		std::wstring artist() const;
		std::wstring album() const;
		std::wstring genre() const;
		std::wstring comment() const;
		unsigned int track() const;
		unsigned int year() const;
		Image jacket() const;
	};
}

