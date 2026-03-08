#pragma once
#include<vector>//配列
#include<stdexcept>//例外
#include<string>//文字列
#include<iostream>//出力
#include<format>//テキスト整形
#include<filesystem>//存在確認
#include<DirectXMath.h>//XMFLOAT4
#include"Helper.h"

namespace Win32
{
	//RGBA値のエイリアスです
//	using ubyte = unsigned char;

	using DirectX::XMFLOAT4;
	using std::wstring;
	using std::wstring_view;
	using ubyte = unsigned char;
	constexpr float inv255 = 1.0f / 255.0f;

	struct Color
	{
	public:
		ubyte r;
		ubyte g;
		ubyte b;
		ubyte a;

		Color() = default;
		Color(const Color&) = default;
		Color(ubyte r, ubyte g, ubyte b);
		Color(ubyte r, ubyte g, ubyte b, ubyte a);
		Color(ubyte gs);
		Color(ubyte gs, ubyte alpha);
		Color(const wstring& hex);

		friend Color operator+(const Color& base, const Color& blend);
		friend Color operator-(const Color& base, const Color& blend);
		friend Color operator*(const Color& base, const Color& blend);
		friend std::ostream& operator<<(std::ostream& os, const Color& color);
		Color operator+=(const Color& blend);
		Color operator-=(const Color& blend);
		XMFLOAT4 floats() const;
		static Color IntHEX(unsigned int hex);
	};

	class Image
	{
	private:
		//MEMO
		/*
		* widthとかheightもprivateにしたいけど、関数名と被るのでpublicにしてます
		*
		* 大文字始まりの関数にすれば被らないけど、
		* 大文字始まりの関数はstatic関数ということにしてるのでやめました
		*
		* 今の所Textureクラスと組み合わせて使っているけど正直面倒
		* Imageはあくまで画像データを保持するものとして使った方がいいかも
		* Textureクラスの色データをImageクラスとして返す関数もあったらうれしい
		*
		* 色のブレンドとかはTextureで出来たら楽だけど
		* Textureでは色データを保持していないので
		* Imageでやらなければならない気がする
		* それこそ全体の色なら保持しているのでシェーダーでブレンドしてもいいかも
		* 乗算ならシェーダーでDiffuseを掛ければ良いだけだしね
		*/

	public:
		int width = 0;
		int height = 0;
		int channels = sizeof(Color);
		std::vector<Color> colors{};

		Image();
		//画像のパスから読み込みます
		Image(wstring_view path);
		//大きさのみ設定します
		Image(int width, int height);
		//生の色データから読み込みます
		Image(int width, int height, const Color* rawData);
		//色データから読み込みます
		Image(int width, int height, const std::vector<Color>& data);

		friend Image operator+(const Image& left, const Image& right);

		Color getPixel(int x, int y) const;

		void setPixel(int x, int y, const Color& color);

		Image clip(int x, int y, int width, int height) const;

		Image scaled(int scale) const;

		size_t size() const;

		std::vector<Color> data() const;

		void blendAddition(const Color& blend);
	};
}

