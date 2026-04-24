#pragma once
#include"Macros.h"
#include"Helper.h"
#include"Utility.h"
#include"AudioResource.h"
#include"AudioFormat.h"
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
#include<mmdeviceapi.h>
#include<endpointvolume.h>
#include<functiondiscoverykeys.h>

namespace Win32
{

	//未使用
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


	//テストよう (gemini作)
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




	namespace Core
	{
		static  std::wstring audioFormatString[(size_t)AudioFormat::MAX]{
			L"None",
			L"Wave",
			L"MP3",
			L"Flac",
			L"OGG",
			L"Opus",
		};
	}

	class Audio
	{
	private:
		ComPtr<IXAudio2> XAudio2 = nullptr;
		IXAudio2MasteringVoice* MasterVoice = nullptr;
		IXAudio2SourceVoice* SourceVoice = nullptr;

		XAUDIO2_VOICE_STATE XAudio2State{};
		int loopCount = 0;
		HRESULT Hr = NULL;
		bool coInitialized = false;

		std::vector<short>	audio;
		AudioMetaData2		audioMetaData{};
		AudioInfo			audioInfo{};
		AudioFormat			audioFormat = AudioFormat::NONE;

	public:
		HRESULT EnumerateDevicesInternal(std::vector<AudioDeviceInfo>& devices);

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

		void EnableEqualizer();

		void DisableEqualizer();

		std::wstring GetTimeW();

		std::wstring GetNowTimeW();

		XAUDIO2_VOICE_STATE State();
			
		const AudioInfo Information() const;

		const AudioMetaData2 MetaData() const;

		const AudioFormat Format() const;

		const std::wstring FormatString() const;
	};
}
