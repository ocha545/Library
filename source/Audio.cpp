#include"../include/Audio.h"
#include"../include/audio_impl.h"

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

	//class MP3 begin
	MP3::MP3(const std::wstring& path, bool extract)
		: mp3(std::make_unique<drmp3>())
	{
		this->isRead = drmp3_init_file_w(this->mp3.get(), path.c_str(), nullptr);

		if (this->isRead && extract)
		{
			const int bufSize = 4096;
			std::unique_ptr<short> buffer(new short[bufSize * mp3->channels]);
			info.SampleRate = mp3->sampleRate;
			info.Channels = mp3->channels;
			info.BitsPerSample = 16;
			info.BlockAlign = static_cast<unsigned short>(info.Channels * 16 / 8);
			info.FormatTag = WAVE_FORMAT_PCM;
			info.AvgBytesPerSec = info.SampleRate * info.BlockAlign;
			pcm.clear();

			while (true)
			{
				drmp3_uint64 readLength = drmp3_read_pcm_frames_s16(mp3.get(), bufSize, buffer.get());
				
				if (readLength == 0)
				{
					//終端
					break;
				}
				if (readLength < 0)
				{
					drmp3_uninit(mp3.get());
					throw std::runtime_error("ファイルが破損してそう");
				}

				size_t sample = readLength * info.Channels;
				pcm.insert(pcm.end(), buffer.get(), buffer.get() + sample);
			}
			if (pcm.size() % info.Channels == 1)
			{
				size_t frames = pcm.size() / info.Channels;
				pcm.resize(frames);
			}
		}
	}
	MP3::~MP3()
	{
		if (isRead)
		{
			drmp3_uninit(this->mp3.get());
			this->mp3.release();
		}
	}
	MP3::operator bool()
	{
		return isRead;
	}
	const std::vector<short> MP3::getPCM() const
	{
		return this->pcm;
	}
	const AudioInfo MP3::getInformation() const
	{
		return this->info;
	}
	//class MP3 end

	//class WAVE begin
	WAVE::WAVE(const std::wstring& path, bool extract)
		: wav(std::make_unique<drwav>())
	{
		this->isRead = drwav_init_file_w(this->wav.get(), path.c_str(), nullptr);

		if (isRead && extract)
		{
			uint64_t allFrameCount =
				static_cast<uint64_t>(wav->channels) *
				static_cast<uint64_t>(wav->totalPCMFrameCount);

			if (allFrameCount > (std::numeric_limits<uint64_t>::max)())
			{
				drwav_uninit(wav.get());
				throw std::runtime_error("バッファサイズがunsigned int64の最大値を超えました");
			}

			pcm.resize(allFrameCount);
			drwav_read_pcm_frames_s16(wav.get(), wav->totalPCMFrameCount, pcm.data());

			info.FormatTag = WAVE_FORMAT_PCM;
			info.Channels = static_cast<unsigned short>(wav->channels);
			info.SampleRate = wav->sampleRate;
			info.BitsPerSample = wav->bitsPerSample;
			info.BlockAlign = static_cast<unsigned short>(wav->channels * wav->bitsPerSample / 8);
			info.AvgBytesPerSec = info.SampleRate * info.BlockAlign;
		}
	}
	WAVE::~WAVE()
	{
		if (isRead)
		{
			drwav_uninit(wav.get());
		}
	}
	WAVE::operator bool()
	{
		return isRead;
	}
	const std::vector<short> WAVE::getPCM() const
	{
		return this->pcm;
	}
	const AudioInfo WAVE::getInformation() const
	{
		return this->info;
	}
	//class WAVE end

	//class FLAC begin
	FLAC::FLAC(const std::wstring& path, bool extract)
		: flac(drflac_open_file_w(path.c_str(), nullptr))
	{
		this->isRead = (flac != nullptr);

		if (isRead && extract)
		{
			uint64_t allFrameCount =
				static_cast<uint64_t>(flac->channels) *
				static_cast<uint64_t>(flac->totalPCMFrameCount);
			if (allFrameCount > (std::numeric_limits<uint64_t>::max)())
			{
				drflac_close(flac);
				throw std::runtime_error("バッファサイズがunsigned int64の最大値を超えました");
			}

			pcm.resize(allFrameCount);
			drflac_read_pcm_frames_s16(flac, flac->totalPCMFrameCount, pcm.data());

			info.FormatTag = WAVE_FORMAT_PCM;
			info.Channels = static_cast<unsigned short>(flac->channels);
			info.SampleRate = flac->sampleRate;
			info.BitsPerSample = 16;
			info.BlockAlign = static_cast<unsigned short>(flac->channels * 16 / 8);
			info.AvgBytesPerSec = info.SampleRate * info.BlockAlign;
		}
	}
	FLAC::~FLAC()
	{
		if (isRead)
		{
			drflac_close(flac);
		}
	}
	FLAC::operator bool()
	{
		return this->isRead;
	}
	const std::vector<short> FLAC::getPCM() const
	{
		return this->pcm;
	}
	const AudioInfo FLAC::getInformation() const
	{
		return this->info;
	}
	//class FLAC end

	//class OGG begin
	OGG::OGG(const std::wstring& path, bool extract)
	{
		this->isRead = ov_fopen(Convert::MultiByteStr(path).c_str(), &ogg) == 0 ? true : false;

		if (isRead && extract)
		{
			vorbis_info* oggInfo = ov_info(&ogg, -1);
			info.SampleRate = oggInfo->rate;
			info.Channels = oggInfo->channels;
			info.BitsPerSample = 16;
			info.BlockAlign = static_cast<unsigned short>(info.Channels * 16 / 8);
			info.FormatTag = WAVE_FORMAT_PCM;
			info.AvgBytesPerSec = info.SampleRate * info.BlockAlign;

			const int bufSize = 4096;
			char buffer[bufSize];

			int bitStream = 0;
			long bytesRead = 0;


			pcm.clear();

			while (true)
			{
				bytesRead = ov_read(&ogg, buffer, bufSize, 0, 2, 1, &bitStream);

				if (bytesRead == 0)
				{
					//終端
					break;
				}

				if (bytesRead < 0)
				{
					ov_clear(&ogg);
					throw std::runtime_error("ファイルがはそんしてるっぽい");
				}
				for (int i = 0; i < bytesRead; i += 2)
				{
					short sample = (short)((unsigned char)buffer[i] | (unsigned char)buffer[i + 1] << 8);
					pcm.emplace_back(sample);
				}
			}
			if (pcm.size() % info.Channels == 1)
			{
				ov_clear(&ogg);
				throw;
			}
		}
	}
	OGG::~OGG()
	{
		if (this->isRead)
		{
			ov_clear(&ogg);
		}
	}
	OGG::operator bool()
	{
		return this->isRead;
	}
	const std::vector<short> OGG::getPCM() const
	{
		return this->pcm;
	}
	const AudioInfo OGG::getInformation() const
	{
		return this->info;
	}
	//class OGG end

	//class OPUS begin
	OPUS::OPUS(const std::wstring& path, bool extract)
	{
		int ret = 0;
		opus = op_open_file(Convert::MultiByteStr(path).c_str(), &ret);
		this->isRead = (ret == 0) ? true : false;

		if (isRead && extract)
		{
			const OpusHead* header = op_head(opus, -1);
			if (!header)
			{
				throw std::runtime_error("ヘッダー情報ないお");
			}
			//48000固定らしい
			info.SampleRate = 48000;
			info.Channels = header->channel_count;
			info.BitsPerSample = 16;
			info.BlockAlign = static_cast<unsigned short>(info.Channels * 16 / 8);
			info.FormatTag = WAVE_FORMAT_PCM;
			info.AvgBytesPerSec = info.SampleRate * info.BlockAlign;

			const int bufSize = 4096;
			std::unique_ptr<short> buffer(new short[bufSize * header->channel_count]);

			long bytesRead = 0l;
			pcm.clear();

			while (true)
			{
				bytesRead = op_read(opus, buffer.get(), bufSize * header->channel_count, NULL);
				if (bytesRead == 0l)
				{
					break;
				}
				if (bytesRead < 0l)
				{
					op_free(opus);
					throw std::runtime_error("ファイルがはそんしてるっぽい");
				}

				size_t sample = bytesRead * header->channel_count;
				pcm.insert(pcm.end(), buffer.get(), buffer.get() + sample);
			}
			if (pcm.size() % info.Channels == 1)
			{
				op_free(opus);
				throw;
			}
		}
	}
	OPUS::~OPUS()
	{
		op_free(opus);
	}
	OPUS::operator bool()
	{
		return this->isRead;
	}
	const std::vector<short> OPUS::getPCM() const
	{
		return this->pcm;
	}
	const AudioInfo OPUS::getInformation() const
	{
		return this->info;
	}
	//class OPUS end

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

