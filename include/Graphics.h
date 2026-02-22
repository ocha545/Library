// 大きさがマイナスの処理やる
#pragma once
#define UsingGraphics Win32_CPP::Graphics
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
#include<Shellapi.h>
#include<iostream>
#include<vector>
#include<cmath>
#include<wrl.h>
#include<unordered_map>
#include<mutex>
using namespace DirectX;
using namespace Microsoft::WRL;

namespace Win32_CPP
{
	// x,y,z座標のエイリアスです
	using Vertex = DirectX::XMFLOAT3;
	// インデックスバッファ用のエイリアスです
	using Index = unsigned int;
	//UV座標用のエイリアスです
	using UV = DirectX::XMFLOAT2;

	namespace Graphics
	{
		//中身見ないで！頼むから！
		namespace Core
		{
			static ComPtr<ID3D11Device> device = nullptr;
			static ComPtr<ID3D11DeviceContext> context = nullptr;
			static ComPtr<ID3D11Debug> debug = nullptr;
			static ComPtr<IDXGISwapChain1> swapChain = nullptr;
			static ComPtr<ID3D11RenderTargetView> backBufferView = nullptr;
			//パイプライン
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

			enum class CommandType : int
			{
				NONE,
				SHAPE,
				TEXTURE,
			};

			typedef struct CircleData
			{
				XMFLOAT2 pos{};
				float radius;
				float edge;
			}CircleData;

			typedef struct Float2Size
			{
				float w;
				float h;
			}Float2Size;

			typedef struct Int2Size
			{
				int w;
				int h;
			}Int2Size;

			typedef struct DrawCommand
			{
			public:
				ComPtr<ID3D11Buffer> vertexBuffer = nullptr;
				ComPtr<ID3D11Buffer> texcoordBuffer = nullptr;
				ComPtr<ID3D11Buffer> indexBuffer = nullptr;
				ComPtr<ID3D11ShaderResourceView> srv = nullptr;
				CommandType type = CommandType::NONE;
				unsigned int indexCount = 0;
				float scale = 1.0f;
				float angleX = 0.0f;
				float angleY = 0.0f;
				float angleZ = 0.0f;
				CircleData cd{};
				Color rgba{ 255, 255, 255 };
				XMFLOAT3 pos{0.0f, 0.0f, 1.0f};
				Float2Size size{};
			}DrawCommand;

			//基底クラスなのでBaseの頭文字を取って付けました
			//インターフェイスじゃないからIShapeは違うもんね～
			//使い方は定義を見てね
			class BShape
			{
				/*
				図形を作成するには、頂点とインデックスを作成する必要があります
				基本的にこの順番で作成します
				※Create Texcoord BufferとWrite Textureはテクスチャを使わなければ、作成する必要は有りません！
				┌───────────┐
				│Create Vertex Buffer  │
				└─────┬─────┘
							│
           					│
				┌─────▼─────┐
				│Create Index Buffer   │
				└─────┬─────┘
						    │
						    │
				┌─────▼─────┐
				│Create Texcoord Buffer│
				└─────┬─────┘
						    │
						    │
				┌─────▼─────┐
				│Write Texture         │
				└───────────┘
				*/

			protected:
				DrawCommand command{};

				ComPtr<ID3D11Buffer> createBuffer(const D3D11_BUFFER_DESC* desc, const D3D11_SUBRESOURCE_DATA* data)
				{
					ComPtr<ID3D11Buffer> tmp = nullptr;
					HRESULT Hr = Core::device->CreateBuffer(desc, data, tmp.GetAddressOf());
					assert(SUCCEEDED(Hr));
					AST_NULL(tmp.Get());
					return tmp;
				}

				ComPtr<ID3D11Buffer> createVertexBuffer(const Vertex* vts, size_t size)
				{
					D3D11_BUFFER_DESC desc{};
					desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
					desc.Usage = D3D11_USAGE_DEFAULT;
					desc.ByteWidth = size;

					D3D11_SUBRESOURCE_DATA data{};
					data.pSysMem = vts;

					return createBuffer(&desc, &data);
				}

				ComPtr<ID3D11Buffer> createTexcoordBuffer(const UV* uvs, size_t size)
				{
					D3D11_BUFFER_DESC desc{};
					desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
					desc.Usage = D3D11_USAGE_DEFAULT;
					desc.ByteWidth = size;

					D3D11_SUBRESOURCE_DATA data{};
					data.pSysMem = uvs;

					return createBuffer(&desc, &data);
				}

