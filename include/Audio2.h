#pragma once
#include"AudioResource.h"
#include"AudioFormat.h"
#include<string>
#include<wrl.h>
#include<xaudio2.h>
#include<xaudio2fx.h>
#include<xapo.h>
#include<xapobase.h>
#include<xapofx.h>
#include<mmdeviceapi.h>
#include<endpointvolume.h>
#include<functiondiscoverykeys.h>

namespace Win32
{
	using std::wstring_view;
	using Microsoft::WRL::ComPtr;

	static  std::wstring audioFormatString[(size_t)AudioFormat::MAX]{
		L"None",
		L"Wave",
		L"MP3",
		L"Flac",
		L"OGG",
		L"Opus",
	};

	static double GetTotalLength(size_t size, unsigned long sampleRate, int channels);
	static void SampleToTime(double in, int& h, int& m, double& s);
	static double TimeToSample(int h, int m, double s);
	static std::string ConvertTimeFormatA(double in);
	static std::wstring ConvertTimeFormatW(double in);
	static AudioFormat CheckFormat(const std::wstring& path);
	static WAVEFORMATEX ConvertWaveFormat(const AudioInfo& af);

	class AudioMaster
	{
	private:
//		XAUDIO2_VOICE_STATE state{};
		AudioInfo info;
		AudioFormat format;
		AudioMetaData2 meta;
		std::vector<short> pcm;

		AudioFormat checkFormat(const std::wstring& path);
	public:
		AudioMaster() : info({}), format(AudioFormat::NONE), meta({}), pcm({}) {
		}
		AudioMaster(const std::wstring& path);
		~AudioMaster();

		AudioInfo getInfo() const;
		AudioFormat getFormat() const;
		std::wstring getFormatString() const;
		AudioMetaData2 getMetaData() const;
		const std::vector<short>& getData() const;
		void internalDestroy();

		IXAudio2SourceVoice* sourceVoice = nullptr;
	};

	class AudioMonitor
	{
	private:
		ComPtr<IXAudio2> xaudio = nullptr;
		IXAudio2MasteringVoice* masterVoice = nullptr;
		bool coInitialized = false;
		void internalDestroy();

	public:
		AudioMonitor();
		AudioMonitor(const AudioDeviceInfo& defaultDevice);
		~AudioMonitor();

		std::vector<AudioDeviceInfo> enumerateDevices();
		bool changeDevice(const AudioDeviceInfo& device, const AudioMaster& amm);
		bool init(const AudioMaster& amm, int loopCount = 0);
		bool isPlaying(const AudioMaster& amm);
		bool seek(const AudioMaster& amm, float pos) const;
		void swap(AudioMonitor& monitor);
		void volume(const AudioMaster& amm, float volume) const;
		void restart(const AudioMaster& amm) const;
		void play(const AudioMaster& amm) const;
		void stop(const AudioMaster& amm) const;
		std::wstring length(const AudioMaster& amm) const;
		std::wstring time(const AudioMaster& amm) const;
		std::wstring fineTime(const AudioMaster& amm) const;
	};
}