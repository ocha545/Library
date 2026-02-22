#pragma once
#include<string>
#include<vector>
#include<fstream>

/*
100,100,\r\n
200,200,\r\n
100,100,\r\n
200,200,\r\n
100,100,\r\n
200,200,\r\n
100,100,\r\n
200,200,\r\n
*/


namespace Win32_CPP
{
	namespace DataFormat
	{
		class CSV
		{
		private:
			std::vector<std::string> content{};

			std::vector<std::string> split(const std::string_view text, char splitTxt)
			{
				// "csv, vsg, rgb, ppu, akb";
				std::vector<std::string> tmp;
				for(int i = 0; i < text.length(); i++)
				{
					int startPos = 0;
					if (text[i] == splitTxt)
					{
						tmp.emplace_back(text.substr(startPos, i));
						startPos += i;
					}
				}
				return tmp;
			}

		public:
			CSV(std::string path)
			{
				std::ifstream read{ path };

				if (!read.is_open())
				{
					throw std::runtime_error("指定されたファイルを開けませんでした");
				}

				std::string text;
				while (std::getline(read, text))
				{
					content.emplace_back(text);
				}

				std::cout << "contents : " << content.size() << "\n";
				for (const auto& txt : content)
				{
					std::cout << "content  : " << txt << "\n";
				}
			}
		};
	}
}