				ComPtr<ID3D11Buffer> createIndexBuffer(const Index* idxs, size_t size)
				{
					D3D11_BUFFER_DESC desc{};
					desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
					desc.Usage = D3D11_USAGE_DEFAULT;
					desc.ByteWidth = size;

					D3D11_SUBRESOURCE_DATA data{};
					data.pSysMem = idxs;

					return createBuffer(&desc, &data);
				}

				ComPtr<ID3D11Texture2D> createTexture(int width, int height, const std::vector<Color>& pixels)
				{
					D3D11_TEXTURE2D_DESC desc{};
					desc.Width = width;
					desc.Height = height;
					desc.MipLevels = 1;
					desc.ArraySize = 1;
					desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
					desc.SampleDesc.Count = 1;
					desc.Usage = D3D11_USAGE_DEFAULT;
					desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
					desc.CPUAccessFlags = 0;
					desc.MiscFlags = 0;

					D3D11_SUBRESOURCE_DATA data{};
					data.pSysMem = pixels.data();
					data.SysMemPitch = width * sizeof(Color);

					ComPtr<ID3D11Texture2D> tmp = nullptr;
					HRESULT Hr = Core::device->CreateTexture2D(&desc, &data, tmp.GetAddressOf());
					return tmp;
				}

				ComPtr<ID3D11Texture2D> createTexture(const D3D11_TEXTURE2D_DESC* desc, const D3D11_SUBRESOURCE_DATA* data)
				{
					ComPtr<ID3D11Texture2D> tex;
					HRESULT Hr = Core::device->CreateTexture2D(desc, data, tex.GetAddressOf());
					assert(SUCCEEDED(Hr));
					AST_NULL(tex);
					return tex;
				}

				ComPtr<ID3D11ShaderResourceView> createShaderResourceView(ID3D11Texture2D* tex, const D3D11_SHADER_RESOURCE_VIEW_DESC* desc)
				{
					ComPtr<ID3D11ShaderResourceView> srv;
					HRESULT Hr = Core::device->CreateShaderResourceView(tex, desc, srv.GetAddressOf());
					assert(SUCCEEDED(Hr));
					AST_NULL(srv);
					return srv;
				}

			public:
				BShape() = default;
				BShape(const BShape&) = default;
				BShape(const DrawCommand& cmd) : command(cmd)
				{
				}

				virtual ~BShape()
				{
					command.indexBuffer.Reset();
					command.vertexBuffer.Reset();
				}

				bool intersect(const DrawCommand& s) const
				{
					const DrawCommand& c = command;
					return(
						c.pos.x < s.pos.x + s.size.w &&
						s.pos.x < c.pos.x + c.size.w &&
						c.pos.y < s.pos.y + s.size.h &&
						s.pos.y < c.pos.y + c.size.h
					);
				}

				BShape& position(float x, float y)
				{
					command.pos = { x, y, 1.0f };
					return *this;
				}

				BShape& color(ubyte r, ubyte g, ubyte b, ubyte a = 255)
				{
					command.rgba = {
						r,g,b,a
					};
					return *this;
				}

				BShape& color(const Color& color)
				{
					command.rgba = color;
					return *this;
				}

				BShape& rotationX(float r)
				{
					command.angleX = r;
					return *this;
				}

				BShape& rotationY(float r)
				{
					command.angleY = r;
					return *this;
				}

				BShape& rotationZ(float r)
				{
					command.angleZ = r;
					return *this;
				}

				BShape& scaled(float scale)
				{
					command.scale = scale;
					return *this;
				}

				const int width() const
				{
					return (int)command.size.w;
				}

				const int height() const
				{
					return (int)command.size.h;
				}

				const std::pair<float, float> pos() const
				{
					return std::pair<float, float>(command.pos.x, command.pos.y);
				}

				float x() const
				{
					return command.pos.x;
				}

				float y() const
				{
					return command.pos.y;
				}

				DrawCommand get() const
				{
					return command;
				}
			};

		}

