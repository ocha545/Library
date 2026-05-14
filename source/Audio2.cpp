#include"../include/Audio2.h"

namespace Win32
{
	static double GetTotalLength(size_t size, unsigned long sampleRate, int channels)
	{
		if (sampleRate <= 0 || channels <= 0) return 0.0;

		double totalSamples = static_cast<double>(size) / static_cast<double>(channels);
		return totalSamples / static_cast<double>(sampleRate);
	}
	static void SampleToTime(double in, int& h, int& m, double& s)
	{
		if (in <= 0.0) in = 0.0;

		h = static_cast<int>(in / 3600);
		in = std::fmod(in, 3600);
		m = static_cast<int>(in / 60);
		in = std::fmod(in, 60);
		s = in;
	}
	static double TimeToSample(int h, int m, double s)
	{
		double tmp = 0.0;
		tmp += h * 3600;
		tmp += m * 60;
		tmp += s;
		return tmp;
	}
	static std::string ConvertTimeFormatA(double in)
	{
		int h = 0;
		int m = 0;
		double s = 0.0;
		SampleToTime(in, h, m, s);
		auto fmtInt = [](int digit) -> std::string {
			return (digit < 10) ? "0" + std::to_string(digit) : std::to_string(digit);
			};
		auto fmtDbl = [](double digit) -> std::string {
			return std::to_string(digit).substr(2, 2);
			};

		return (fmtInt(h) + ":" + fmtInt(m) + ":" + fmtInt((int)s) + "." + fmtDbl(s));
	}
	static std::wstring ConvertTimeFormatW(double in)
	{
		int h = 0;
		int m = 0;
		double s = 0.0;
		SampleToTime(in, h, m, s);
		auto fmtInt = [](int digit) -> std::wstring {
			return (digit < 10) ? L"0"+ std::to_wstring(digit) : std::to_wstring(digit);
		};
		auto fmtDbl = [](double digit) -> std::wstring {
			return (digit < 10) ? std::to_wstring(digit).substr(2, 2) : std::to_wstring(digit).substr(3, 2);
		};

		return (fmtInt(h) + L":" + fmtInt(m) + L":" + fmtInt((int)s) + L"." + fmtDbl(s));
	}
	static WAVEFORMATEX ConvertWaveFormat(const AudioInfo& af)
	{
		return {
			af.FormatTag,
			af.Channels,
			af.SampleRate,
			af.AvgBytesPerSec,
			af.BlockAlign,
			af.BitsPerSample,
			NULL
		};
	}

	//class AudioMaster begin
	AudioMaster::AudioMaster(const std::wstring& path)
	{
		switch (checkFormat(path))
		{
		case AudioFormat::WAVE:
			{
				WAVE data{ path, true };
				this->pcm = data.getPCM();
				this->format = AudioFormat::WAVE;
				this->info = data.getInformation();
				this->meta = data.getMetaData();
			}
			break;
		case AudioFormat::MP3:
			{
				MP3 data{ path, true };
				this->pcm = data.getPCM();
				this->format = AudioFormat::MP3;
				this->info = data.getInformation();
				this->meta = data.getMetaData();
			}
			break;
		case AudioFormat::FLAC:
			{
				FLAC data{ path, true };
				this->pcm = data.getPCM();
				this->format = AudioFormat::FLAC;
				this->info = data.getInformation();
				this->meta = data.getMetaData();
			}
			break;
		case AudioFormat::OGG:
			{
				OGG data{ path, true };
				this->pcm = data.getPCM();
				this->format = AudioFormat::OGG;
				this->info = data.getInformation();
				this->meta = data.getMetaData();
			}
			break;
		case AudioFormat::OPUS:
			{
				OPUS data{ path, true };
				this->pcm = data.getPCM();
				this->format = AudioFormat::OPUS;
				this->info = data.getInformation();
				this->meta = data.getMetaData();
			}
			break;
		}
	}
	AudioMaster::~AudioMaster()
	{
		internalDestroy();
		pcm.clear();
	}


