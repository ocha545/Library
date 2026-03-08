#pragma once
#include"Macros.h"
#include"Helper.h"
#include"Utility.h"
// ComPtr メモリリークがトラウマなので
#include<wrl.h>
using Microsoft::WRL::ComPtr;

#include<string>
#include<vector>
#include<filesystem>
#include<codecvt>
#include<memory>
#include<math.h>

// 音声出力ライブラリ
#include<xaudio2.h>
#include<xaudio2fx.h>
#include<xapo.h>
#include<xapobase.h>
#include<xapofx.h>

// Audio
// 音声ファイルの詳細を取得するライブラリ
#include"third_party/taglib/tag.h"
#include"third_party/taglib/fileref.h"
#include"third_party/taglib/mpeg/mpegfile.h"
#include"third_party/taglib/toolkit/tlist.h"
// 音声ファイル読み取りライブラリ
// Wave, MP3, FLAC, OGG
#include"third_party/dr_wav.h"
#include"third_party/dr_mp3.h"
#include"third_party/dr_flac.h"
#include"third_party/vorbis/vorbisfile.h"
#include"third_party/opusfile/opusfile.h"


namespace Win32
{
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

	enum class Log
	{
		Information,
		Warning,
		Error,
		Other
	};

	static double GetTotalLength(size_t size, unsigned long sampleRate, int channels);
	static void SampleToTime(double in, int& h, int& m, double& s);
	static double TimeToSample(int h, int m, double s);
	static std::string ConvertTimeFormatA(double in);
	static std::wstring ConvertTimeFormatW(double in);
	static AudioFormat CheckFormat(const std::wstring& path);
	static WAVEFORMATEX ConvertWaveFormat(const AudioInfo& af);
	static void WriteLog()
	{

	}

	class MP3
	{
	private:
		std::unique_ptr<drmp3> mp3;
		std::vector<short> pcm;
		AudioInfo info{};
		AudioMetaData meta{};
		bool isRead;

	public:
		MP3(const std::wstring& path, bool extract);
		~MP3();
		operator bool();
		const std::vector<short> getPCM() const;
		const AudioInfo getInformation() const;
		const AudioMetaData getMetaData() const;
	};

	class WAVE
	{
	private:
		std::unique_ptr<drwav> wav;
		std::vector<short> pcm;
		AudioInfo info{};
		AudioMetaData meta{};
		bool isRead;

	public:
		WAVE(const std::wstring& path, bool extract);
		~WAVE();
		operator bool();
		const std::vector<short> getPCM() const;
		const AudioInfo getInformation() const;
		const AudioMetaData getMetaData() const;
	};

	class FLAC
	{
	private:
		drflac* flac = nullptr;
		std::vector<short> pcm;
		AudioInfo info{};
		AudioMetaData meta{};
		bool isRead;

	public:
		FLAC(const std::wstring& path, bool extract);
		~FLAC();
		operator bool();
		const std::vector<short> getPCM() const;
		const AudioInfo getInformation() const;
		const AudioMetaData getMetaData() const;
	};

	class OGG
	{
	private:
		OggVorbis_File ogg;
		std::vector<short> pcm;
		AudioInfo info{};
		AudioMetaData meta{};
		bool isRead;

	public:
		OGG(const std::wstring& path, bool extract);
		~OGG();
		operator bool();
		const std::vector<short> getPCM() const;
		const AudioInfo getInformation() const;
		const AudioMetaData getMetaData() const;
	};

	class OPUS
	{
	private:
		bool isRead;
		std::vector<short> pcm;
		AudioInfo info{};
		AudioMetaData meta{};
		OggOpusFile* opus;

	public:
		OPUS(const std::wstring& path, bool extract);
		~OPUS();
		operator bool();
		const std::vector<short> getPCM() const;
		const AudioInfo getInformation() const;
		const AudioMetaData getMetaData() const;
	};








	class LowShelfFilter {
	private:
		// フィルタ係数
		double b0, b1, b2, a0, a1, a2;
		// 遅延バッファ（過去の値を保持）
		double x1 = 0, x2 = 0, y1 = 0, y2 = 0;

