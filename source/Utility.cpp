#pragma warning(push)
#pragma warning(disable: 6262)//関数はスタックの '35036' バイトを使用します。データの一部をヒープに移動することを考慮してください。
#pragma warning(disable: 26819)//switch ラベルどうしの間に、注釈の付いていないフォールスルーがあります (es.78)。
#define STB_IMAGE_IMPLEMENTATION
#include"../include/third_party/stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include"../include/third_party/stb/stb_image_write.h"
#pragma warning(pop)
#include"../include/Utility.h"

namespace Win32
{
	//struct Color begin
	Color::Color(ubyte r, ubyte g, ubyte b) : 
		r(std::min<ubyte>(r, 0xff)),
		g(std::min<ubyte>(g, 0xff)),
		b(std::min<ubyte>(b, 0xff)),
		a(std::min<ubyte>(0xff, 0xff))
	{
	}
	Color::Color(ubyte r, ubyte g, ubyte b, ubyte a) :
		r(std::min<ubyte>(r, 0xff)),
		g(std::min<ubyte>(g, 0xff)),
		b(std::min<ubyte>(b, 0xff)),
		a(std::min<ubyte>(a, 0xff))
	{
	}
	Color::Color(ubyte gs) :
		r(std::min<ubyte>(gs, 0xff)),
		g(std::min<ubyte>(gs, 0xff)),
		b(std::min<ubyte>(gs, 0xff)),
		a(std::min<ubyte>(0xff, 0xff))
	{
	}
	Color::Color(ubyte gs, ubyte alpha) : 
		r(std::min<ubyte>(gs, 0xff)),
		g(std::min<ubyte>(gs, 0xff)),
		b(std::min<ubyte>(gs, 0xff)),
		a(std::min<ubyte>(alpha, 0xff))
	{
	}
	Color::Color(const wstring& hex)
	{
		wstring outHex;
		outHex.resize(hex.size());
		for (int i = 0; i < hex.size(); i++)
		{
			outHex[i] = std::tolower(hex[i]);
		}

		ubyte rgb[3]{};
		for (int i = 0; i < 3; i++)
		{
			rgb[i] = std::stoi(hex.substr(1 + i * 2, 2), nullptr, 16);
		}

		this->r = rgb[0];
		this->g = rgb[1];
		this->b = rgb[2];
		this->a = 0xff;
	}

	Color operator+(const Color& base, const Color& blend)
	{
		return Color{
			std::min<ubyte>(255, base.r + blend.r),
			std::min<ubyte>(255, base.g + blend.g),
			std::min<ubyte>(255, base.b + blend.b),
			std::min<ubyte>(255, base.a + blend.a)
		};
	}
	Color operator-(const Color& base, const Color& blend)
	{
		return Color{
			std::max<ubyte>(0, base.r - blend.r),
			std::max<ubyte>(0, base.g - blend.g),
			std::max<ubyte>(0, base.b - blend.b),
			std::max<ubyte>(0, base.a - blend.a)
		};
	}
	Color operator*(const Color& base, const Color& blend)
	{
		return Color{};
	}
	std::ostream& operator<<(std::ostream& os, const Color& color)
	{
		os << "R:" << (int)color.r
			<< ", G:" << (int)color.g
			<< ", B:" << (int)color.b
			<< ", A:" << (int)color.a;
		return os;
	}
	bool operator==(const Color& left, const Color& right)
	{
		return ((left.r == right.r) && (left.g == right.g) && (left.b == right.b) && (left.a == right.a));
	}