		class Rect SEALED : public Core::BShape
		{
		public:
			Rect(int width, int height)
			{
				command.size = { (float)width, (float)height };
				command.type = Core::CommandType::SHAPE;
				//頂点バッファの設定とデータの作成
				Vertex vertices[]
				{
					{  0.0f,   0.0f, 1.0f },
					{  0.0f, (float)height, 1.0f },
					{ (float)width, (float)height, 1.0f },
					{ (float)width,   0.0f, 1.0f },
				};
				{
					D3D11_BUFFER_DESC desc{};
					desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
					desc.Usage = D3D11_USAGE_DEFAULT;
					desc.ByteWidth = sizeof(vertices);

					D3D11_SUBRESOURCE_DATA data{};
					data.pSysMem = vertices;

//					createBuffer(&desc, &data, command.vertexBuffer.GetAddressOf());
					command.vertexBuffer = createBuffer(&desc, &data);
				}

				//インデックスバッファの設定とデータの作成
				Index indices[]
				{
					0,1,2,
					0,2,3,
				};
				command.indexCount = _countof(indices);
				{
					D3D11_BUFFER_DESC desc{};
					desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
					desc.Usage = D3D11_USAGE_DEFAULT;
					desc.ByteWidth = sizeof(indices);

					D3D11_SUBRESOURCE_DATA data{};
					data.pSysMem = indices;

					command.indexBuffer = createBuffer(&desc, &data);
				}

				//テクスチャの作成
/*
				{
					D3D11_TEXTURE2D_DESC desc{};
					desc.Width = (UINT)width;
					desc.Height = (UINT)height;
					desc.MipLevels = 1;
					desc.ArraySize = 1;
					desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
					desc.SampleDesc.Count = 1;
					desc.Usage = D3D11_USAGE_DEFAULT;
					desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
					desc.CPUAccessFlags = 0;
					desc.MiscFlags = 0;
//					動的テクスチャを作る時だけ使いたい
//					desc.Usage = D3D11_USAGE_DYNAMIC;
//					desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
					std::vector<Color> pixels((UINT)(width * height), Color{ 0xff, 0xff, 0xff, 0xff });
					D3D11_SUBRESOURCE_DATA data{};
					data.pSysMem = pixels.data();
					data.SysMemPitch = static_cast<UINT>((size_t)width * sizeof(Color));
					data.SysMemSlicePitch = 0;

					createTexture(desc, data, command.tex.GetAddressOf());

					D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
					srvDesc.Format = desc.Format;
					srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
					srvDesc.Texture2D.MostDetailedMip = 0;
					srvDesc.Texture2D.MipLevels = 1;

					createShaderResourceView(command.tex.Get(), srvDesc, command.srv.GetAddressOf());
				}
*/
			}
		};

		class Circle SEALED : public Core::BShape
		{
		private:
		public:
			Circle(int radius)
			{
				const int segment = 128;
				command.size = { (float)radius, (float)radius };
				command.cd.pos = { 0, 0 };
				command.cd.radius = (float)radius;
				command.cd.edge = 1.0f;
				command.type = Core::CommandType::SHAPE;

				//頂点
				{
					std::vector<Vertex> vertices;
					for (int i = 0; i <= segment; i++)
					{
						float theta = 2.0f * XM_PI * i / segment;
						vertices.emplace_back(Vertex{
								radius * cosf(theta),
								radius * sinf(theta),
								1.0f
							}
						);
//std::wcout << std::format(L"Vertex  {}  {}  {}", radius * cosf(theta), radius * sinf(theta), 1.0f) << std::endl;
					}
					D3D11_BUFFER_DESC desc{};
					desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
					desc.Usage = D3D11_USAGE_DEFAULT;
					desc.ByteWidth = static_cast<UINT>(vertices.size() * sizeof(Vertex));

					D3D11_SUBRESOURCE_DATA data{};
					data.pSysMem = vertices.data();

					command.vertexBuffer = createBuffer(&desc, &data);
				}

				//インデックス
				{
					std::vector<Index> indices;

					for (int i = 0; i <= segment; i++)
					{
						indices.emplace_back(0);
						indices.emplace_back(i);
						indices.emplace_back(i + 1);
//std::wcout << std::format(L"Indices  {}  {}  {}", 0, i, i + 1) << std::endl;
					}
					command.indexCount = static_cast<unsigned int>(indices.size());

					D3D11_BUFFER_DESC desc{};
					desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
					desc.Usage = D3D11_USAGE_DEFAULT;
					desc.ByteWidth = static_cast<UINT>(indices.size() * sizeof(Index));

					D3D11_SUBRESOURCE_DATA data{};
					data.pSysMem = indices.data();

					command.indexBuffer = createBuffer(&desc, &data);
				}
			}