	public:
		// sampleRate: 44100等, cutoff: 200Hz等, dbGain: 増減量(dB), Q: 0.707等
		void updateCoefficients(double sampleRate, double cutoff, double dbGain, double Q) {
			double A = std::pow(10.0, dbGain / 40.0);
			double omega = 2.0 * 3.14159265358979323846 * cutoff / sampleRate;
			double sn = std::sin(omega);
			double cs = std::cos(omega);
			double alpha = sn / (2.0 * Q);
			double beta = 2.0 * std::sqrt(A) * alpha;

			b0 = A * ((A + 1.0) - (A - 1.0) * cs + beta);
			b1 = 2.0 * A * ((A - 1.0) - (A + 1.0) * cs);
			b2 = A * ((A + 1.0) - (A - 1.0) * cs - beta);
			a0 = (A + 1.0) + (A - 1.0) * cs + beta;
			a1 = -2.0 * ((A - 1.0) + (A + 1.0) * cs);
			a2 = (A + 1.0) + (A - 1.0) * cs - beta;
		}

		float process(float input) {
			// 差分方程式の計算
			double output = (b0 / a0) * input + (b1 / a0) * x1 + (b2 / a0) * x2
				- (a1 / a0) * y1 - (a2 / a0) * y2;

			// 過去の値を更新
			x2 = x1;
			x1 = input;
			y2 = y1;
			y1 = output;

			return static_cast<float>(output);
		}
	};









	class Audio
	{
	private:
		ComPtr<IXAudio2> XAudio2 = nullptr;
		IXAudio2MasteringVoice* MasterVoice = nullptr;
		IXAudio2SourceVoice* SourceVoice = nullptr;

		XAUDIO2_VOICE_STATE XAudio2State{};
		int loopCount = 0;
		HRESULT Hr = NULL;
		bool coInitilized = false;

		std::vector<short>	audio;
		AudioMetaData			audioMetaData{};
		AudioInfo				audioInfo{};
		AudioFormat				audioFormat = AudioFormat::NONE;
		std::wstring			audioFormatString[(size_t)AudioFormat::MAX]{
			L"None",
			L"Wave",
			L"MP3",
			L"Flac",
			L"OGG",
			L"Opus",
		};

	public:
		Audio(const std::wstring& path, UINT32 loopCount = 0);
		Audio(const std::vector<short>& pcmData, const AudioInfo& info, UINT32 loopCount = 0);
		~Audio();

		void Stop();

		void Play();

		void EnableReverb();

		void DisableReverb();

		void EnableEcho();

		void DisableEcho();

		void EnableEuqalizer();

		void DisableEuqalizer();

		std::wstring GetTimeW();

		std::wstring GetNowTimeW();

		XAUDIO2_VOICE_STATE State();
			
		const AudioInfo Information() const;

		const AudioMetaData MetaData() const;

		const AudioFormat Format() const;

		const std::wstring FormatString() const;
	};
}

