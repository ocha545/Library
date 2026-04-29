#include"../include/AudioResource.h"

namespace Win32
{
	AudioMetaData2::AudioMetaData2(const TagLib::Tag* tag, bool disableImage)
	{
		allTag =
			tag->title().toWString() + SEPARATOR +
			tag->artist().toWString() + SEPARATOR +
			tag->album().toWString() + SEPARATOR +
			tag->genre().toWString() + SEPARATOR +
			tag->comment().toWString();

		t = tag->track();
		y = tag->year();
//		if (!disableImage)
		if (false)
		{
			std::ofstream outFile{ "cover.jpeg", std::ios::out | std::ios::binary };
			auto pictureData = tag->complexProperties("PICTURE").front()["data"].toByteVector();
			outFile.write(pictureData.data(), pictureData.size());
			outFile.close();
			j = Image{ L"cover.jpeg" };
			std::filesystem::remove(L"cover.jpeg");
		}
	}

	std::wstring AudioMetaData2::title() const
	{
		size_t tagSize = String::Split(allTag, SEPARATOR).size() - 1;
		if (tagSize < 0)
		{
			return L"None";
		}
		return String::Split(allTag, SEPARATOR)[0];
	}
	std::wstring AudioMetaData2::artist() const
	{
		size_t tagSize = String::Split(allTag, SEPARATOR).size() - 1;
		if (tagSize < 1)
		{
			return L"None";
		}
		return String::Split(allTag, SEPARATOR)[1];
	}
	std::wstring AudioMetaData2::album() const
	{
		size_t tagSize = String::Split(allTag, SEPARATOR).size() - 1;
		if (tagSize < 2)
		{
			return L"None";
		}
		return String::Split(allTag, SEPARATOR)[2];
	}
	std::wstring AudioMetaData2::genre() const
	{
		size_t tagSize = String::Split(allTag, SEPARATOR).size() - 1;
		if (tagSize < 3)
		{
			return L"None";
		}
		return String::Split(allTag, SEPARATOR)[3];
	}
	std::wstring AudioMetaData2::comment() const
	{
		size_t tagSize = String::Split(allTag, SEPARATOR).size() - 1;
		if (tagSize < 4)
		{
			return L"None";
		}
		return String::Split(allTag, SEPARATOR)[4];
	}
	unsigned int AudioMetaData2::track() const
	{
		return t;
	}
	unsigned int AudioMetaData2::year() const
	{
		return y;
	}
	Image AudioMetaData2::jacket() const
	{
		return j;
	}
}
