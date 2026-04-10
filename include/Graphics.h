#pragma once
#include"Macros.h"
#include"Helper.h"
#include"Utility.h"

// DirectX 11
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#include<d3d11.h>
#include<dxgi1_6.h>
#include<d3dcompiler.h>
#include<DirectXMath.h>
#include<iostream>
#include<vector>
#include<cmath>
#include<wrl.h>
#include<unordered_map>
using namespace DirectX;
using Microsoft::WRL::ComPtr;

namespace Win32
{
	// x,y,z座標のエイリアスです
	using Vertex = DirectX::XMFLOAT3;
	// インデックスバッファ用のエイリアスです
	using Index = unsigned int;
	//UV座標用のエイリアスです
	using UV = DirectX::XMFLOAT2;

	using std::wstring;

	namespace Core
	{
		static ComPtr<ID3D11Device> device = nullptr;
		static ComPtr<ID3D11DeviceContext> context = nullptr;
		static ComPtr<ID3D11Debug> debug = nullptr;
		static ComPtr<IDXGISwapChain1> swapChain = nullptr;
		static ComPtr<ID3D11RenderTargetView> backBufferView = nullptr;
		static ComPtr<ID3D11InputLayout> inputLayout = nullptr;
		static ComPtr<ID3D11VertexShader> vShader = nullptr;
		static ComPtr<ID3D11PixelShader> pShader = nullptr;
		static ComPtr<ID3D11RasterizerState> rasterizerState = nullptr;
		static D3D11_VIEWPORT viewPort{};
		static ComPtr<ID3D11Buffer> matrixConstantBuffer = nullptr;
		static ComPtr<ID3D11Buffer> diffuseConstantBuffer = nullptr;
		static ComPtr<ID3D11Buffer> circleDataConstantBuffer = nullptr;
		static ComPtr<ID3D11Buffer> drawTypeConstantBuffer = nullptr;
		static ComPtr<ID3D11SamplerState> samplerState = nullptr;
		static ComPtr<ID3D11BlendState> blendState = nullptr;
		static Color swapChainClearColor{ 128, 128, 255 };

		static std::unordered_map<wchar_t, Image> textMap{};

		enum class CommandType : int
		{
			NONE,
			SHAPE,
			TEXTURE,
			LINE,
		};

		struct CircleData
		{
			XMFLOAT2 pos{};
			float radius;
			float edge;
		};
		struct Float2Size
		{
			float w;
			float h;
		};
		struct Int2Size
		{
			int w;
			int h;
		};
		struct DrawCommand
		{
			ComPtr<ID3D11Buffer> vertexBuffer = nullptr;
			ComPtr<ID3D11Buffer> indexBuffer = nullptr;
			ComPtr<ID3D11Buffer> texcoordBuffer;
			ComPtr<ID3D11ShaderResourceView> srv;

			CircleData cd{};
			CommandType type = CommandType::NONE;
			unsigned int indexCount = 0;

			float scale = 1.0f;
			float angleX = 0.0f;
			float angleY = 0.0f;
			float angleZ = 0.0f;
			Color rgba{ 255, 255, 255 };
			XMFLOAT3 pos{ 0.0f, 0.0f, 1.0f };
			Float2Size size{};

			bool isUpdate = false;

			bool update() const
			{
				return this->isUpdate;
			}
			void runUpdate()
			{
				this->isUpdate = true;
			}
			void endUpdate()
			{
				this->isUpdate = false;
			}
		};

		class BShape
		{
		protected:
			DrawCommand command{};
			ComPtr<ID3D11Buffer> createBuffer(const D3D11_BUFFER_DESC* desc, const D3D11_SUBRESOURCE_DATA* data);
			ComPtr<ID3D11Buffer> createVertexBuffer(const Vertex* vtxs, size_t size);
			ComPtr<ID3D11Buffer> createTexcoordBuffer(const UV* uvs, size_t size);
			ComPtr<ID3D11Buffer> createIndexBuffer(const Index* idxs, size_t size);
			ComPtr<ID3D11Texture2D> createTexture(const Image& image);
			ComPtr<ID3D11Texture2D> createTexture(int width, int height, const std::vector<Color>& pixels);
			ComPtr<ID3D11Texture2D> createTexture(const D3D11_TEXTURE2D_DESC* desc, const D3D11_SUBRESOURCE_DATA* data);
			ComPtr<ID3D11Texture2D> createDynamicTexture(const Image& image);
			ComPtr<ID3D11ShaderResourceView> createShaderResourceView(ID3D11Texture2D* tex, const D3D11_SHADER_RESOURCE_VIEW_DESC* desc);

		public:
			BShape() = default;
			BShape(const BShape&) = default;
			BShape(const DrawCommand&);
			~BShape();

			bool intersect(const DrawCommand& cmd);
			BShape& position(float x, float y);
			BShape& color(ubyte r, ubyte g, ubyte b);
			BShape& color(ubyte r, ubyte g, ubyte b, ubyte a);
			BShape& color(const Color& color);
			BShape& rotationX(float r);
			BShape& rotationY(float r);
			BShape& rotationZ(float r);
			BShape& scaled(float scale);
			DrawCommand get() const;
		};

		bool UpdateConstantBuffer(ID3D11Resource* buf, const void* data, size_t size);
	}

	class Rect SEALED : public Core::BShape
	{
	public:
		Rect() = default;
		Rect(const Rect&) = default;
		Rect(int width, int height);
	};

	class Line SEALED : public Core::BShape
	{
	public:
		//未使用
		Line(int startX, int startY, int endX, int endY);
	};