/*
namespace Win32_CPP
{
	namespace Audio
	{
		namespace Core
		{
			class MP3
			{
			private:
				std::unique_ptr<drmp3> mp3;
				bool isRead;

			public:
				MP3(const std::wstring& path)
					: mp3(new drmp3{})
				{
					this->isRead = drmp3_init_file_w(mp3.get(), path.c_str(), nullptr);
				}
				~MP3()
				{
					if (isRead)
					{
						drmp3_uninit(mp3.get());
					}
				}

				operator bool()
				{
					return this->isRead;
				}
			};

			class WAV
			{
			private:
				std::unique_ptr<drwav> wav;
				bool isRead;

			public:
				WAV(const std::wstring& path)
					: wav(new drwav)
				{
					this->isRead = drwav_init_file_w(wav.get(), path.c_str(), nullptr);
				}
				~WAV()
				{
					if (isRead)
					{
						drwav_uninit(wav.get());
					}
				}

				operator bool()
				{
					return this->isRead;
				}
			};

			class FLAC
			{
			private:
				drflac* flac = nullptr;
				bool isRead;

			public:
				FLAC(const std::wstring& path)
					: flac(drflac_open_file_w(path.c_str(), nullptr))
				{
					this->isRead = (flac);
				}
				~FLAC()
				{
					if (isRead)
					{
						drflac_close(flac);
					}
				}

				operator bool()
				{
					return this->isRead;
				}
			};

			class OGG
			{
			private:
				OggVorbis_File ogg;
				bool isRead;

			public:
				OGG(const std::wstring& path)
				{
					this->isRead = ov_fopen(Convert::MultiByteStr(path).c_str(), &ogg) == 0 ? true : false;
				}
				~OGG()
				{
					if (isRead)
					{
						ov_clear(&ogg);
					}
				}

				operator bool()
				{
					return this->isRead;
				}
			};

			class OPUS
			{
			private:
				bool isRead;
				OggOpusFile* opus;

			public:
				OPUS(const std::wstring& path)
				{
					int ret;
					opus = op_open_file(Convert::MultiByteStr(path).c_str(), &ret);
					this->isRead = (ret == 0) ? true : false;
				}
				~OPUS()
				{
					op_free(this->opus);
				}

				operator bool()
				{
					return this->isRead;
				}
			};
		}

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

		//使い古しです。今後使えたらいいね～
//		typedef enum AudioEffect
//		{
//			BATHRROOM = 0,
//			MAX,
//		}AudioEffect;

		typedef struct AudioMetaData
		{
			std::wstring title = L"";
			std::wstring artist = L"";
			std::wstring album = L"";
			std::wstring genre = L"";
			std::wstring comment = L"";
			unsigned int track = 0;
			unsigned int year = 0;
			Image jacket{};
		}AudioMetaData;

		typedef struct AudioInfo
		{
			unsigned short FormatTag = NULL;
			unsigned short Channels = NULL;
			unsigned long SampleRate = NULL;
			unsigned long nAvgBytesPerSec = NULL;
			unsigned short BlockAlign = NULL;
			unsigned short BitsPerSample = NULL;
		}AudioInfo;

		inline double GetTotalLength(size_t size, unsigned long sampleRate, int channels)
		{
			if (sampleRate <= 0 || channels <= 0) return 0.0;

			double totalSamples = static_cast<double>(size) / static_cast<double>(channels);
			return totalSamples / static_cast<double>(sampleRate);
		}

		inline void SampleToTime(double in, int& h, int& m, double& s)
		{
			if (in <= 0.0) in = 0.0;

			h = static_cast<int>(in / 3600);
			in = std::fmod(in, 3600);
			m = static_cast<int>(in / 60);
			in = std::fmod(in, 60);
			s = in;
		}

		inline double TimeToSample(int h, int m, double s)
		{
			double tmp = 0.0;
			tmp += h * 3600;
			tmp += m * 60;
			tmp += s;
			return tmp;
		}

		inline std::string ConvertTimeFormatA(double in)
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

		inline std::wstring ConvertTimeFormatW(double in)
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

		class Audio SEALED
		{
		private:
			ComPtr<IXAudio2> XAudio2 = nullptr;
			IXAudio2MasteringVoice* MasterVoice = nullptr;
			IXAudio2SourceVoice* SourceVoice = nullptr;

			XAUDIO2_VOICE_STATE XAudio2State{};
			int loopCount = 0;
			HRESULT Hr = NULL;
			bool coInitilized = false;

			std::vector<short>	audio;
			AudioMetaData			audioMetaData{};
			AudioInfo				audioInfo{};
			AudioFormat				audioFormat = AudioFormat::NONE;
			std::wstring			audioFormatString[(size_t)AudioFormat::MAX]{
				L"None",
				L"Wave",
				L"MP3",
				L"Flac",
				L"OGG",
				L"Opus",
			};

			bool opusGetPCM(const std::wstring& path, std::vector<short>& pcmData, long& sampleRate, int& channels)
			{
				OggOpusFile* opus = op_open_file(Convert::MultiByteStr(path).c_str(), nullptr);
				const OpusHead* header = op_head(opus, -1);
				if (!header)
				{
					return false;
				}
				//48000固定らしい
				sampleRate = 48000;
				channels = header->channel_count;
				const int bufSize = 4096;
				std::unique_ptr<short> buffer(new short[bufSize * header->channel_count]);

				long bytesRead = 0l;

				pcmData.clear();

				while (true)
				{
					bytesRead = op_read(opus, buffer.get(), bufSize * header->channel_count, NULL);
					if (bytesRead == 0l)
					{
						break;
					}
					if (bytesRead < 0l)
					{
						std::cout << "DBG bytesRead < 0" << std::endl;
						op_free(opus);
						return false;
					}

					size_t sample = bytesRead * header->channel_count;
					pcmData.insert(pcmData.end(), buffer.get(), buffer.get() + sample);
				}
				if (pcmData.size() % channels == 1)
				{
					op_free(opus);
					return false;
				}

				op_free(opus);
				return true;
			}

			bool oggGetPCM(const std::wstring& path, std::vector<short>& pcmData, long& sampleRate, int& channels)
			{
				OggVorbis_File ogg;
				std::string mbstr = Convert::MultiByteStr(path).c_str();
				std::cout << "DBG Path:" << mbstr << std::endl;

				int error = ov_fopen(mbstr.c_str(), &ogg);
				if (error != 0)
				{
					switch (error)
					{
					case OV_EREAD:
					case OV_ENOTVORBIS:
					case OV_EVERSION:
					case OV_EBADHEADER:
					case OV_EFAULT:
						break;
					default:
						break;
					}
					std::cout << "DBG error != 0" << std::endl;
					return false;
				}

				vorbis_info* info = ov_info(&ogg, -1);
				sampleRate = info->rate;
				channels = info->channels;

				const int bufSize = 4096;
				char buffer[bufSize];

				int bitStream = 0;
				long bytesRead = 0;

				pcmData.clear();

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
						std::cout << "DBG bytesRead < 0" << std::endl;
						ov_clear(&ogg);
						return false;
					}
					for (int i = 0; i < bytesRead; i += 2)
					{
						short sample = (short)((unsigned char)buffer[i] | (unsigned char)buffer[i + 1] << 8);
						pcmData.emplace_back(sample);
					}
				}
				if (pcmData.size() % channels == 1)
				{
					ov_clear(&ogg);
					return false;
				}

				ov_clear(&ogg);
				return true;
			}

			bool mp3GetPCM(const std::wstring& path, std::vector<short>& pcmData, long& sampleRate, int& channels)
			{
				std::unique_ptr<drmp3> mp3(new drmp3);
				if (!drmp3_init_file_w(mp3.get(), path.c_str(), nullptr))
				{
					throw std::runtime_error("DR MP3の初期化に失敗しました");
				}
				const int bufSize = 4096;
				std::unique_ptr<short> buffer(new short[bufSize * mp3->channels]);
				sampleRate = mp3->sampleRate;
				channels = mp3->channels;
				pcmData.clear();

				while (true)
				{
					drmp3_uint64 framesRead = drmp3_read_pcm_frames_s16(mp3.get(), bufSize, buffer.get());

					if (framesRead == 0)
					{
						//終端
						break;
					}
					if (framesRead < 0)
					{
						std::cout << "DBG bytesRead < 0" << std::endl;
						drmp3_uninit(mp3.get());
						return false;
					}

					size_t sample = framesRead * mp3->channels;
					pcmData.insert(pcmData.end(), buffer.get(), buffer.get() + sample);

//					size_t sampleCount = static_cast<size_t>(framesRead) * channels;
//					pcmData.insert(pcmData.end(), buffer.get(), buffer.get() + framesRead);
				}
				if (pcmData.size() % channels == 1)
				{
					//あまっちゃったデータを補正
					size_t frames = pcmData.size() / channels;
					pcmData.resize(frames * channels);
				}

				drmp3_uninit(mp3.get());
				return true;
			}

			AudioInfo setInfo(const WAVEFORMATEX& fmt)
			{
				return AudioInfo
				{
					.FormatTag = fmt.wFormatTag,
					.Channels = fmt.nChannels,
					.SampleRate = fmt.nSamplesPerSec,
					.nAvgBytesPerSec = fmt.nAvgBytesPerSec,
					.BlockAlign = fmt.nBlockAlign,
					.BitsPerSample = fmt.wBitsPerSample,
				};
			}

			AudioMetaData setMetaData(const TagLib::Tag& meta)
			{
				return AudioMetaData
				{
					.title = Convert::WideStr(meta.title().toCString(true)),
					.artist = Convert::WideStr(meta.artist().toCString(true)),
					.album = Convert::WideStr(meta.album().toCString(true)),
					.genre = Convert::WideStr(meta.genre().toCString(true)),
					.comment = Convert::WideStr(meta.comment().toCString(true)),
					.track = meta.track(),
					.year = meta.year(),
				};
			}

		public:
			Audio() = default;
			Audio(const Audio&) = delete;
			Audio(Audio&&) noexcept = default;
			Audio& operator=(const Audio&) = delete;
			Audio&& operator=(Audio&&) noexcept = delete;

			Audio(const std::wstring& path, UINT32 loopCount = 0)
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
				AudioFormat localFormat = Audio::CheckFormat(path);

//				std::cout << audioFormatString[(int)localFormat];
//				throw;

				WAVEFORMATEX format{};
				if (localFormat == AudioFormat::WAVE)
				{
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
				}
				else if (localFormat == AudioFormat::MP3)
				{
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
				}
				else if (localFormat == AudioFormat::FLAC)
				{
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
				}
				else if (localFormat == AudioFormat::OGG)
				{
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
				}
				else if (localFormat == AudioFormat::OPUS)
				{
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
				}
				else
				{
					throw std::runtime_error("現在サポートされていないフォーマットです");
				}

				const uint64_t audioByteCount = static_cast<uint64_t>(localAudio.size()) * sizeof(short);
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
				ZeroMemory(&xAudio2Buffer, sizeof(XAUDIO2_BUFFER));


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
			~Audio()
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

			void Stop()
			{
				if (SourceVoice)
				{
					SourceVoice->Stop();
				}
			}

			void Play()
			{
				if (SourceVoice)
				{
					SourceVoice->Start();
				}
			}

			void EnableReverb()
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

			void DisableReverb()
			{
				if (SourceVoice)
				{
					SourceVoice->DisableEffect(0);
				}
			}

			std::wstring GetTimeW()
			{
				return ConvertTimeFormatW(
					GetTotalLength(
						audio.size(),
						audioInfo.SampleRate,
						static_cast<int>(audioInfo.Channels)
					)
				);
			}

			std::wstring GetNowTimeW()
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

			XAUDIO2_VOICE_STATE State()
			{
				if (SourceVoice)
				{
					SourceVoice->GetState(&XAudio2State);
				}
				return XAudio2State;
			}

			const AudioInfo Information() const
			{
				return audioInfo;
			}

			const AudioMetaData MetaData() const
			{
				return audioMetaData;
			}

			const AudioFormat Format() const
			{
				return audioFormat;
			}

			const std::wstring FormatString() const
			{
				return audioFormatString[(int)audioFormat];
			}

			static AudioMetaData MetaData(const std::wstring& path)
			{
				TagLib::FileRef rawData = TagLib::FileRef(path.c_str());
				if (!rawData.isNull() && rawData.tag())
				{
					return AudioMetaData{
						.title = Convert::WideStr(rawData.tag()->title().toCString(true)),
						.artist = Convert::WideStr(rawData.tag()->artist().toCString(true)),
						.album = Convert::WideStr(rawData.tag()->album().toCString(true)),
						.genre = Convert::WideStr(rawData.tag()->genre().toCString(true)),
						.comment = Convert::WideStr(rawData.tag()->comment().toCString(true)),
						.track = rawData.tag()->track(),
						.year = rawData.tag()->year(),
					};
				}
				return AudioMetaData{};
			}

			static AudioFormat CheckFormat(const std::wstring& path)
			{
				//Wave
				{
					Core::WAV wav{ path };
					if (wav)
					{
						return AudioFormat::WAVE;
					}
				}
				//MP3
				{
					Core::MP3 mp3{ path };
					if (mp3)
					{
						return AudioFormat::MP3;
					}
				}
				//Flac
				{
					Core::FLAC flac{ path };
					if (flac)
					{
						return AudioFormat::FLAC;
					}
				}
				//OGG
				{
					Core::OGG ogg{ path };
					if (ogg)
					{
						return AudioFormat::OGG;
					}
				}
				//Opus
				{
					Core::OPUS opus{ path };
					if (opus)
					{
						return AudioFormat::OPUS;
					}
				}

				return AudioFormat::NONE;
			}

			class BuildTag
			{
			private:
				std::string path;
				TagLib::FileRef tags;
				AudioFormat format = AudioFormat::NONE;

			public:
				BuildTag() = default;
				BuildTag(const BuildTag&) = default;

				BuildTag& Path(const std::string& file)
				{
					tags = TagLib::FileRef(file.c_str(), true);
					path = file;
					return *this;
				}
				BuildTag& Format(const AudioFormat af)
				{
					format = af;
					return *this;
				}
				BuildTag& Title(const std::string& txt)
				{
					tags.tag()->setTitle(TagLib::String(txt,
						format == AudioFormat::WAVE ? TagLib::String::Latin1 : TagLib::String::UTF8));
					return *this;
				}
				BuildTag& Artist(const std::string& txt)
				{
					tags.tag()->setArtist(TagLib::String(txt,
						format == AudioFormat::WAVE ? TagLib::String::Latin1 : TagLib::String::UTF8));
					return *this;
				}
				BuildTag& Album(const std::string txt)
				{
					tags.tag()->setAlbum(TagLib::String(txt,
						format == AudioFormat::WAVE ? TagLib::String::Latin1 : TagLib::String::UTF8));
					return *this;
				}
				BuildTag& Genre(const std::string txt)
				{
					tags.tag()->setGenre(TagLib::String(txt,
						format == AudioFormat::WAVE ? TagLib::String::Latin1 : TagLib::String::UTF8));
					return *this;
				}
				BuildTag& Year(unsigned int year)
				{
					tags.tag()->setYear(year);
					return *this;
				}
				BuildTag& Track(unsigned int track)
				{
					tags.tag()->setTrack(track);
					return *this;
				}
				void Build()
				{
					tags.save();
				}
			};
		};
	}
}
*/