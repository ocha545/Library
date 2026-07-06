#include"../include/AudioFormat.h"
// class MP3
// class WAVE
// class FLAC
// class OGG
// class OPUS

namespace Win32
{
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


			TagLib::FileRef tagFile(TagLib::FileName(path.c_str()), true);
			auto jacket = tagFile.tag()->complexProperties("PICTURE");
			auto keys = tagFile.tag()->complexPropertyKeys();
			if (tagFile.tag() && !tagFile.isNull())
			{
				meta = AudioMetaData2{ tagFile.tag() };
				//if (!jacket.isEmpty())
				//{
				//	std::ofstream outFile{ "cover.jpeg", std::ios::out | std::ios::binary };
				//	auto pictureData = jacket.front()["data"].toByteVector();
				//	outFile.write(pictureData.data(), pictureData.size());
				//	outFile.close();
				//	meta.jacket = Image{ L"cover.jpeg" };
				//}

				//meta.title = tagFile.tag()->title().toWString();
				//meta.artist = tagFile.tag()->artist().toWString();
				//meta.album = tagFile.tag()->album().toWString();
				//meta.genre = tagFile.tag()->genre().toWString();
				//meta.comment = tagFile.tag()->comment().toWString();
				//meta.track = tagFile.tag()->track();
				//meta.year = tagFile.tag()->year();
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
	const AudioMetaData2 MP3::getMetaData() const
	{
		return meta;
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
			if (wav->bitsPerSample == 8)
			{
				drwav_read_raw(wav.get(), wav->totalPCMFrameCount, pcm.data());
			}
			else
			{
				drwav_read_pcm_frames_s16(wav.get(), wav->totalPCMFrameCount, pcm.data());
			}

			info.FormatTag = WAVE_FORMAT_PCM;
			info.Channels = static_cast<unsigned short>(wav->channels);
			info.SampleRate = wav->sampleRate;
			info.BitsPerSample = wav->bitsPerSample;
			info.BlockAlign = static_cast<unsigned short>(wav->channels * wav->bitsPerSample / 8);
			info.AvgBytesPerSec = info.SampleRate * info.BlockAlign;
			pcm.shrink_to_fit();

			TagLib::FileRef tagFile(TagLib::FileName(path.c_str()), true);
			if (tagFile.tag() && !tagFile.isNull())
			{
				meta = AudioMetaData2{ tagFile.tag() };

				//meta.title = tagFile.tag()->title().toWString();
				//meta.artist = tagFile.tag()->artist().toWString();
				//meta.album = tagFile.tag()->album().toWString();
				//meta.genre = tagFile.tag()->genre().toWString();
				//meta.comment = tagFile.tag()->comment().toWString();
				//meta.track = tagFile.tag()->track();
				//meta.year = tagFile.tag()->year();
			}
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
	const AudioMetaData2 WAVE::getMetaData() const
	{
		return meta;
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

			TagLib::FileRef tagFile(TagLib::FileName(path.c_str()), true);
			auto jacket = tagFile.tag()->complexProperties("PICTURE");
			auto keys = tagFile.tag()->complexPropertyKeys();
			if (tagFile.tag() && !tagFile.isNull())
			{
				meta = AudioMetaData2{ tagFile.tag() };
				//if (!jacket.isEmpty())
				//{
				//	std::ofstream outFile{ "cover.jpeg", std::ios::out | std::ios::binary };
				//	auto pictureData = jacket.front()["data"].toByteVector();
				//	outFile.write(pictureData.data(), pictureData.size());
				//	outFile.close();
				//	meta.jacket = Image{ L"cover.jpeg" };
				//}

				//meta.title = tagFile.tag()->title().toWString();
				//meta.artist = tagFile.tag()->artist().toWString();
				//meta.album = tagFile.tag()->album().toWString();
				//meta.genre = tagFile.tag()->genre().toWString();
				//meta.comment = tagFile.tag()->comment().toWString();
				//meta.track = tagFile.tag()->track();
				//meta.year = tagFile.tag()->year();
			}
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
	const AudioMetaData2 FLAC::getMetaData() const
	{
		return meta;
	}
	//class FLAC end

	//class OGG begin
	OGG::OGG(const std::wstring& path, bool extract)
	{
		this->isRead = ov_fopen(String::MultiByteStr(path).c_str(), &ogg) == 0 ? true : false;

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

			TagLib::FileRef tagFile(TagLib::FileName(path.c_str()), true);
			auto jacket = tagFile.tag()->complexProperties("PICTURE");
			auto keys = tagFile.tag()->complexPropertyKeys();
			if (tagFile.tag() && !tagFile.isNull())
			{
				meta = AudioMetaData2{ tagFile.tag() };
				//if (!jacket.isEmpty())
				//{
				//	std::ofstream outFile{ "cover.jpeg", std::ios::out | std::ios::binary };
				//	auto pictureData = jacket.front()["data"].toByteVector();
				//	outFile.write(pictureData.data(), pictureData.size());
				//	outFile.close();
				//	meta.jacket = Image{ L"cover.jpeg" };
				//}

				//meta.title = tagFile.tag()->title().toWString();
				//meta.artist = tagFile.tag()->artist().toWString();
				//meta.album = tagFile.tag()->album().toWString();
				//meta.genre = tagFile.tag()->genre().toWString();
				//meta.comment = tagFile.tag()->comment().toWString();
				//meta.track = tagFile.tag()->track();
				//meta.year = tagFile.tag()->year();
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
	const AudioMetaData2 OGG::getMetaData() const
	{
		return meta;
	}
	//class OGG end

	//class OPUS begin
	OPUS::OPUS(const std::wstring& path, bool extract)
	{
		int ret = 0;
		opus = op_open_file(String::MultiByteStr(path).c_str(), &ret);
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

				size_t sample = static_cast<size_t>(bytesRead * header->channel_count);
				pcm.insert(pcm.end(), buffer.get(), buffer.get() + sample);
			}
			if (pcm.size() % info.Channels == 1)
			{
				op_free(opus);
				throw;
			}

			TagLib::FileRef tagFile(TagLib::FileName(path.c_str()), true);
			auto jacket = tagFile.tag()->complexProperties("PICTURE");
			auto keys = tagFile.tag()->complexPropertyKeys();
			if (tagFile.tag() && !tagFile.isNull())
			{
				meta = AudioMetaData2{ tagFile.tag() };
				//if (!jacket.isEmpty())
				//{
				//	std::ofstream outFile{ "cover.jpeg", std::ios::out | std::ios::binary };
				//	auto pictureData = jacket.front()["data"].toByteVector();
				//	outFile.write(pictureData.data(), pictureData.size());
				//	outFile.close();
				//	meta.jacket = Image{ L"cover.jpeg" };
				//}

				//meta.title = tagFile.tag()->title().toWString();
				//meta.artist = tagFile.tag()->artist().toWString();
				//meta.album = tagFile.tag()->album().toWString();
				//meta.genre = tagFile.tag()->genre().toWString();
				//meta.comment = tagFile.tag()->comment().toWString();
				//meta.track = tagFile.tag()->track();
				//meta.year = tagFile.tag()->year();
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
	const AudioMetaData2 OPUS::getMetaData() const
	{
		return meta;
	}
	//class OPUS end
}