	class Circle SEALED : public Core::BShape
	{
	public:
		Circle() = default;
		Circle(const Circle&) = default;
		Circle(int radius);
		Circle(int radius, int segment);
	};

	class Texture SEALED : public Core::BShape
	{
	public:
		Texture() = default;
		Texture(int width, int height, std::vector<Color> pixels);
		Texture(const Image& image);
	};



	class Text
	{
	private:
		inline static std::unordered_map<wchar_t, wchar_t> dakutenTable{
			{ L'が', L'か' },
			{ L'ぎ', L'き' },
			{ L'ぐ', L'く' },
			{ L'げ', L'け' },
			{ L'ご', L'こ' },

			{ L'ざ', L'さ' },
			{ L'じ', L'し' },
			{ L'ず', L'す' },
			{ L'ぜ', L'せ' },
			{ L'ぞ', L'そ' },

			{ L'だ', L'た' },
			{ L'ぢ', L'ち' },
			{ L'づ', L'つ' },
			{ L'で', L'て' },
			{ L'ど', L'と' },

			{ L'ば', L'は' },
			{ L'び', L'ひ' },
			{ L'ぶ', L'ふ' },
			{ L'べ', L'へ' },
			{ L'ぼ', L'ほ' },
		};
		inline static std::unordered_map<wchar_t, wchar_t> handakutenTable{
			{ L'ぱ', L'は' },
			{ L'ぴ', L'ひ' },
			{ L'ぷ', L'ふ' },
			{ L'ぺ', L'へ' },
			{ L'ぽ', L'ほ' },
		};
		inline static std::unordered_map<wchar_t, std::wstring> symbolTable{
			{ L'!',		L"exclamationMark" },
			{ L'！',	L"exclamationMark" },
			{ L'?',		L"questionMark" },
			{ L'？',	L"questionMark" },
			{ L'#',		L"hash"},
			{ L'＃',	L"hash"},
			{ L'$',		L"doller" },
			{ L'＄',	L"doller"},
			{ L'%',		L"percent"},
			{ L'％',	L"percent"},
			{ L'&',		L"ampersand" },
			{ L'＆',	L"ampersand" },
			{ L'\'',	L"apostrophe" },
			{ L'’',	L"apostrophe" },
			{ L'(',		L"leftRoundBrackets" },
			{ L'（',	L"leftRoundBrackets" },
			{ L')',		L"rightRoundBrackets" },
			{ L'）',	L"rightRoundBrackets" },
			{ L'*',		L"asterisk" },
			{ L'＊',	L"asterisk" },
			{ L'+',		L"plus" },
			{ L'＋',	L"plus" },
			{ L'-',		L"minus" },
			{ L',',		L"comma" },
			{ L'，',	L"comma" },
			{ L':',		L"colon" },
			{ L'：',	L"colon" },
			{ L';',		L"semi-colon" },
			{ L'；',	L"semi-colon" },
			{ L'[',		L"leftSquareBrackets" },
			{ L']',		L"rightSquareBrackets" },
			{ L'\\',	L"yen" },
			{ L'￥',	L"yen" },
			{ L'^',		L"hat" },
			{ L'＾',	L"hat" },
			{ L'_',		L"unserscore" },
			{ L'＿',	L"unserscore" },
			{ L'{',		L"leftCurlyBrackets" },
			{ L'｛',	L"leftCurlyBrackets" },
			{ L'}',		L"rightCurlyBrackets" },
			{ L'｝',	L"rightCurlyBrackets" },
			{ L'|',		L"pipe" },
			{ L'｜',	L"pipe" },
			{ L'~',		L"tilde" },
			{ L'～',	L"tilde" },
			{ L'.',		L"period" },//ぴりおど
			{ L'/',		L"slash" },//スラッシュ
			{ L'<',		L"smaller" },
			{ L'>',		L"big" },
		};
		inline static std::unordered_map<wchar_t, std::wstring> lowerTable{
			{ L'a', L"al" },
			{ L'b', L"bl" },
			{ L'c', L"cl" },
			{ L'd', L"dl" },
			{ L'e', L"el" },
			{ L'f', L"fl" },
			{ L'g', L"gl" },
			{ L'h', L"hl" },
			{ L'i', L"il" },
			{ L'j', L"jl" },
			{ L'k', L"kl" },
			{ L'l', L"ll" },
			{ L'm', L"ml" },
			{ L'n', L"nl" },
			{ L'o', L"ol" },
			{ L'p', L"pl" },
			{ L'q', L"ql" },
			{ L'r', L"rl" },
			{ L's', L"sl" },
			{ L't', L"tl" },
			{ L'u', L"ul" },
			{ L'v', L"vl" },
			{ L'w', L"wl" },
			{ L'x', L"xl" },
			{ L'y', L"yl" },
			{ L'z', L"zl" },
		};

		enum MojiType {
			NORMAL,
			DAKUTEN,
			HANDAKUTEN,
		};
		static std::pair<std::wstring, MojiType> getCharImagePath(wchar_t wch);
	public:
		static Image GetTextImage(const wstring& text);
	};

	class GraphicsXI
	{
	private:
		enum
		{
			SingleBuffer = 1,
			DoubleBuffer = 2,
			TripleBuffer = 3,
		};

		HRESULT Hr;
		std::vector<Core::DrawCommand> cmds;
		bool isUpdate = true;

		LARGE_INTEGER lint{};

	public:
		GraphicsXI() = default;
		GraphicsXI(const GraphicsXI&) = default;
		GraphicsXI(HWND parentHdl, HINSTANCE parentIns);

		static void SetClearColor(const Color& color);

		void clear();
		void firstSetting();
		void present();
		void draw(const Core::DrawCommand& cmd);
		const size_t drawCount() const;
	};
}