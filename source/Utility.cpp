#include"../include/Utility.h"
#pragma warning(push)
#pragma warning(disable: 6262)//関数はスタックの '35036' バイトを使用します。データの一部をヒープに移動することを考慮してください。
#pragma warning(disable: 26819)//switch ラベルどうしの間に、注釈の付いていないフォールスルーがあります (es.78)。
#pragma warning(disable: 4996)//'sprintf': This function or variable may be unsafe. Consider using sprintf_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
#define STB_IMAGE_IMPLEMENTATION
#include"../include/third_party/stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include"../include/third_party/stb/stb_image_write.h"
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable: 4018)//'<': signed と unsigned の数値を比較しようとしました。
namespace Win32
{
	//struct Color begin
	Color::Color() : 
		r(0xff), g(0xff), b(0xff), a(0xff)
	{
	}
	Color::Color(ubyte r, ubyte g, ubyte b) : 
		r(r), g(g), b(b), a(0xff)
	{
	}
	Color::Color(ubyte r, ubyte g, ubyte b, ubyte a) :
		r(r), g(g), b(b), a(a)
	{
	}
	Color::Color(ubyte gs) :
		r(gs),
		g(gs),
		b(gs),
		a(0xff)
	{
	}
	Color::Color(ubyte gs, ubyte alpha) : 
		r(gs),
		g(gs),
		b(gs),
		a(alpha)
	{
	}
	Color::Color(const wstring& hex)
	{
		if (hex.length() < 7)
		{
			throw std::invalid_argument("カラーコードの形式が無効です( #RRGGBB の形式で入力してください)");
		}

		wstring hexDigit = hex.substr(1);

		try
		{
			r = std::stoi(hexDigit.substr(0, 2), nullptr, 16);
			g = std::stoi(hexDigit.substr(2, 2), nullptr, 16);
			b = std::stoi(hexDigit.substr(4, 2), nullptr, 16);
			a = 0xff;
		}
		catch (std::exception& e)
		{
			throw  std::invalid_argument(std::format("カラーコードのパースに失敗しました:{}", e.what()));
		}
	}

	Color operator+(Color base, Color blend)
	{
		return Color{
			std::min<ubyte>(255, base.r + blend.r),
			std::min<ubyte>(255, base.g + blend.g),
			std::min<ubyte>(255, base.b + blend.b),
			std::min<ubyte>(255, base.a + blend.a)
		};
	}
	Color operator-(Color base, Color blend)
	{
		return Color{
			std::max<ubyte>(0, base.r - blend.r),
			std::max<ubyte>(0, base.g - blend.g),
			std::max<ubyte>(0, base.b - blend.b),
			std::max<ubyte>(0, base.a - blend.a)
		};
	}
	Color operator*(Color base, Color blend)
	{
		return Color{
			static_cast<ubyte>((base.r * blend.r) / 0xff),
			static_cast<ubyte>((base.g * blend.g) / 0xff),
			static_cast<ubyte>((base.b * blend.b) / 0xff),
			static_cast<ubyte>((base.a * blend.a) / 0xff)
		};
	}
	Color Color::operator+=(Color blend)
	{
		*this = *this + blend;
		return *this;
	}
	Color Color::operator-=(Color blend)
	{
		*this = *this - blend;
		return *this;
	}
	Color Color::operator*=(Color blend)
	{
		*this = *this * blend;
		return *this;
	}
	bool operator==(Color left, Color right)
	{
		return ((left.r == right.r) && (left.g == right.g) && (left.b == right.b) && (left.a == right.a));
	}

	std::ostream& operator<<(std::ostream& os, Color color)
	{
		os << "R:" << (int)color.r
			<< ", G:" << (int)color.g
			<< ", B:" << (int)color.b
			<< ", A:" << (int)color.a;
		return os;
	}