			Circle(int radius, int segment)
			{
				Core::DrawCommand cmd{};
				cmd.size = { (float)radius, (float)radius };
				cmd.cd.pos = { 0, 0 };
				cmd.cd.radius = radius;
				cmd.cd.edge = 1.0f;
				cmd.type = Core::CommandType::SHAPE;

				//頂点
				{
					std::vector<Vertex> vertices;
					for (int i = 0; i <= segment; i++)
					{
						float theta = 2.0f * XM_PI * i / segment;
						vertices.emplace_back(Vertex{
								radius * cosf(theta),
								radius * sinf(theta),
								1.0f
							}
						);
						//std::wcout << std::format(L"Vertex  {}  {}  {}", radius * cosf(theta), radius * sinf(theta), 1.0f) << std::endl;
					}
					D3D11_BUFFER_DESC desc{};
					desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
					desc.Usage = D3D11_USAGE_DEFAULT;
					desc.ByteWidth = static_cast<UINT>(vertices.size() * sizeof(Vertex));

					D3D11_SUBRESOURCE_DATA data{};
					data.pSysMem = vertices.data();

					cmd.vertexBuffer = createBuffer(&desc, &data);
				}

				//インデックス
				{
					std::vector<Index> indices;

					for (int i = 0; i <= segment; i++)
					{
						indices.emplace_back(0);
						indices.emplace_back(i);
						indices.emplace_back(i + 1);
						//std::wcout << std::format(L"Indices  {}  {}  {}", 0, i, i + 1) << std::endl;
					}
					cmd.indexCount = static_cast<unsigned int>(indices.size());

					D3D11_BUFFER_DESC desc{};
					desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
					desc.Usage = D3D11_USAGE_DEFAULT;
					desc.ByteWidth = static_cast<UINT>(indices.size() * sizeof(Index));

					D3D11_SUBRESOURCE_DATA data{};
					data.pSysMem = indices.data();

					cmd.indexBuffer = createBuffer(&desc, &data);
				}

				this->command = cmd;
			}
		};

		class Texture SEALED : public Core::BShape
		{
		public:
			Texture(int width, int height, std::vector<Color> pixels)
			{
				float width_f = static_cast<float>(width);
				float height_f = static_cast<float>(height);
				//頂点の作成
				command.type = Core::CommandType::TEXTURE;
				command.size = { width_f, height_f };
				Vertex vertices[] = {
					{  0.0f,   0.0f,     0.0f},
					{  0.0f,   height_f, 1.0f},
					{ width_f, height_f, 1.0f},
					{ width_f, 0.0f,     1.0f},
				};
				command.vertexBuffer = createVertexBuffer(vertices, sizeof(vertices));

				//インデックスの作成
				Index indices[] = {
					0,1,2,
					0,2,3,
				};
				command.indexCount = _countof(indices);
				command.indexBuffer = createIndexBuffer(indices, sizeof(indices));

				//テクスチャ座標の作成
				UV texcoords[] = {
					{0.0f, 0.0f},
					{0.0f, 1.0f},
					{1.0f, 1.0f},
					{1.0f, 0.0f}
				};
				command.texcoordBuffer = createTexcoordBuffer(texcoords, sizeof(texcoords));

				//テクスチャの書き込み
				//サイズが指定されていたら処理
				int totalPixel = width * height;
				command.srv = createShaderResourceView(
					createTexture(
						width,
						height,
						pixels).Get(), 
					nullptr);
			}

			Texture(const Image& image)
			{
				float width_f = static_cast<float>(image.width);
				float height_f = static_cast<float>(image.height);
				//頂点の作成
				command.type = Core::CommandType::TEXTURE;
				command.size = { width_f, height_f };
				Vertex vertices[] = {
					{  0.0f,   0.0f,     0.0f},
					{  0.0f,   height_f, 1.0f},
					{ width_f, height_f, 1.0f},
					{ width_f, 0.0f,     1.0f},
				};
				command.vertexBuffer = createVertexBuffer(vertices, sizeof(vertices));

				//インデックスの作成
				Index indices[] = {
					0,1,2,
					0,2,3,
				};
				command.indexCount = _countof(indices);
				command.indexBuffer = createIndexBuffer(indices, sizeof(indices));

				//テクスチャ座標の作成
				UV texcoords[] = {
					{0.0f, 0.0f},
					{0.0f, 1.0f},
					{1.0f, 1.0f},
					{1.0f, 0.0f}
				};
				command.texcoordBuffer = createTexcoordBuffer(texcoords, sizeof(texcoords));

				//テクスチャの書き込み
				//サイズが指定されていたら処理
				command.srv = createShaderResourceView(createTexture(
						image.width,
						image.height,
						image.data()
						)
					.Get(), nullptr
				);
			}
		};

		class Text
		{
		private:
			inline static std::unordered_map<wchar_t, Image> textImages{};