	void AudioMaster::internalDestroy()
	{
		if (sourceVoice)
		{
			sourceVoice->Stop();
			sourceVoice->DestroyVoice();
		}
	}

	AudioFormat AudioMaster::checkFormat(const std::wstring& path)
	{
		if (WAVE{ path, false })
		{
			return AudioFormat::WAVE;
		}
		else if (MP3{ path, false })
		{
			return AudioFormat::MP3;
		}
		else if (FLAC{ path, false })
		{
			return AudioFormat::FLAC;
		}
		else if (OGG{ path, false })
		{
			return AudioFormat::OGG;
		}
		else if (OPUS{ path, false })
		{
			return AudioFormat::OPUS;
		}
		else
		{
			return AudioFormat::NONE;
		}
	}

	AudioInfo AudioMaster::getInfo() const
	{
		return info;
	}
	AudioFormat AudioMaster::getFormat() const
	{
		return format;
	}
	std::wstring AudioMaster::getFormatString() const
	{
		return audioFormatString[(size_t)format];
	}
	AudioMetaData2 AudioMaster::getMetaData() const
	{
		return meta;
	}
	const std::vector<short>& AudioMaster::getData() const
	{
		return pcm;
	}

	//class AudioMaster end

	
	//class AudioMonitor begin
	AudioMonitor::AudioMonitor()
	{
		HRESULT hres = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
		if (FAILED(hres))	throw std::runtime_error("COMオブジェクトの初期化に失敗しました");
		coInitialized = true;

		hres = XAudio2Create(xaudio.GetAddressOf());
		if (FAILED(hres))
		{
			CoUninitialize();
			coInitialized = false;
			throw std::runtime_error("XAudio2の作成に失敗しました");
		}

		hres = xaudio->CreateMasteringVoice(&masterVoice);
		if (FAILED(hres))
		{
			xaudio.Reset();
			CoUninitialize();
			coInitialized = false;
			throw std::runtime_error("XAudio2 MasterVoiceの作成に失敗しました");
		}
	}
	AudioMonitor::AudioMonitor(const AudioDeviceInfo& defaultDevice)
	{
		HRESULT hres = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
		if (FAILED(hres))	throw std::runtime_error("COMオブジェクトの初期化に失敗しました");
		coInitialized = true;

		hres = XAudio2Create(xaudio.GetAddressOf());
		if (FAILED(hres))
		{
			CoUninitialize();
			coInitialized = false;
			throw std::runtime_error("XAudio2の作成に失敗しました");
		}

		hres = xaudio->CreateMasteringVoice(&masterVoice,
			XAUDIO2_DEFAULT_CHANNELS,
			XAUDIO2_DEFAULT_SAMPLERATE,
			0,
			defaultDevice.deviceId.c_str()
		);
		if (FAILED(hres))
		{
			xaudio.Reset();
			CoUninitialize();
			coInitialized = false;
			throw std::runtime_error("XAudio2 MasterVoiceの作成に失敗しました");
		}
	}


	AudioMonitor::~AudioMonitor()
	{
		internalDestroy();
	}
	void AudioMonitor::internalDestroy()
	{
		try
		{
			if (masterVoice)
			{
				masterVoice->DestroyVoice();
				masterVoice = nullptr;
			}
			xaudio.Reset();

			if (coInitialized)
			{
				CoUninitialize();
				coInitialized = false;
			}
		}
		catch (std::exception e)
		{
		}
	}