	Color Color::operator+=(const Color& blend)
	{
		return Color{ r, g, b, a } + blend;
	}
	Color Color::operator-=(const Color& blend)
	{
		return Color{ r, g, b, a } - blend;
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

		unsigned char* pixels = stbi_load(Convert::MultiByteStr(path).c_str(), &width, &height, &channels, sizeof(Color));
		std::cout << Convert::MultiByteStr(path).c_str() << std::endl;
		if (!std::filesystem::exists(path))
		{
			throw std::runtime_error(std::format("ファイルが存在しません \n path:{}", Convert::MultiByteStr(path).c_str()));
		}
		else if (!pixels)
		{
			throw std::runtime_error("画像データが破損しています");
		}
		std::cout << "Image:Width " << width << "\n";
		std::cout << "Image:Height " << height << "\n";

		const size_t size = static_cast<size_t>(width * height);
		if (size > SIZE_MAX) throw std::runtime_error("データサイズが大きすぎます！");

		colors.clear();
		colors.resize(size);
		std::memcpy(colors.data(), pixels, size * sizeof(Color));
//		std::memcpy(colors.data(), pixels, size);

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
		size_t index = static_cast<size_t>(x + this->width * y);
		if (index >= colors.size() || x < 0 || y < 0)
		{
			return Color(255, 0);
		}
		return colors[index];
	}

	void Image::setPixel(int x, int y, const Color& color)
	{
		size_t index = static_cast<size_t>(x + this->width * y);
		if (index >= colors.size() || x < 0 || y < 0)
		{
			return;
		}
		colors[index] = color;
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

	Image Image::scaled(int scale) const
	{
		if (scale <= 0)
		{
			throw std::runtime_error("scaleに負の値を指定しないで下さい");
		}

		Image out{ width * scale, height * scale };
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

	Image Image::swapped(const Color& target, const Color& color) const
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

		case Filter::Mosaic:
			break;

		case Filter::Invert:
			for (auto& rgb : out.colors)
			{
				rgb.r = 255 - rgb.r;
				rgb.g = 255 - rgb.g;
				rgb.b = 255 - rgb.b;
			}
			break;

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
				Color bottomColor = out.getPixel(x, height - y);

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
				Color rightColor = out.getPixel(width - x, y);

				out.setPixel(x, y, rightColor);
				out.setPixel(width - x, y, leftColor);
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

	void Image::blendAddition(const Color& blend)
	{
		for (auto& pixel : colors)
		{
			pixel += blend;
		}
	}

	void Image::encodePNG(const wstring_view fileName) const
	{
		if (!std::filesystem::exists(fileName))
		{
			stbi_write_png(Convert::MultiByteStr(fileName).c_str(), width, height, sizeof(Color), colors.data(), 0);
		}
	}

	void Image::encodePNG(const wstring_view fileName, bool overWrite) const
	{
		if (overWrite)
		{
			stbi_write_png(Convert::MultiByteStr(fileName).c_str(), width, height, sizeof(Color), colors.data(), 0);
		}
	}

	void Image::encodeJPG(const wstring_view fileName) const
	{
		if (!std::filesystem::exists(fileName))
		{
			stbi_write_jpg(Convert::MultiByteStr(fileName).c_str(), width, height, sizeof(Color), colors.data(), 0);
		}
	}

	void Image::encodeJPG(const wstring_view fileName, bool overWrite) const
	{
		if (overWrite)
		{
			stbi_write_jpg(Convert::MultiByteStr(fileName).c_str(), width, height, sizeof(Color), colors.data(), 0);
		}
	}

	void Image::encodeBMP(const wstring_view fileName) const
	{
		if (!std::filesystem::exists(fileName))
		{
			stbi_write_bmp(Convert::MultiByteStr(fileName).c_str(), width, height, sizeof(Color), colors.data());
		}
	}

	void Image::encodeBMP(const wstring_view fileName, bool overWrite) const
	{
		if (overWrite)
		{
			stbi_write_bmp(Convert::MultiByteStr(fileName).c_str(), width, height, sizeof(Color), colors.data());
		}
	}

	void Image::encodeTGA(const wstring_view fileName) const
	{
		if (!std::filesystem::exists(fileName))
		{
			stbi_write_tga(Convert::MultiByteStr(fileName).c_str(), width, height, sizeof(Color), colors.data());
		}
	}

	void Image::encodeTGA(const wstring_view fileName, bool overWrite) const
	{
		if (overWrite)
		{
			stbi_write_tga(Convert::MultiByteStr(fileName).c_str(), width, height, sizeof(Color), colors.data());
		}
	}
	//class Image end
}