	XMFLOAT4 Color::floats() const
	{
		return{
			static_cast<float>(r) * inv255,
			static_cast<float>(g) * inv255,
			static_cast<float>(b) * inv255,
			static_cast<float>(a) * inv255
		};
	}
	Color Color::IntHEX(unsigned int hex)
	{
		ubyte r = (hex >> 8 * 3) & 0xff;
		ubyte g = (hex >> 8 * 2) & 0xff;
		ubyte b = (hex >> 8 * 1) & 0xff;
		ubyte a = hex & 0xff;

		return Color(r, g, b, a);
	}
	//struct Color end

	//class Image begin
	Image::Image()
	{
		const int rgba = 255 / 100;
		width = 100;
		height = 100;
		const size_t size = static_cast<size_t>(width * height);
		if (size > SIZE_MAX) throw std::runtime_error("データサイズが大きすぎます！");
		colors.resize(size);
		for (int y = 0; y < 100; y++)
		{
			for (int x = 0; x < 100; x++)
			{
				colors[static_cast<size_t>(x + 100 * y)] = Color(rgba * y, rgba * x, rgba * x / 2);
			}
		}
	}
	Image::Image(wstring_view path)
	{
		if (!std::filesystem::exists(path))
		{
			throw std::runtime_error(std::format("ファイルが存在しません \n path:{}", String::MultiByteStr(path).c_str()));
		}

		std::string mbPath = String::MultiByteStr(path);

		unsigned char* pixels = stbi_load(mbPath.c_str(), &width, &height, &channels, sizeof(Color));
		if (!pixels)
		{
			throw std::runtime_error(std::format("画像データが破損しています \n path", mbPath));
		}

		const size_t size = static_cast<size_t>(width * height);
		if (size > SIZE_MAX)
		{
			stbi_image_free(pixels);
			throw std::runtime_error("データサイズが大きすぎます！");
		}

		colors.clear();
		colors.resize(size);
		std::memcpy(colors.data(), pixels, size * sizeof(Color));

		stbi_image_free(pixels);
	}
	Image::Image(int width, int height)
		: width(width), height(height)
	{
		const size_t size = static_cast<size_t>(width * height);
		if (size > SIZE_MAX) throw std::runtime_error("データサイズが大きすぎます！");

		colors.clear();
		colors.resize(size);
	}
	Image::Image(int width, int height, Color singleColor)
		: width(width), height(height)
	{
		const size_t size = static_cast<size_t>(width * height);
		if (size > SIZE_MAX) throw std::runtime_error("データサイズが大きすぎます！");

		colors.clear();
		colors.resize(size);

		for (auto& color : colors)
		{
			color = singleColor;
		}
	}

	Image::Image(int width, int height, const Color* rawData)
		: width(width), height(height)
	{
		const size_t size = static_cast<size_t>(width * height);
		if (size > SIZE_MAX) throw std::runtime_error("データサイズが大きすぎます！");

		colors.clear();
		colors.resize(size);

		std::memcpy(colors.data(), rawData, colors.size() * sizeof(Color));
	}
	Image::Image(int width, int height, const std::vector<Color>& data)
		: width(width), height(height)
	{
		const size_t size = static_cast<size_t>(width * height);
		if (size > SIZE_MAX) throw std::runtime_error("データサイズが大きすぎます！");

		colors.clear();
		colors.resize(size);

		std::memcpy(colors.data(), data.data(), colors.size() * sizeof(Color));
	}

	Image operator+(const Image& left, const Image& right)
	{
		if (left.height != right.height) {
			throw std::runtime_error("高さが一致していません");
		}

		int outW = left.width + right.width;
		int outH = left.height;

		std::vector<Color> out(outW * outH, Color(0));

		for (int y = 0; y < outH; y++)
		{
			for (int x = 0; x < left.width; x++)
			{
				size_t leftIndex = static_cast<size_t>(x + outW * y);
				out[leftIndex] = left.getPixel(x, y);
			}
			for (int x = 0; x < right.width; x++)
			{
				size_t rightIndex = static_cast<size_t>((x + left.width) + outW * y);
				out[rightIndex] = right.getPixel(x, y);
			}
		}

		return Image{ outW, outH, out };
	}