	std::vector<AudioDeviceInfo> AudioMonitor::enumerateDevices()
	{
		std::vector<AudioDeviceInfo> devices;

		ComPtr<IMMDeviceEnumerator> enumerator = nullptr;
		{
			HRESULT hres = CoCreateInstance(__uuidof(MMDeviceEnumerator),
				nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)enumerator.GetAddressOf()
			);
			if (FAILED(hres))
			{
				throw std::runtime_error("IMMDeviceEnumeratorの作成に失敗しました");
			}
		}
		ComPtr<IMMDeviceCollection> collection = nullptr;
		{
			HRESULT hres = enumerator->EnumAudioEndpoints(
				eRender,
				DEVICE_STATE_ACTIVE,
				collection.GetAddressOf()
			);
			if (FAILED(hres))
			{
				throw std::runtime_error("デバイス一覧を取得できませんでした");
			}
		}
		UINT deviceCount = 0;
		{
			HRESULT hres = collection->GetCount(&deviceCount);
			if (FAILED(hres))
			{
				throw std::runtime_error("デバイスの数を取得できませんでした");
			}
			devices.clear();
		}

		for (UINT i = 0; i < deviceCount; ++i)
		{
			ComPtr<IMMDevice> device = nullptr;
			{
				HRESULT hres = collection->Item(i, device.GetAddressOf());
				if (FAILED(hres))
				{
					continue;
				}
			}

			std::wstring deviceId;
			{
				LPWSTR tempId = nullptr;
				HRESULT hres = device->GetId(&tempId);
				if (SUCCEEDED(hres))
				{
					deviceId = std::wstring(tempId);
					CoTaskMemFree(tempId);
				}
				else
				{
					continue;
				}
			}

			ComPtr<IPropertyStore> store = nullptr;
			{
				HRESULT hres = device->OpenPropertyStore(STGM_READ, store.GetAddressOf());
				if (FAILED(hres))
				{
					continue;
				}
			}

			std::wstring deviceName = L"Unknown Device";
			PROPVARIANT variant;
			{
				PropVariantInit(&variant);//この関数要る？
				HRESULT hres = store->GetValue(PKEY_Device_FriendlyName, &variant);

				if (SUCCEEDED(hres) && variant.pwszVal != nullptr)
				{
					deviceName = variant.pwszVal;
				}
				PropVariantClear(&variant);
			}
			devices.emplace_back(AudioDeviceInfo{ deviceId, deviceName });
		}
		devices.shrink_to_fit();