//		WAVEFORMATEX format{}; <- ConvertWaveFormat(localInfo);
		if (localFormat == AudioFormat::WAVE)
		{
			WAVE wav{ path, true };
			localInfo = wav.getInformation();
			localAudio = wav.getPCM();
/*
			std::unique_ptr<drwav> wav(new drwav);

			if (!drwav_init_file_w(wav.get(), path.c_str(), nullptr))
			{
				throw std::runtime_error("DR Wavの初期化に失敗しました");
			}

			uint64_t frameCount =
				static_cast<uint64_t>(wav->channels) *
				static_cast<uint64_t>(wav->totalPCMFrameCount);

			if (frameCount > (std::numeric_limits<uint64_t>::max)())
			{
				drwav_uninit(wav.get());
				throw std::runtime_error("バッファサイズがunsigned int64の最大値を超えました");
			}

			localAudio.resize(frameCount);
			drwav_read_pcm_frames_s16(wav.get(), wav->totalPCMFrameCount, localAudio.data());

			format.wFormatTag = WAVE_FORMAT_PCM;
			format.nChannels = static_cast<unsigned short>(wav->channels);
			format.nSamplesPerSec = wav->sampleRate;
			format.wBitsPerSample = wav->bitsPerSample;
			format.nBlockAlign = static_cast<unsigned short>(wav->channels * wav->bitsPerSample / 8);
			format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;
			localInfo = setInfo(format);

			drwav_uninit(wav.get());
*/
		}
		else if (localFormat == AudioFormat::MP3)
		{
			MP3 mp3{ path, true };
			localInfo = mp3.getInformation();
			localAudio = mp3.getPCM();
/*
			long sampleRate = 0l;
			int channels = 0;
			if (!mp3GetPCM(path, localAudio, sampleRate, channels))
			{
				throw std::runtime_error("MP3ファイルの読み込みに失敗しました");
			}
			std::cout << "SR" << sampleRate << std::endl;
			std::cout << "CS" << channels << std::endl;

			if (localAudio.empty())
			{
				throw;
			}

			format.wFormatTag = WAVE_FORMAT_PCM;
			format.nChannels = static_cast<unsigned short>(channels);
			format.nSamplesPerSec = sampleRate;
			format.wBitsPerSample = 16;
			format.nBlockAlign = static_cast<unsigned short>(channels * 16 / 8);
			format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;
			localInfo = setInfo(format);


			//					auto meta = TagLib::MPEG::File(TagLib::FileName(path.c_str()), true);
			//					auto metag = meta.ID3v2Tag();

			auto meta = TagLib::FileRef(TagLib::FileName(path.c_str()), true);
			if (!meta.isNull() && meta.tag())
			{
				this->audioMetaData = setMetaData(*meta.tag());
			}
*/
		}
		else if (localFormat == AudioFormat::FLAC)
		{
			FLAC flac{ path, true };
			localInfo = flac.getInformation();
			localAudio = flac.getPCM();
/*
			drflac* flac = drflac_open_file_w(path.c_str(), nullptr);
			if (!flac)
			{
				throw std::exception("DR Flacの初期化に失敗しました");
				drflac_close(flac);
			}

			uint64_t frameCount =
				static_cast<uint64_t>(flac->channels) *
				static_cast<uint64_t>(flac->totalPCMFrameCount);
			if (frameCount > (std::numeric_limits<uint64_t>::max)())
			{
				drflac_close(flac);
				throw std::runtime_error("バッファサイズがunsigned int64の最大値を超えました");
			}

			localAudio.resize(frameCount);
			drflac_read_pcm_frames_s16(flac, flac->totalPCMFrameCount, localAudio.data());

			format.wFormatTag = WAVE_FORMAT_PCM;
			format.nChannels = static_cast<unsigned short>(flac->channels);
			format.nSamplesPerSec = flac->sampleRate;
			format.wBitsPerSample = 16;
			format.nBlockAlign = static_cast<unsigned short>(flac->channels * 16 / 8);
			format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;
			localInfo = setInfo(format);

			drflac_close(flac);
*/
		}
		else if (localFormat == AudioFormat::OGG)
		{
			OGG ogg{ path, true };
			localInfo = ogg.getInformation();
			localAudio = ogg.getPCM();
/*
			long sampleRate = 0;
			int channels = 0;
			if (!oggGetPCM(path, localAudio, sampleRate, channels))
			{
				throw std::runtime_error("Oggファイルの読み込みに失敗しました");
			}
			format.wFormatTag = WAVE_FORMAT_PCM;
			format.nChannels = channels;
			format.nSamplesPerSec = sampleRate;
			format.wBitsPerSample = 16;
			format.nBlockAlign = channels * 16 / 8;
			format.nAvgBytesPerSec = sampleRate * format.nBlockAlign;
			localInfo = setInfo(format);
*/
		}
		else if (localFormat == AudioFormat::OPUS)
		{
			OPUS opus{ path, true };
			localInfo = opus.getInformation();
			localAudio = opus.getPCM();
/*
			long sampleRate = 0;
			int channels = 0;
			if (!opusGetPCM(path, localAudio, sampleRate, channels))
			{
				throw std::runtime_error("Opusファイルの読み込みに失敗しました");
			}
			format.wFormatTag = WAVE_FORMAT_PCM;
			format.nChannels = channels;
			format.nSamplesPerSec = sampleRate;
			format.wBitsPerSample = 16;
			format.nBlockAlign = channels * 16 / 8;
			format.nAvgBytesPerSec = sampleRate * format.nBlockAlign;
			localInfo = setInfo(format);
*/
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
			this->coInitilized = true;
		}
		else
		{
			throw std::runtime_error("COMオブジェクトの初期化に失敗しました");
		}

		Hr = XAudio2Create(XAudio2.GetAddressOf());
		if (FAILED(Hr))
		{
			CoUninitialize();
			this->coInitilized = false;
			throw std::runtime_error("XAudio2の作成に失敗しました");
		}

		Hr = XAudio2->CreateMasteringVoice(&MasterVoice);
		if (FAILED(Hr))
		{
			XAudio2.Reset();
			CoUninitialize();
			this->coInitilized = false;
			throw std::runtime_error("マスターボイスの作成に失敗しました");
		}

		WAVEFORMATEX format = ConvertWaveFormat(localInfo);
		Hr = XAudio2->CreateSourceVoice(&SourceVoice, &format, XAUDIO2_VOICE_USEFILTER);
		if (FAILED(Hr))
		{
			XAudio2.Reset();
			CoUninitialize();
			this->coInitilized = false;
			throw std::runtime_error("ソースボイスの作成に失敗しました");
		}

		std::cout << "Audio Bytes : " << audioByteCount << "\n";
		std::cout << "LImit?	  : " << (audioByteCount >= 0x7fffffff);
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
			this->coInitilized = false;
			throw std::runtime_error(std::format("SouceBufferの送信に失敗しました:{}", Hr));
		}

		this->audio.swap(localAudio);
		this->audioInfo = localInfo;
		this->audioFormat = localFormat;
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

			if (this->coInitilized)
			{
				CoUninitialize();
				this->coInitilized = false;
			}
		}
		catch (std::exception)
		{
		}
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
		//				SourceVoice->SetEffectParameters(0, &reverbParameters[0], sizeof(reverbParameters[0]));
		SourceVoice->EnableEffect(0);

	}

	void Audio::DisableReverb()
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

//	const AudioMetaData Audio::MetaData() const
//	{
//		return audioMetaData;
//	}

	const AudioFormat Audio::Format() const
	{
		return audioFormat;
	}

	const std::wstring Audio::FormatString() const
	{
		return audioFormatString[(int)audioFormat];
	}
}
