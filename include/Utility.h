#pragma once
#include<vector>//配列
#include<stdexcept>//例外
#include<string>//文字列
#include<iostream>//出力
#include<format>//テキスト整形
#include<filesystem>//存在確認
#include<DirectXMath.h>//XMFLOAT4
#include<random>//乱数
#include"Helper.h"

namespace Win32
{
	using DirectX::XMFLOAT4;
	using std::wstring;
	using std::wstring_view;
	using ubyte = unsigned char;
	constexpr float inv255 = 1.0f / 255.0f;

	enum class Filter
	{
		GrayScale,
		Mosaic,
		Invert,
		Noise
	};

	enum class ImageFormat
	{
		PNG,
		JPEG,
		BMP,
		TGA
	};

	struct Color
	{
	public:
		ubyte r;
		ubyte g;
		ubyte b;
		ubyte a;

		Color();
		Color(const Color&) = default;
		Color(ubyte r, ubyte g, ubyte b);
		Color(ubyte r, ubyte g, ubyte b, ubyte a);
		Color(ubyte gs);
		Color(ubyte gs, ubyte alpha);
		Color(const wstring& hex);

		friend Color operator+(Color base, Color blend);
		friend Color operator-(Color base, Color blend);
		friend Color operator*(Color base, Color blend);
		Color operator+=(Color blend);
		Color operator-=(Color blend);
		Color operator*=(Color blend);
		friend bool operator==(Color left, Color right);
		friend std::ostream& operator<<(std::ostream& os, Color color);
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

		void setPixel(int x, int y, Color color);

		Image clipped(int x, int y, int width, int height) const;

		Image scaled(int scale) const;

		Image swapped(Color target, Color color) const;

		Image filtered(Filter mode) const;

		Image framed(int px, Color frame)const;

		Image flipV() const;

		Image flipH() const;

		std::vector<Image> divided(int width, int height, int divX, int  divY) const;

		size_t size() const;

		std::vector<Color> data() const;

		const Color* rawData() const;

		void free();

		void encode(const wstring_view fileName, ImageFormat format, bool overWrite) const;
	};
}