			inline static std::unordered_map<wchar_t, wchar_t> dakutenMap{
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
			inline static std::unordered_map<wchar_t, wchar_t> handakutenMap{
				{ L'ぱ', L'は' },
				{ L'ぴ', L'ひ' },
				{ L'ぷ', L'ふ' },
				{ L'ぺ', L'へ' },
				{ L'ぽ', L'ほ' },
			};
			inline static std::unordered_map<wchar_t, std::wstring> symbolMap{
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
			inline static std::unordered_map<wchar_t, std::wstring> lowerMap{
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

			enum MojiType{
				NORMAL,
				DAKUTEN,
				HANDAKUTEN,
			};
			static std::pair<std::wstring, MojiType> getCharImagePath(wchar_t wch)
			{
				MojiType type;

				std::wstring file = L"Null";

				if (dakutenMap.contains(wch))
				{
					type = MojiType::DAKUTEN;
					wch = dakutenMap[wch];
				}
				else if(handakutenMap.contains(wch))
				{
					type = MojiType::HANDAKUTEN;
					wch = handakutenMap[wch];
				}
				else if (symbolMap.contains(wch))
				{
					file = symbolMap[wch];
				}
				else if (lowerMap.contains(wch))
				{
					file = lowerMap[wch];
				}

				std::wstring path = L"images/font/";
				if (file != L"Null")
				{
					path += file;
				}
				else
				{
					path.push_back(wch);
				}
				path += L".png";

				if (wch == L' ')
				{
					path = L"images/font/space.png";
				}
				if (!std::filesystem::exists(path))
				{
					path = L"images/font/tofu.png";
				}

				return { path, type };
			}


//Old getCharImagePath
/*
			static std::wstring getCharImagePath(wchar_t wch)
			{
				std::wstring path = L"images/font/";
				path.push_back(wch);
				path += L".png";

				if (wch == L' ')
				{
					path = L"images/font/space.png";
				}
				if (!std::filesystem::exists(path))
				{
					path = L"images/font/tofu.png";
				}
				return path;
			}
*/
		public:
			static Image GetTextImage(const std::wstring& text)
			{
				if (text.length() <= 0)
				{
					return Image{ getCharImagePath(L' ').first };
				}


				auto[path, type] = getCharImagePath(text[0]);
				Image out = Image{ path };
				if (type == MojiType::DAKUTEN)
				{
					if (!textImages.contains(L'゛'))//画像を読み込んでいなかったら
					{
						textImages.emplace(L'゛', Image{ getCharImagePath(L'゛').first });
					}
					out = out + textImages[L'゛'];
				}
				else if (type == MojiType::HANDAKUTEN)//画像を読み込んでいなかったら
				{
					if (!textImages.contains(L'゜'))
					{
						textImages.emplace(L'゜', Image{ getCharImagePath(L'゜').first });
					}
					out = out + textImages[L'゜'];
				}

				for (int i = 1; i < text.length(); i++)
				{
					auto [path2, type2] = getCharImagePath(text[i]);
					if (!textImages.contains(text[i]))//画像を読み込んでいなかったら
					{
						textImages.emplace(text[i], Image{ path2 });
					}
					out = out + textImages[text[i]];

					if (type2 == MojiType::DAKUTEN)
					{
						if (!textImages.contains(L'゛'))//画像を読み込んでいなかったら
						{
							textImages.emplace(L'゛', Image{ getCharImagePath(L'゛').first });
						}
						out = out + textImages[L'゛'];
					}
					else if (type2 == MojiType::HANDAKUTEN)
					{
						if (!textImages.contains(L'゜'))//画像を読み込んでいなかったら
						{
							textImages.emplace(L'゜', Image{ getCharImagePath(L'゜').first });
						}
						out = out + textImages[L'゜'];
					}
				}

				return out;
			}
		};

		class GraphicsXI SEALED
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

		public:
			GraphicsXI() = default;
			GraphicsXI(const GraphicsXI&) = default;
			GraphicsXI(const HWND parentHdl, const HINSTANCE parentIns)
			{
				static_assert(sizeof(Color) == 4, "Colorのサイズが4バイトではありませんでした");

				//デバイスとか諸々作成
				{

					UINT creationFlag = 0;
#ifdef _DEBUG
					creationFlag |= D3D11_CREATE_DEVICE_DEBUG;
#endif
					D3D_FEATURE_LEVEL featureLevels[]{
						D3D_FEATURE_LEVEL_11_1,
						D3D_FEATURE_LEVEL_11_0,
					};

					UINT numFeatureLevel = ARRAYSIZE(featureLevels);

					Hr = D3D11CreateDevice(
						nullptr,					//Adapter
						D3D_DRIVER_TYPE_HARDWARE,	//Driver Type
						nullptr,					//Soft ware
						creationFlag,				//Flags
						featureLevels,				//DX11 Versions
						numFeatureLevel,			//DX11 Version Count
						D3D11_SDK_VERSION,			//DX11 SDK Version
						Core::device.GetAddressOf(),//Device Pointer
						nullptr,					//DX11 Version
						Core::context.GetAddressOf()//Context Pointer
					);
					assert(SUCCEEDED(Hr));
					AST_NULL(Core::device.Get());
					AST_NULL(Core::context.Get());

#ifdef _DEBUG
					Hr = Core::device->QueryInterface(IID_PPV_ARGS(&Core::debug));
					assert(SUCCEEDED(Hr));
#endif
				}

				//リソース
				{
					//スワップチェインとバックバッファ作成
					{
						ComPtr<IDXGIFactory7> factory = nullptr;
						Hr = CreateDXGIFactory2(0, IID_PPV_ARGS(&factory));
						assert(SUCCEEDED(Hr));

						//スワップチェインの設定とか
						DXGI_SWAP_CHAIN_DESC1 desc{};
						desc.Width = Win32_CPP::GetWindowWidth(parentHdl);
						desc.Height = Win32_CPP::GetWindowHeight(parentHdl);
						desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
						desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
						desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
						desc.BufferCount = DoubleBuffer;
						desc.SampleDesc.Count = 1;
						desc.Scaling = DXGI_SCALING_NONE;
						Hr = factory->CreateSwapChainForHwnd(Core::device.Get(), parentHdl, &desc, nullptr, nullptr, Core::swapChain.GetAddressOf());
						assert(SUCCEEDED(Hr));
					}
					//バックバッファービューを作る
					{
						//バックバッファを取得
						ComPtr<ID3D11Texture2D> backBuffer = nullptr;
						Hr = Core::swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
						assert(SUCCEEDED(Hr));

						//ビューを作成
						Hr = Core::device->CreateRenderTargetView(backBuffer.Get(), nullptr, Core::backBufferView.GetAddressOf());
					}
				}

				//パイプライン
				{
					//インプットレイアウトとシェーダー
					{
						UINT slot0 = 0;//座標ばっふぁ
						UINT slot1 = 1;//テクスチャ座標ばっふぁ
						D3D11_INPUT_ELEMENT_DESC desc[]
						{
							{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, slot0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
							{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,	 slot1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
						};
						UINT numElements = sizeof(desc) / sizeof(desc[0]);

						ComPtr<ID3DBlob> vs = nullptr;

						Hr = D3DReadFileToBlob(L"shader/VertexShader.vso", vs.GetAddressOf());
						assert(SUCCEEDED(Hr));
						AST_NULL(vs.Get());

						ComPtr<ID3DBlob> ps = nullptr;
						Hr = D3DReadFileToBlob(L"shader/PixelShader.pso", ps.GetAddressOf());
						assert(SUCCEEDED(Hr));
						AST_NULL(ps.Get());

						Hr = Core::device->CreateInputLayout(desc, numElements,
							vs->GetBufferPointer(), vs->GetBufferSize(),
							Core::inputLayout.GetAddressOf());
						assert(SUCCEEDED(Hr));
						AST_NULL(Core::device.Get());

						//シェーダーの作成(PSとVS)
						Hr = Core::device->CreateVertexShader(vs->GetBufferPointer(), vs->GetBufferSize(), nullptr, Core::vShader.GetAddressOf());
						assert(SUCCEEDED(Hr));
						AST_NULL(Core::vShader.Get());

						Hr = Core::device->CreatePixelShader(ps->GetBufferPointer(), ps->GetBufferSize(), nullptr, Core::pShader.GetAddressOf());
						assert(SUCCEEDED(Hr));
						AST_NULL(Core::pShader.Get());
					}
					//ラスタライザーステートの作成
					{
						D3D11_RASTERIZER_DESC desc{};
						desc.FrontCounterClockwise = FALSE;
						desc.CullMode = D3D11_CULL_NONE;
						desc.DepthBias = 0;
						desc.DepthBiasClamp = 0;
						desc.SlopeScaledDepthBias = 0;
						desc.DepthClipEnable = FALSE;
						desc.ScissorEnable = FALSE;
						desc.MultisampleEnable = FALSE;
						desc.AntialiasedLineEnable = FALSE;
						desc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
// 						desc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
						Core::device->CreateRasterizerState(&desc, Core::rasterizerState.GetAddressOf());
					}
					//表示領域の作成
					{
						Core::viewPort.Width = (float)(Win32_CPP::GetWindowWidth(parentHdl));
						Core::viewPort.Height = (float)(Win32_CPP::GetWindowHeight(parentHdl));
						Core::viewPort.TopLeftX = 0;
						Core::viewPort.TopLeftY = 0;
						Core::viewPort.MinDepth = D3D11_MIN_DEPTH;
						Core::viewPort.MaxDepth = D3D11_MAX_DEPTH;
					}
				}

				//定数バッファ
				{
					//行列
					{
						D3D11_BUFFER_DESC desc{};
						desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
						desc.Usage = D3D11_USAGE_DYNAMIC;
						desc.ByteWidth = sizeof(XMMATRIX);
						desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

						Hr = Core::device->CreateBuffer(&desc, nullptr, Core::matrixConstantBuffer.GetAddressOf());
						assert(SUCCEEDED(Hr));
						AST_NULL(Core::matrixConstantBuffer.Get());
					}

					//ディフューズ
					{
						D3D11_BUFFER_DESC desc{};
						desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
						desc.Usage = D3D11_USAGE_DYNAMIC;
						desc.ByteWidth = sizeof(XMFLOAT4);
						desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

						Hr = Core::device->CreateBuffer(&desc, nullptr, Core::diffuseConstantBuffer.GetAddressOf());
						assert(SUCCEEDED(Hr));
						AST_NULL(Core::diffuseConstantBuffer.Get());
					}

					//円を綺麗に書くもの
					{
						D3D11_BUFFER_DESC desc{};
						desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
						desc.Usage = D3D11_USAGE_DYNAMIC;
						desc.ByteWidth = sizeof(Core::CircleData);
						desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

Hr = Core::device->CreateBuffer(&desc, nullptr, Core::circleDataConstantBuffer.GetAddressOf());
						assert(SUCCEEDED(Hr));
						AST_NULL(Core::circleDataConstantBuffer.Get());
					}

					//描画するタイプ(テクスチャのありなし)
					{
						D3D11_BUFFER_DESC desc{};
						desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
						desc.Usage = D3D11_USAGE_DYNAMIC;
						desc.ByteWidth = sizeof(int) * 4;
						desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

Hr = Core::device->CreateBuffer(&desc, nullptr, Core::drawTypeConstantBuffer.GetAddressOf());
						std::cout << "Result:" << Hr << std::endl;
						assert(SUCCEEDED(Hr));
						AST_NULL(Core::drawTypeConstantBuffer.Get());
					}
				}

				//サンプラーステート
				{
					D3D11_SAMPLER_DESC desc{};
					desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
//					desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
					desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
					desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
					desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
					desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
					desc.MinLOD = 0;
					desc.MaxLOD = D3D11_FLOAT32_MAX;
					Hr = Core::device->CreateSamplerState(&desc, Core::samplerState.GetAddressOf());
				}

				//ブレンドステート
				{
					D3D11_BLEND_DESC desc{};
					desc.AlphaToCoverageEnable = FALSE;
					desc.IndependentBlendEnable = FALSE;
					for (int i = 0; i < 1; i++)
					{
						desc.RenderTarget[i].BlendEnable = TRUE;
						desc.RenderTarget[i].SrcBlend = D3D11_BLEND_SRC_ALPHA;
						desc.RenderTarget[i].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
						desc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
						desc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
						desc.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ZERO;
						desc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
						desc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
					}
					Hr = Core::device->CreateBlendState(&desc, Core::blendState.GetAddressOf());
					assert(SUCCEEDED(Hr));
				}
			}

			static void ClearColor(const Color& color)
			{
				Core::swapChainClearColor = color;
			}

			bool UpdateConstantBuffer(ID3D11Resource* buf, const void* data, size_t size)
			{
				D3D11_MAPPED_SUBRESOURCE mappedBuf{};
				if (buf == nullptr) { return false; }
				Hr = Core::context->Map(buf, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuf);
				if (FAILED(Hr)) { return false; }
				memcpy(mappedBuf.pData, data, size);
				Core::context->Unmap(buf, 0);
				return true;
			}

			void clear()
			{
				Core::context->OMSetRenderTargets(1, Core::backBufferView.GetAddressOf(), nullptr);

				float color[4] = {
					(float)Core::swapChainClearColor.r / 255.0f,
					(float)Core::swapChainClearColor.g / 255.0f,
					(float)Core::swapChainClearColor.b / 255.0f,
					(float)Core::swapChainClearColor.a / 255.0f,
				};

				Core::context->ClearRenderTargetView(Core::backBufferView.Get(), color);

				//ブレンドステートをセット
				float blendFactor[4]{ 0, 0, 0, 0 };
				Core::context->OMSetBlendState(Core::blendState.Get(), blendFactor, 0xffffffff);
			}

			void firstSetting()
			{
				Core::context->IASetInputLayout(Core::inputLayout.Get());
				Core::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				Core::context->VSSetShader(Core::vShader.Get(), nullptr, NULL);
				Core::context->PSSetShader(Core::pShader.Get(), nullptr, NULL);
				UINT registerB0 = 0;//座標などの行列データ
				Core::context->VSSetConstantBuffers(registerB0, 1, Core::matrixConstantBuffer.GetAddressOf());
				UINT registerB1 = 1;//色情報データ
				Core::context->PSSetConstantBuffers(registerB1, 1, Core::diffuseConstantBuffer.GetAddressOf());
				UINT registerB2 = 2;//(未使用)滑らかな円にするデータ
				Core::context->PSSetConstantBuffers(registerB2, 1, Core::circleDataConstantBuffer.GetAddressOf());
				UINT registerB3 = 3;//描画形式データ
				Core::context->PSSetConstantBuffers(registerB3, 1, Core::drawTypeConstantBuffer.GetAddressOf());
				UINT registerS0 = 0;//サンプラー
				Core::context->PSSetSamplers(registerS0, 1, Core::samplerState.GetAddressOf());
				Core::context->RSSetState(Core::rasterizerState.Get());
				Core::context->RSSetViewports(1, &Core::viewPort);
			}

			void present()
			{
				if (isUpdate)
				{
					firstSetting();
					isUpdate = false;
				}
				for (const auto& cmd : cmds)
				{
					//行列計算とか
					XMMATRIX matrix =
//						XMMatrixTranslation(0 - cmd.size.w / 2, 0 - cmd.size.h / 2, cmd.pos.z) *	//回転用座標
//						XMMatrixRotationX(cmd.angleX) *												//X回転
//						XMMatrixRotationY(cmd.angleY) *												//Y回転
//						XMMatrixRotationZ(cmd.angleZ) *												//Z回転
						XMMatrixScaling(cmd.scale, cmd.scale, cmd.scale) *							//スケール
						XMMatrixTranslation(cmd.pos.x, cmd.pos.y, cmd.pos.z) *						//座標
						XMMatrixOrthographicOffCenterLH(											//座標変換
							0.0f, Core::viewPort.Width,
							Core::viewPort.Height, 0.0f,
							0.0f, 1.0f
						);

					float inv255 = 1.0f / 255.0f;
					XMFLOAT4 rgba = cmd.rgba.floats();

					if (!UpdateConstantBuffer(Core::matrixConstantBuffer.Get(), &matrix, sizeof(matrix)))
					{
						throw std::runtime_error("行列計算で失敗しました。哀れ");
					}
					if (!UpdateConstantBuffer(Core::diffuseConstantBuffer.Get(), &rgba, sizeof(rgba)))
					{
						throw std::runtime_error("色設定で失敗しました。哀れ");
					}
					if (!UpdateConstantBuffer(Core::circleDataConstantBuffer.Get(), &cmd.cd, sizeof(cmd.cd)))
					{
						throw std::runtime_error("円を綺麗にする工程で失敗しました。哀れ");
					}
					if (!UpdateConstantBuffer(Core::drawTypeConstantBuffer.Get(), &cmd.type, sizeof(cmd.type)))
					{
						throw std::runtime_error("描画方式を指定する工程で失敗しました。哀れ");
					}

					//本体
					ID3D11Buffer* buffers[] = {
						cmd.vertexBuffer.Get(),
						cmd.texcoordBuffer.Get()
					};
					UINT strides[] = { sizeof(XMFLOAT3), sizeof(XMFLOAT2) };
					UINT offsets[] = { 0, 0 };
					Core::context->IASetVertexBuffers(0, _countof(buffers), buffers, strides, offsets);
					Core::context->IASetIndexBuffer(cmd.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
					UINT registerT0 = 0;
					Core::context->PSSetShaderResources(registerT0, 1, cmd.srv.GetAddressOf());
					Core::context->DrawIndexed(cmd.indexCount, 0, 0);
				}

				Core::swapChain->Present(1, 0);

				cmds.clear();
			}

			void draw(const Core::DrawCommand& cmd)
			{
				cmds.emplace_back(cmd);
			}

			const size_t drawCount() const
			{
				return cmds.size();
			}
		};
	}
}