	Color Image::getPixel(int x, int y) const
	{
		if (x < 0 || y < 0 || x >= width || y >= height)
		{
			return Color(0, 0);//透明の黒
		}
		return colors[static_cast<size_t>(x + width * y)];
	}

	void Image::setPixel(int x, int y, Color color)
	{
		if (x < 0 || y < 0 || x >= width || y >= height)
		{
			return;
		}
		colors[static_cast<size_t>(x + width * y)] = color;
	}

	Image Image::clipped(int x, int y, int width, int height) const
	{
		if ((x + width) > this->width || (y + height) > this->height)
		{
			throw std::runtime_error("指定したクリップ範囲が範囲外です");
		}

		std::vector<Color> out(width * height, Color(255, 0, 0));

		for (size_t outY = 0; outY < height; outY++)
		{
			for (size_t outX = 0; outX < width; outX++)
			{
				size_t outIndex = outX + width * outY;
				size_t srcIndex = (outX + x) + this->width * (outY + y);
				out[outIndex] = colors[srcIndex];
			}
		}
		out.shrink_to_fit();
		return Image{ width, height, out };
	}

	//マイナスの値を指定すると縮小します←嘘です
	Image Image::scaled(int scale) const
	{
		if (scale < 0)
		{
			throw std::invalid_argument("scaleは正の値でなければなりません");
		}

		Image out;
		out = Image{ width * scale, height * scale };
		Color color;

		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				color = this->getPixel(x, y);
				for (int xs = 0; xs < scale; xs++)
				{
					for (int ys = 0; ys < scale; ys++)
					{
						out.setPixel(x * scale + xs, y * scale + ys, color);
					}
				}
			}
		}

		return out;
	}

	Image Image::swapped(Color target, Color color) const
	{
		Image out = Image(*this);

		for (auto& pixel : out.colors)
		{
			if (pixel == target)
			{
				pixel = color;
			}
		}

		return out;
	}

	static unsigned short sm64rng(unsigned short input)
	{
		if (input == 0x560a)	input = 0;// prevent a two - number loop
		unsigned short s0 = 0, s1 = 0;
		s0 = (input << 8) & 0xffff;
		s0 ^= input;
		input = ((s0 & 0xff) << 8) | ((s0 & 0xff00) >> 8);
		s0 = ((s0 & 0xff) << 1) ^ input;
		s1 = (s0 >> 1) ^ 0xff80;
		if ((s0 & 1) == 0)
			if (s1 == 0xaa55)
				input = 0;// reset cycle at 65, 114th number
			else
				input = s1 ^ 0x1FF4;
		else
			input = s1 ^ 0x8180;
		return input;
	}

	Image Image::filtered(Filter mode) const
	{
		Image out{ *this };

		switch (mode)
		{
		case Filter::GrayScale:
			for (auto& rgb : out.colors)
			{
				int average = (rgb.r + rgb.g + rgb.b) / 3;
				rgb.r = average;
				rgb.g = average;
				rgb.b = average;
			}
			break;

		case Filter::Mosaic:{
			//適切に動いてないかもしれない
			int mosaicBlockSize = 2;
			for (int y = 0; y < out.height / mosaicBlockSize; y++)
			{
				for (int x = 0; x < out.width / mosaicBlockSize; x++)
				{
					int index0 = y * out.width + x;
					int index1 = y * out.width + (x + 1);
					int index2 = (y + 1) * out.width + x;
					int index3 = (y + 1) * out.width + (x + 1);
					ubyte avg_r = (colors[index0].r + colors[index1].r + colors[index2].r + colors[index3].r) / 4;
					ubyte avg_g = (colors[index0].g + colors[index1].g + colors[index2].g + colors[index3].g) / 4;
					ubyte avg_b = (colors[index0].b + colors[index1].b + colors[index2].b + colors[index3].b) / 4;
					ubyte avg_a = (colors[index0].a + colors[index1].a + colors[index2].a + colors[index3].a) / 4;
					out.colors[index0] = Color(avg_r, avg_g, avg_b, avg_a);
					out.colors[index1] = Color(avg_r, avg_g, avg_b, avg_a);
					out.colors[index2] = Color(avg_r, avg_g, avg_b, avg_a);
					out.colors[index3] = Color(avg_r, avg_g, avg_b, avg_a);
				}
			}
		}break;

		case Filter::Invert:
			for (auto& rgb : out.colors)
			{
				rgb.r = 255 - rgb.r;
				rgb.g = 255 - rgb.g;
				rgb.b = 255 - rgb.b;
			}
			break;

		case Filter::Noise:
			ubyte threshold0 = 255;		//色の幅     0 ~ 255
			ubyte threshold1 = 64;		//透明度の幅 0 ~ 255
			unsigned short input0 = 0;	//色用の初期シード     0 ~ 65535
			unsigned short input1 = 1;	//透明度用の初期シード 0 ~ 65535
			for (auto& rgb : out.colors)
			{
				input0 = sm64rng(input0);
				input1 = sm64rng(input1);
				rgb *= Color(input0 % threshold0, input0 % threshold0, input0 % threshold0, 255 - input1 % threshold1);
			}
			break;
		}

		return out;
	}

	Image Image::framed(int px, Color frame) const
	{
		Image out{ *this };

		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				if (x == 0 || y == 0 || x == width - 1 || y == height - 1)
				{
					out.setPixel(x, y, frame);
				}
			}
		}
		return out;
	}

	Image Image::flipV() const
	{
		Image out{ *this };

		for (int y = 0; y < (height >> 1); y++)
		{
			for (int x = 0; x < width; x++)
			{
				Color topColor = out.getPixel(x, y);
				Color bottomColor = out.getPixel(x, height - 1 - y);

				out.setPixel(x, y, bottomColor);
				out.setPixel(x, height - y, topColor);
			}
		}

		return out;
	}

	Image Image::flipH() const
	{
		Image out{ *this };

		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < (width >> 1); x++)
			{
				Color leftColor = out.getPixel(x, y);
				Color rightColor = out.getPixel(width - 1 - x, y);

				out.setPixel(x, y, rightColor);
				out.setPixel(width - x, y, leftColor);
			}
		}

		return out;
	}

	std::vector<Image> Image::divided(int width, int height, int divX, int  divY) const
	{
		std::vector<Image> out;
		for (int y = 0; y < divY; y++)
		{
			for (int x = 0; x < divX; x++)
			{
				out.emplace_back(
					clipped(x * width, y * height, width, height)
				);
			}
		}
		return out;
	}

	size_t Image::size() const
	{
		return colors.size();
	}

	std::vector<Color> Image::data() const
	{
		return colors;
	}

	const Color* Image::rawData() const
	{
		return colors.data();
	}

	void Image::free()
	{
		colors.clear();
		colors.shrink_to_fit();
	}

	void Image::encode(const wstring_view fileName, ImageFormat format, bool overWrite = false) const
	{
		if (!overWrite)
		{
			return;
		}

		int result = 0;
		switch (format)
		{
		case ImageFormat::PNG:
			result = stbi_write_png(String::MultiByteStr(fileName).c_str(), width, height, sizeof(Color), colors.data(), 0);
			break;
		case ImageFormat::JPEG:
			result = stbi_write_jpg(String::MultiByteStr(fileName).c_str(), width, height, sizeof(Color), colors.data(), 0);
			break;
		case ImageFormat::BMP:
			result = stbi_write_bmp(String::MultiByteStr(fileName).c_str(), width, height, sizeof(Color), colors.data());
			break;
		case ImageFormat::TGA:
			result = stbi_write_tga(String::MultiByteStr(fileName).c_str(), width, height, sizeof(Color), colors.data());
			break;
		}

		if (!result)
		{
			throw std::runtime_error("画像ファイルの保存に失敗しました");
		}
	}
	//class Image end
}
#pragma warning(pop)