/*
namespace Win32
{
	//RGBA値のエイリアスです
	using ubyte = unsigned char;

	using DirectX::XMFLOAT4;
	constexpr float inv255 = 1.0f / 255.0f;

	struct Color
	{
	public:
		ubyte r;
		ubyte g;
		ubyte b;
		ubyte a;

		Color() = default;
		Color(const Color&) = default;
		Color(ubyte r, ubyte g, ubyte b, ubyte a = 0xff) :
			r(std::min<ubyte>(r, 0xff)),
			g(std::min<ubyte>(g, 0xff)),
			b(std::min<ubyte>(b, 0xff)),
			a(std::min<ubyte>(a, 0xff)) {
		}

		Color(ubyte gs) :
			r(std::min<ubyte>(gs, 0xff)),
			g(std::min<ubyte>(gs, 0xff)),
			b(std::min<ubyte>(gs, 0xff)),
			a(0xff) {
		}

		Color(ubyte gs, ubyte alpha) :
			r(std::min<ubyte>(gs, 0xff)),
			g(std::min<ubyte>(gs, 0xff)),
			b(std::min<ubyte>(gs, 0xff)),
			a(std::min<ubyte>(alpha, 0xff)) {
		}

		Color(const std::wstring& hex)
		{
			std::wstring outHex;
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

		friend Color operator+(const Color& base, const Color& blend)
		{
			return Color{
				std::min<ubyte>(255, base.r + blend.r),
				std::min<ubyte>(255, base.g + blend.g),
				std::min<ubyte>(255, base.b + blend.b),
				std::min<ubyte>(255, base.a + blend.a)
			};
		}
		friend Color operator-(const Color& base, const Color& blend)
		{
			return Color{
				std::max<ubyte>(0, base.r - blend.r),
				std::max<ubyte>(0, base.g - blend.g),
				std::max<ubyte>(0, base.b - blend.b),
				std::max<ubyte>(0, base.a - blend.a)
			};
		}
		friend Color operator*(const Color& base, const Color& blend)
		{
			return Color{};
		}
		friend std::ostream& operator<<(std::ostream& os, const Color& color)
		{
			os << "R:" << (int)color.r
				<< ", G:" << (int)color.g
				<< ", B:" << (int)color.b
				<< ", A:" << (int)color.a;
			return os;
		}
		Color operator+=(const Color& blend)
		{
			return Color{ r, g, b, a } + blend;
		}
		Color operator-=(const Color& blend)
		{
			return Color{ r, g, b, a } - blend;
		}

		XMFLOAT4 floats() const
		{
			return{
				static_cast<float>(r) * inv255,
				static_cast<float>(g) * inv255,
				static_cast<float>(b) * inv255,
				static_cast<float>(a) * inv255
			};
		}

		static Color IntHEX(unsigned int hex)
		{
			ubyte r = (hex >> 8 * 3) & 0xff;
			ubyte g = (hex >> 8 * 2) & 0xff;
			ubyte b = (hex >> 8 * 1) & 0xff;
			ubyte a = hex & 0xff;

			return Color(r, g, b, a);
		}
	};

	class Image
	{
	private:
		//MEMO
		/*
		* widthとかheightもprivateにしたいけど、関数名と被るのでpublicにしてます
		*
		* 大文字始まりの関数にすれば被らないけど、
		* 大文字始まりの関数はstatic関数ということにしてるのでやめました
		*
		* 今の所Textureクラスと組み合わせて使っているけど正直面倒
		* Imageはあくまで画像データを保持するものとして使った方がいいかも
		* Textureクラスの色データをImageクラスとして返す関数もあったらうれしい
		*
		* 色のブレンドとかはTextureで出来たら楽だけど
		* Textureでは色データを保持していないので
		* Imageでやらなければならない気がする
		* それこそ全体の色なら保持しているのでシェーダーでブレンドしてもいいかも
		* 乗算ならシェーダーでDiffuseを掛ければ良いだけだしね

	public:
		int width = 0;
		int height = 0;
		int channels = sizeof(Color);
		std::vector<Color> colors{};

		Image() = default;
		//画像のパスから読み込みます
		Image(const std::wstring& path)
		{
			unsigned char* pixels = stbi_load(Convert::MultiByteStr(path).c_str(), &width, &height, &channels, NULL);
			std::cout << Convert::MultiByteStr(path).c_str() << std::endl;
			if (!std::filesystem::exists(path))
			{
				throw std::runtime_error(std::format("ファイルが存在しません \n path:{}", Convert::MultiByteStr(path).c_str()));
			}
			else if (!pixels)
			{
				throw std::runtime_error("画像データが破損しています");
			}

			const size_t size = static_cast<size_t>(width * height);
			if (size > SIZE_MAX) throw std::runtime_error("データサイズが大きすぎます！");

			colors.clear();
			colors.resize(size);
			std::memcpy(colors.data(), pixels, size * sizeof(Color));

			stbi_image_free(pixels);
		}
		//生の色データから読み込みます
		Image(int width, int height, const Color* rawData)
			: width(width), height(height)
		{
			const size_t size = static_cast<size_t>(width * height);
			if (size > SIZE_MAX) throw std::runtime_error("データサイズが大きすぎます！");

			colors.clear();
			colors.resize(size);

			std::memcpy(colors.data(), rawData, colors.size() * sizeof(Color));
		}
		//色データから読み込みます
		Image(int width, int height, const std::vector<Color>& data)
			: width(width), height(height)
		{
			const size_t size = static_cast<size_t>(width * height);
			if (size > SIZE_MAX) throw std::runtime_error("データサイズが大きすぎます！");

			colors.clear();
			colors.resize(size);

			std::memcpy(colors.data(), data.data(), colors.size() * sizeof(Color));
		}

		friend Image operator+(const Image& left, const Image& right)
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

		Color getPixel(int x, int y) const
		{
			size_t index = static_cast<size_t>(x + this->width * y);
			if (index >= colors.size() || x < 0 || y < 0)
			{
				return Color(255);
			}
			return colors[index];
		}

		void setPixel(int x, int y, const Color& color)
		{
			size_t index = static_cast<size_t>(x + this->width * y);
			if (index >= colors.size() || x < 0 || y < 0)
			{
				return;
			}
			colors[index] = color;
		}

		Image clip(int x, int y, int width, int height) const
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

		size_t size() const
		{
			return colors.size();
		}

		std::vector<Color> data() const
		{
			return colors;
		}

		void BlendAddition(const Color& blend)
		{
			for (auto& pixel : colors)
			{
				pixel += blend;
			}
		}
	};
}
*/