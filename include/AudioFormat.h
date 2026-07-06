#pragma once
#include"AudioResource.h"
#include"third_party/taglib/tag.h"
#include"third_party/taglib/fileref.h"
#include"third_party/taglib/mpeg/mpegfile.h"
#include"third_party/taglib/toolkit/tlist.h"
#include"third_party/dr_wav.h"
#include"third_party/dr_mp3.h"
#include"third_party/dr_flac.h"
#include"third_party/vorbis/vorbisfile.h"
#include"third_party/opusfile/opusfile.h"

namespace Win32
{
	class MP3
	{
	private:
		std::unique_ptr<drmp3> mp3;
		std::vector<short> pcm;
		AudioInfo info{};
		AudioMetaData2 meta{};
		bool isRead;

	public:
		MP3(const std::wstring& path, bool extract);
		~MP3();
		operator bool();
		constexpr AudioFormat getFormat() const { return AudioFormat::MP3; }
		const std::vector<short> getPCM() const;
		const AudioInfo getInformation() const;
		const AudioMetaData2 getMetaData() const;
	};

	class WAVE
	{
	private:
		std::unique_ptr<drwav> wav;
		std::vector<short> pcm;
		AudioInfo info{};
		AudioMetaData2 meta{};
		bool isRead;

	public:
		WAVE(const std::wstring& path, bool extract);
		~WAVE();
		operator bool();
		constexpr AudioFormat getFormat() const { return AudioFormat::WAVE; }
		const std::vector<short> getPCM() const;
		const AudioInfo getInformation() const;
		const AudioMetaData2 getMetaData() const;
	};

	class FLAC
	{
	private:
		drflac* flac = nullptr;
		std::vector<short> pcm;
		AudioInfo info{};
		AudioMetaData2 meta{};
		bool isRead;

	public:
		FLAC(const std::wstring& path, bool extract);
		~FLAC();
		operator bool();
		constexpr AudioFormat getFormat() const { return AudioFormat::FLAC; }
		const std::vector<short> getPCM() const;
		const AudioInfo getInformation() const;
		const AudioMetaData2 getMetaData() const;
	};

	class OGG
	{
	private:
		OggVorbis_File ogg;
		std::vector<short> pcm;
		AudioInfo info{};
		AudioMetaData2 meta{};
		bool isRead;

	public:
		OGG(const std::wstring& path, bool extract);
		~OGG();
		operator bool();
		constexpr AudioFormat getFormat() const { return AudioFormat::OGG; }
		const std::vector<short> getPCM() const;
		const AudioInfo getInformation() const;
		const AudioMetaData2 getMetaData() const;
	};

	class OPUS
	{
	private:
		bool isRead;
		std::vector<short> pcm;
		AudioInfo info{};
		AudioMetaData2 meta{};
		OggOpusFile* opus;

	public:
		OPUS(const std::wstring& path, bool extract);
		~OPUS();
		operator bool();
		constexpr AudioFormat getFormat() const { return AudioFormat::OPUS; }
		const std::vector<short> getPCM() const;
		const AudioInfo getInformation() const;
		const AudioMetaData2 getMetaData() const;
	};
}
