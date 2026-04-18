#include"../include/Audio.h"
#include"../include/audio_impl.h"
#include<fstream>

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
			return (digit < 10) ? std::format("0{}", digit) : std::format("{}", digit);
			};
		const std::string& fmtDbl = (s < 10.0) ? std::format("0{:.0f}", s) : std::format("{:.0f}", s);
		return std::format("{}:{}:{}", fmtInt(h), fmtInt(m), fmtDbl);
	}
	static std::wstring ConvertTimeFormatW(double in)
	{
		int h = 0;
		int m = 0;
		double s = 0.0;
		SampleToTime(in, h, m, s);
		auto fmtInt = [](int digit) -> std::wstring {
			return (digit < 10) ? std::format(L"0{}", digit) : std::format(L"{}", digit);
			};
		const std::wstring& fmtDbl = (s < 10.0) ? std::format(L"0{:.0f}", s) : std::format(L"{:.0f}", s);
		return std::format(L"{}:{}:{}", fmtInt(h), fmtInt(m), fmtDbl);
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
	static AudioFormat CheckFormat(const std::wstring& path)
	{
		//Wave
		{
			WAVE wav{ path, false };
			if (wav)
			{
				return AudioFormat::WAVE;
			}
		}
		//MP3
		{
			MP3 mp3{ path, false };
			if (mp3)
			{
				return AudioFormat::MP3;
			}
		}
		//Flac
		{
			FLAC flac{ path, false };
			if (flac)
			{
				return AudioFormat::FLAC;
			}
		}
		//OGG
		{
			OGG ogg{ path, false };
			if (ogg)
			{
				return AudioFormat::OGG;
			}
		}
		//Opus
		{
			OPUS opus{ path, false };
			if (opus)
			{
				return AudioFormat::OPUS;
			}
		}

		return AudioFormat::NONE;
	}

	



	//class Audio begin
	Audio::Audio(const std::wstring& path, UINT32 loopCount)
	{
		if (loopCount > INFINITE_LOOP)
		{
			throw std::invalid_argument("ループ回数が255以上になりました。無限ループにする場合、INFINITE_LOOPを指定してください");
		}
		if (!std::filesystem::exists(path))
		{
			throw std::runtime_error("ファイルが見つかりませんでした");
		}

		std::vector<short> localAudio;
		AudioInfo localInfo{};
		AudioFormat localFormat = CheckFormat(path);
		AudioMetaData localMetaData{};

		if (localFormat == AudioFormat::WAVE)
		{
			WAVE wav{ path, true };
			localInfo = wav.getInformation();
			localAudio = wav.getPCM();
		}
		else if (localFormat == AudioFormat::MP3)
		{
			MP3 mp3{ path, true };
			localInfo = mp3.getInformation();
			localAudio = mp3.getPCM();
			localMetaData = mp3.getMetaData();
		}
		else if (localFormat == AudioFormat::FLAC)
		{
			FLAC flac{ path, true };
			localInfo = flac.getInformation();
			localAudio = flac.getPCM();
		}
		else if (localFormat == AudioFormat::OGG)
		{
			OGG ogg{ path, true };
			localInfo = ogg.getInformation();
			localAudio = ogg.getPCM();
		}
		else if (localFormat == AudioFormat::OPUS)
		{
			OPUS opus{ path, true };
			localInfo = opus.getInformation();
			localAudio = opus.getPCM();
		}
		else
		{
			throw std::runtime_error("現在サポートされていないフォーマットです");
		}

		const uint64_t audioByteCount = static_cast<uint64_t>(localAudio.size()) * sizeof(int16_t);
		if (audioByteCount > (std::numeric_limits<uint64_t>::max)())
		{
			throw std::runtime_error("音声データが大きすぎてXAudio2に送れません");
		}

		Hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
		if (SUCCEEDED(Hr))
		{
			this->coInitialized = true;
		}
		else
		{
			throw std::runtime_error("COMオブジェクトの初期化に失敗しました");
		}

		Hr = XAudio2Create(XAudio2.GetAddressOf());
		if (FAILED(Hr))
		{
			CoUninitialize();
			this->coInitialized = false;
			throw std::runtime_error("XAudio2の作成に失敗しました");
		}

		Hr = XAudio2->CreateMasteringVoice(&MasterVoice);
		if (FAILED(Hr))
		{
			XAudio2.Reset();
			CoUninitialize();
			this->coInitialized = false;
			throw std::runtime_error("マスターボイスの作成に失敗しました");
		}

		WAVEFORMATEX format = ConvertWaveFormat(localInfo);
		Hr = XAudio2->CreateSourceVoice(&SourceVoice, &format, XAUDIO2_VOICE_USEFILTER);
		if (FAILED(Hr))
		{
			XAudio2.Reset();
			CoUninitialize();
			this->coInitialized = false;
			throw std::runtime_error("ソースボイスの作成に失敗しました");
		}

		std::cout << "Audio Bytes : " << audioByteCount << "\n";
		std::cout << "Limit?	  : " << (audioByteCount >= 0x7fffffff);
		std::cout << "pcmData.size() % channels = " << (localAudio.size() % format.nChannels) << std::endl;


		XAUDIO2_BUFFER xAudio2Buffer{};
//		ZeroMemory(&xAudio2Buffer, sizeof(XAUDIO2_BUFFER));

		xAudio2Buffer.pAudioData = reinterpret_cast<const BYTE*>(localAudio.data());
		xAudio2Buffer.Flags = XAUDIO2_END_OF_STREAM;
		xAudio2Buffer.AudioBytes = static_cast<UINT32>(audioByteCount);
		xAudio2Buffer.LoopCount = loopCount;

		Hr = SourceVoice->SubmitSourceBuffer(&xAudio2Buffer);
		if (FAILED(Hr))
		{
			SourceVoice->DestroyVoice();
			XAudio2.Reset();
			CoUninitialize();
			this->coInitialized = false;
			throw std::runtime_error(std::format("SourceBufferの送信に失敗しました:{}", Hr));
		}

		this->audio.swap(localAudio);
		this->audioInfo = localInfo;
		this->audioFormat = localFormat;
		this->audioMetaData = localMetaData;
	}
	Audio::Audio(const std::vector<short>& pcmData, const AudioInfo& info, UINT32 loopCount)
	{
		if (loopCount > INFINITE_LOOP)
		{
			throw std::invalid_argument("ループ回数が255以上になりました。無限ループにする場合、INFINITE_LOOPを指定してください");
		}

		std::vector<short> localAudio = pcmData;
		AudioInfo localInfo = info;
		AudioFormat localFormat = AudioFormat::NONE;
		AudioMetaData localMetaData{};

		const uint64_t audioByteCount = static_cast<uint64_t>(localAudio.size()) * sizeof(int16_t);
		if (audioByteCount > (std::numeric_limits<uint64_t>::max)())
		{
			throw std::runtime_error("音声データが大きすぎてXAudio2に送れません");
		}

		Hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
		if (SUCCEEDED(Hr))
		{
			this->coInitialized = true;
		}
		else
		{
			throw std::runtime_error("COMオブジェクトの初期化に失敗しました");
		}

		Hr = XAudio2Create(XAudio2.GetAddressOf());
		if (FAILED(Hr))
		{
			CoUninitialize();
			this->coInitialized = false;
			throw std::runtime_error("XAudio2の作成に失敗しました");
		}

		Hr = XAudio2->CreateMasteringVoice(&MasterVoice);
		if (FAILED(Hr))
		{
			XAudio2.Reset();
			CoUninitialize();
			this->coInitialized = false;
			throw std::runtime_error("マスターボイスの作成に失敗しました");
		}

		WAVEFORMATEX format = ConvertWaveFormat(localInfo);
		Hr = XAudio2->CreateSourceVoice(&SourceVoice, &format, XAUDIO2_VOICE_USEFILTER);
		if (FAILED(Hr))
		{
			XAudio2.Reset();
			CoUninitialize();
			this->coInitialized = false;
			throw std::runtime_error("ソースボイスの作成に失敗しました");
		}

		std::cout << "Audio Bytes : " << audioByteCount << "\n";
		std::cout << "Limit?	  : " << (audioByteCount >= 0x7fffffff);
		std::cout << "pcmData.size() % channels = " << (localAudio.size() % format.nChannels) << std::endl;


		XAUDIO2_BUFFER xAudio2Buffer{};

		xAudio2Buffer.pAudioData = reinterpret_cast<const BYTE*>(localAudio.data());
		xAudio2Buffer.Flags = XAUDIO2_END_OF_STREAM;
		xAudio2Buffer.AudioBytes = static_cast<UINT32>(audioByteCount);
		xAudio2Buffer.LoopCount = loopCount;

		Hr = SourceVoice->SubmitSourceBuffer(&xAudio2Buffer);
		if (FAILED(Hr))
		{
			SourceVoice->DestroyVoice();
			XAudio2.Reset();
			CoUninitialize();
			this->coInitialized = false;
			throw std::runtime_error(std::format("SourceBufferの送信に失敗しました:{}", Hr));
		}

		this->audio.swap(localAudio);
		this->audioInfo = localInfo;
		this->audioFormat = localFormat;
		this->audioMetaData = localMetaData;
	}
	Audio::~Audio()
	{
		try
		{
			if (SourceVoice)
			{
				SourceVoice->Stop();
				SourceVoice->FlushSourceBuffers();
				SourceVoice->DestroyVoice();
			}
			if (MasterVoice)
			{
				MasterVoice->DestroyVoice();
			}

			XAudio2.Reset();
			audio.clear();

			if (this->coInitialized)
			{
				CoUninitialize();
				this->coInitialized = false;
			}
		}
		catch (std::exception)
		{
		}
	}

	HRESULT Audio::EnumerateDevicesInternal(std::vector<AudioDeviceInfo>& devices)
	{
		ComPtr<IMMDeviceEnumerator> immEnum;
		HRESULT hr = CoCreateInstance(
			__uuidof(MMDeviceEnumerator),
			nullptr,
			CLSCTX_ALL,
			__uuidof(IMMDeviceEnumerator),
			reinterpret_cast<void**>(immEnum.GetAddressOf())
		);
		if (FAILED(Hr)) return hr;

		ComPtr<IMMDeviceCollection> immCollection;
		hr = immEnum->EnumAudioEndpoints(
			eRender,
			DEVICE_STATE_ACTIVE,
			immCollection.GetAddressOf()
		);
		if (FAILED(Hr)) return hr;

		UINT deviceCount = 0;
		hr = immCollection->GetCount(&deviceCount);
		if (FAILED(Hr)) return hr;

		devices.clear();

		for (UINT i = 0; i < deviceCount; i++)
		{
			ComPtr<IMMDevice> immDevice;
			hr = immCollection->Item(i, immDevice.GetAddressOf());
			if (FAILED(Hr)) continue;

			LPWSTR lpwId = nullptr;
			hr = immDevice->GetId(&lpwId);
			if (FAILED(Hr)) continue;

			std::wstring deviceId{ lpwId };
			CoTaskMemFree(lpwId);

			ComPtr<IPropertyStore> ipStore;
			hr = immDevice->OpenPropertyStore(STGM_READ, ipStore.GetAddressOf());
			if (FAILED(Hr)) continue;

			PROPVARIANT varName;
			PropVariantInit(&varName);
			hr = ipStore->GetValue(PKEY_Device_FriendlyName, &varName);

			std::wstring deviceName = L"Unknown Device";
			if (SUCCEEDED(Hr) && varName.pwszVal != nullptr)
			{
				deviceName = varName.pwszVal;
			}
			PropVariantClear(&varName);

			devices.emplace_back(deviceId, deviceName);
		}

		return S_OK;
	}

	void Audio::Stop()
	{
		if (SourceVoice)
		{
			SourceVoice->Stop();
		}
	}

	void Audio::Play()
	{
		if (SourceVoice)
		{
			SourceVoice->Start();
		}
	}

	void Audio::EnableReverb()
	{
		if (!SourceVoice) return;
		ComPtr<IUnknown> pXAPO;
//		Hr = XAudio2CreateReverb(pXAPO.GetAddressOf());
		Hr = CreateFX(__uuidof(FXReverb), pXAPO.GetAddressOf());
		if (FAILED(Hr) || !pXAPO) return;
		XAUDIO2_EFFECT_DESCRIPTOR effectDesc{};
		effectDesc.InitialState = true;
		effectDesc.OutputChannels = audioInfo.Channels;
		effectDesc.pEffect = pXAPO.Get();
		XAUDIO2_EFFECT_CHAIN effectChain{};
		effectChain.EffectCount = 1;
		effectChain.pEffectDescriptors = &effectDesc;
		SourceVoice->SetEffectChain(&effectChain);

		FXREVERB_PARAMETERS params{};
		params.Diffusion = FXREVERB_MIN_DIFFUSION;
		params.RoomSize =  FXREVERB_MIN_ROOMSIZE;

		SourceVoice->SetEffectParameters(0, &params, sizeof(FXREVERB_PARAMETERS));
		SourceVoice->EnableEffect(0);

	}

	void Audio::DisableReverb()
	{
		if (SourceVoice)
		{
			SourceVoice->DisableEffect(0);
		}
	}

	void Audio::EnableEcho()
	{
		if (!SourceVoice) return;
		ComPtr<IUnknown> pXAPO;
		Hr = CreateFX(__uuidof(FXEcho), pXAPO.GetAddressOf());
//		Hr = XAudio2CreateReverb(pXAPO.GetAddressOf());
		if (FAILED(Hr) || !pXAPO) return;
		XAUDIO2_EFFECT_DESCRIPTOR effectDesc{};
		effectDesc.InitialState = true;
		effectDesc.OutputChannels = audioInfo.Channels;
		effectDesc.pEffect = pXAPO.Get();
		XAUDIO2_EFFECT_CHAIN effectChain{};
		effectChain.EffectCount = 1;
		effectChain.pEffectDescriptors = &effectDesc;
		SourceVoice->SetEffectChain(&effectChain);

		FXECHO_PARAMETERS params{};
		params.Delay = FXECHO_DEFAULT_DELAY;
		params.Feedback = FXECHO_DEFAULT_FEEDBACK;
		params.WetDryMix = FXECHO_DEFAULT_WETDRYMIX;

		SourceVoice->SetEffectParameters(0, &params, sizeof(FXECHO_PARAMETERS));
		SourceVoice->EnableEffect(0);
	}

	void Audio::DisableEcho()
	{
		if (SourceVoice)
		{
			SourceVoice->DisableEffect(0);
		}
	}

	void Audio::EnableEqualizer()
	{
		if (!SourceVoice) return;
		ComPtr<IUnknown> pXAPO;

		Hr = CreateFX(__uuidof(FXEQ), pXAPO.GetAddressOf());

		Hr = XAudio2CreateReverb(pXAPO.GetAddressOf());
		if (FAILED(Hr) || !pXAPO) return;
		XAUDIO2_EFFECT_DESCRIPTOR effectDesc{};
		effectDesc.InitialState = true;
		effectDesc.OutputChannels = audioInfo.Channels;
		effectDesc.pEffect = pXAPO.Get();
		XAUDIO2_EFFECT_CHAIN effectChain{};
		effectChain.EffectCount = 1;
		effectChain.pEffectDescriptors = &effectDesc;
		SourceVoice->SetEffectChain(&effectChain);

		FXEQ_PARAMETERS params{};
		params.Bandwidth0 = FXEQ_DEFAULT_BANDWIDTH;
		params.Bandwidth1 = FXEQ_DEFAULT_BANDWIDTH;
		params.Bandwidth2 = FXEQ_DEFAULT_BANDWIDTH;
		params.Bandwidth3 = FXEQ_DEFAULT_BANDWIDTH;
		params.FrequencyCenter0 = FXEQ_DEFAULT_FREQUENCY_CENTER_0;
		params.FrequencyCenter1 = FXEQ_DEFAULT_FREQUENCY_CENTER_1;
		params.FrequencyCenter2 = FXEQ_DEFAULT_FREQUENCY_CENTER_2;
		params.FrequencyCenter3 = FXEQ_DEFAULT_FREQUENCY_CENTER_3;
		params.Gain0 = FXEQ_DEFAULT_GAIN;
		params.Gain1 = FXEQ_DEFAULT_GAIN;
		params.Gain2 = FXEQ_DEFAULT_GAIN;
		params.Gain3 = FXEQ_DEFAULT_GAIN;

		SourceVoice->SetEffectParameters(0, &params, sizeof(FXEQ_PARAMETERS));
		SourceVoice->EnableEffect(0);
	}

	void Audio::DisableEqualizer()
	{
		if (SourceVoice)
		{
			SourceVoice->DisableEffect(0);
		}
	}

	std::wstring Audio::GetTimeW()
	{
		return ConvertTimeFormatW(
			GetTotalLength(
				audio.size(),
				audioInfo.SampleRate,
				static_cast<int>(audioInfo.Channels)
			)
		);
	}

	std::wstring Audio::GetNowTimeW()
	{
		XAUDIO2_VOICE_STATE state{};
		if (SourceVoice)
		{
			SourceVoice->GetState(&state);
		}
		return ConvertTimeFormatW(
			GetTotalLength(
				state.SamplesPlayed,
				audioInfo.SampleRate,
				1
			)
		);
	}

	XAUDIO2_VOICE_STATE Audio::State()
	{
		if (SourceVoice)
		{
			SourceVoice->GetState(&XAudio2State);
		}
		return XAudio2State;
	}

	const AudioInfo Audio::Information() const
	{
		return audioInfo;
	}

	const AudioMetaData Audio::MetaData() const
	{
		return audioMetaData;
	}

	const AudioFormat Audio::Format() const
	{
		return audioFormat;
	}

	const std::wstring Audio::FormatString() const
	{
		return Core::audioFormatString[(int)audioFormat];
	}
}