		return devices;
	}

	bool AudioMonitor::changeDevice(const AudioDeviceInfo& device, const AudioMaster& amm)
	{
		if (device.deviceId.empty())
		{
			return false;
		}

		try
		{
			AudioMonitor monitor{ device };
			swap(monitor);
			init(amm);
			return true;
		}
		catch (std::exception)
		{
			return false;
		}
	}

	bool AudioMonitor::init(const AudioMaster& amm, int loopCount)
	{
		WAVEFORMATEX format = ConvertWaveFormat(amm.getInfo());
		HRESULT hres = xaudio->CreateSourceVoice((IXAudio2SourceVoice**)&amm.sourceVoice, &format, XAUDIO2_VOICE_USEFILTER);
		if (FAILED(hres))
		{
			internalDestroy();
			return false; //ソースボイスの作成に失敗しました
		}
		XAUDIO2_BUFFER xAudio2Buffer{};

		const uint64_t audioByteCount = static_cast<uint64_t>(amm.getData().size()) * sizeof(int16_t);
		if (audioByteCount > (std::numeric_limits<uint64_t>::max)())
		{
			return false;//音声データが大きすぎてXAudio2に送れません
		}

		xAudio2Buffer.pAudioData = reinterpret_cast<const BYTE*>(amm.getData().data());
		xAudio2Buffer.Flags = XAUDIO2_END_OF_STREAM;
		xAudio2Buffer.AudioBytes = static_cast<UINT32>(audioByteCount);
		xAudio2Buffer.LoopCount = loopCount;

		hres = amm.sourceVoice->SubmitSourceBuffer(&xAudio2Buffer);
		if (FAILED(hres))
		{
			amm.sourceVoice->DestroyVoice();
			internalDestroy();
			return false;
		}
		return true;
	}


	bool AudioMonitor::isPlaying(const AudioMaster& amm)
	{
		XAUDIO2_VOICE_STATE state{};
		if(amm.sourceVoice)
		{
			amm.sourceVoice->GetState(&state);
		}
		return (state.BuffersQueued);
//		return true;
	}

	bool AudioMonitor::seek(const AudioMaster& amm, float pos) const
	{
		if (!amm.sourceVoice)
		{
			return false;
		}

		const AudioInfo& info = amm.getInfo();
		if (info.SampleRate == 0 || info.BlockAlign == 0)
		{
			return false;
		}

		const size_t totalBytes = static_cast<size_t>(amm.getData().size()) * sizeof(short);
		if (totalBytes == 0)
		{
			return false;
		}
		if (pos < 0.0f)
		{
			pos = 0.0f;
		}

		size_t byteOffset = static_cast<size_t>(pos * static_cast<float>(info.AvgBytesPerSec));
		if (byteOffset >= totalBytes)
		{
			return false;
		}

		const size_t remainBytes = totalBytes - byteOffset;
		if (remainBytes > (std::numeric_limits<uint64_t>::max)())
		{
			return false;
		}

		XAUDIO2_BUFFER xAudio2Buffer{};
		xAudio2Buffer.pAudioData = reinterpret_cast<const BYTE*>(amm.getData().data()) + byteOffset;
		xAudio2Buffer.Flags = XAUDIO2_END_OF_STREAM;
		xAudio2Buffer.AudioBytes = static_cast<UINT32>(remainBytes);
		xAudio2Buffer.LoopCount = 0;


		amm.sourceVoice->Stop();
		amm.sourceVoice->FlushSourceBuffers();
		HRESULT hres = amm.sourceVoice->SubmitSourceBuffer(&xAudio2Buffer);
		if (FAILED(hres))
		{
			return false;
		}

//		hres = amm.sourceVoice->Start();
//		return SUCCEEDED(hres);
		return true;
	}

	void AudioMonitor::swap(AudioMonitor& monitor)
	{
		std::swap<IXAudio2MasteringVoice*>(masterVoice, monitor.masterVoice);
		std::swap<ComPtr<IXAudio2>>(xaudio, monitor.xaudio);
		std::swap<bool>(coInitialized, monitor.coInitialized);
	}

	void AudioMonitor::volume(const AudioMaster& amm, float volume) const
	{
		if (amm.sourceVoice)
		{
			amm.sourceVoice->SetVolume(volume);
		}
	}

	void AudioMonitor::restart(const AudioMaster& amm) const
	{
		if (amm.sourceVoice)
		{
			seek(amm, 0.0f);
		}
		play(amm);
	}

	void AudioMonitor::reset(const AudioMaster& amm) const
	{
		if (amm.sourceVoice)
		{
			seek(amm, 0.0f);
		}
	}

	void AudioMonitor::play(const AudioMaster& amm) const
	{
		if (amm.sourceVoice)
		{
			amm.sourceVoice->Start();
		}
	}

	void AudioMonitor::stop(const AudioMaster& amm) const
	{
		if (amm.sourceVoice)
		{
			amm.sourceVoice->Stop();
		}
	}

	std::wstring AudioMonitor::length(const AudioMaster& amm) const
	{
		return ConvertTimeFormatW(
			GetTotalLength(
				amm.getData().size(),
				amm.getInfo().SampleRate,
				amm.getInfo().Channels
			)
		);
	}
	std::wstring AudioMonitor::time(const AudioMaster& amm) const
	{
		if (!amm.sourceVoice)
		{
			return L"00:00:00.00";
		}
		XAUDIO2_VOICE_STATE state{};
		amm.sourceVoice->GetState(&state);
		
		return ConvertTimeFormatW(
			GetTotalLength(
				state.SamplesPlayed,
				amm.getInfo().SampleRate,
				1
			)
		);
	}
	size_t AudioMonitor::sample(const AudioMaster& amm) const
	{
		if (!amm.sourceVoice)
		{
			return 0l;
		}

		XAUDIO2_VOICE_STATE state{};
		amm.sourceVoice->GetState(&state);

		return state.SamplesPlayed;
	}
}