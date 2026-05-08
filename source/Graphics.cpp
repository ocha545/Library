#include"../include/Graphics.h"

namespace Win32
{
	bool Core::UpdateConstantBuffer(ID3D11Resource* buf, const void* data, size_t size)
	{
		D3D11_MAPPED_SUBRESOURCE mappedBuf{};
		if (buf == nullptr)
		{
			return false;
		}

		HRESULT Hr = Core::context->Map(buf, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuf);
		if (FAILED(Hr))
		{
			return false;
		}
		memcpy(mappedBuf.pData, data, size);
		Core::context->Unmap(buf, 0);

		return true;
	}


	//class BShape begin
	ComPtr<ID3D11Buffer> Core::BShape::createBuffer(const D3D11_BUFFER_DESC* desc, const D3D11_SUBRESOURCE_DATA* data)
	{
		ComPtr<ID3D11Buffer> tmp = nullptr;
		HRESULT Hr = Core::device->CreateBuffer(desc, data, tmp.GetAddressOf());
		assert(SUCCEEDED(Hr));
		AST_NULL(tmp.Get());
		return tmp;
	}
	ComPtr<ID3D11Buffer> Core::BShape::createVertexBuffer(const Vertex* vtxs, size_t size)
	{
		D3D11_BUFFER_DESC desc{};
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = static_cast<UINT>(size);

		D3D11_SUBRESOURCE_DATA data{};
		data.pSysMem = vtxs;

		return createBuffer(&desc, &data);
	}
	ComPtr<ID3D11Buffer> Core::BShape::createTexcoordBuffer(const UV* uvs, size_t size)
	{
		D3D11_BUFFER_DESC desc{};
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = static_cast<UINT>(size);

		D3D11_SUBRESOURCE_DATA data{};
		data.pSysMem = uvs;

		return createBuffer(&desc, &data);
	}
	ComPtr<ID3D11Buffer> Core::BShape::createIndexBuffer(const Index* idxs, size_t size)
	{
		D3D11_BUFFER_DESC desc{};
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = static_cast<UINT>(size);

		D3D11_SUBRESOURCE_DATA data{};
		data.pSysMem = idxs;

		return createBuffer(&desc, &data);
	}
	ComPtr<ID3D11Texture2D> Core::BShape::createTexture(const Image& image)
	{
		D3D11_TEXTURE2D_DESC desc{};
		desc.Width = image.width;
		desc.Height = image.height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA data{};
		data.pSysMem = image.rawData();
		data.SysMemPitch = image.width * sizeof(Color);

		ComPtr<ID3D11Texture2D> tmp = nullptr;
		HRESULT Hr = Core::device->CreateTexture2D(&desc, &data, tmp.GetAddressOf());
		assert(SUCCEEDED(Hr));
		AST_NULL(tmp.Get());
		return tmp;
	}
	ComPtr<ID3D11Texture2D> Core::BShape::createTexture(int width, int height, const std::vector<Color>& pixels)
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
		assert(SUCCEEDED(Hr));
		AST_NULL(tmp.Get());
		return tmp;
	}
	ComPtr<ID3D11Texture2D> Core::BShape::createTexture(const D3D11_TEXTURE2D_DESC* desc, const D3D11_SUBRESOURCE_DATA* data)
	{
		ComPtr<ID3D11Texture2D> tex;
		HRESULT Hr = Core::device->CreateTexture2D(desc, data, tex.GetAddressOf());
		assert(SUCCEEDED(Hr));
		AST_NULL(tex.Get());
		return tex;
	}
	ComPtr<ID3D11Texture2D> Core::BShape::createDynamicTexture(const Image& image)
	{
		D3D11_TEXTURE2D_DESC desc{};
		desc.Width = image.width;
		desc.Height = image.height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA data{};
		data.pSysMem = image.rawData();
		data.SysMemPitch = image.width * sizeof(Color);

		ComPtr<ID3D11Texture2D> tmp = nullptr;
		HRESULT Hr = Core::device->CreateTexture2D(&desc, &data, tmp.GetAddressOf());
		assert(SUCCEEDED(Hr));
		AST_NULL(tmp.Get());
		return tmp;
	}
	ComPtr<ID3D11ShaderResourceView> Core::BShape::createShaderResourceView(ID3D11Texture2D* tex, const D3D11_SHADER_RESOURCE_VIEW_DESC* desc)
	{
		ComPtr<ID3D11ShaderResourceView> srv;
		HRESULT Hr = Core::device->CreateShaderResourceView(tex, desc, srv.GetAddressOf());
		assert(SUCCEEDED(Hr));
		AST_NULL(srv.Get());
		return srv;
	}

	Core::BShape::BShape(const DrawCommand& cmd)
		: command(cmd)
	{
	}
	Core::BShape::~BShape()
	{
		command.indexBuffer.Reset();
		command.vertexBuffer.Reset();
	}


	bool Core::BShape::intersect(const DrawCommand& s)
	{
		const DrawCommand& c = command;
		return(
			c.pos.x < s.pos.x + s.size.w &&
			s.pos.x < c.pos.x + c.size.w &&
			c.pos.y < s.pos.y + s.size.h &&
			s.pos.y < c.pos.y + c.size.h
		);
	}
	Core::BShape& Core::BShape::position(float x, float y)
	{
		command.pos = { x, y, 1.0f };
		return *this;
	}
	Core::BShape& Core::BShape::color(ubyte r, ubyte g, ubyte b)
	{
		command.rgba = { r, g, b, 0xff };
		command.runUpdate();
		return *this;
	}
	Core::BShape& Core::BShape::color(ubyte r, ubyte g, ubyte b, ubyte a)
	{
		command.rgba = { r, g, b, a };
		command.runUpdate();
		return *this;
	}
	Core::BShape& Core::BShape::color(Color color)
	{
		command.rgba = color;
		command.runUpdate();
		return *this;
	}
	Core::BShape& Core::BShape::rotationX(float r)
	{
		command.angleX = r;
		return *this;
	}
	Core::BShape& Core::BShape::rotationY(float r)
	{
		command.angleY = r;
		return *this;
	}
	Core::BShape& Core::BShape::rotationZ(float r)
	{
		command.angleZ = r;
		return *this;
	}
	Core::BShape& Core::BShape::scaled(float scale)
	{
		command.scale = scale;
		command.size.w = scale * command.size.w;
		command.size.h = scale * command.size.h;
		return *this;
	}
	Core::DrawCommand Core::BShape::get() const
	{
		return command;
	}
	//class BShape end
	// 
	//namespace Core end

	//class Rect begin
	Rect::Rect(int width, int height)
	{
		float width_f = static_cast<float>(width);
		float height_f = static_cast<float>(height);

		command.size = { width_f, height_f };
		command.type = Core::CommandType::SHAPE;
		//頂点バッファの設定とデータの作成
		Vertex vertices[]
		{
			{  0.0f,		0.0f, 1.0f },
			{  0.0f,	height_f, 1.0f },
			{ width_f,	height_f, 1.0f },
			{ width_f,		0.0f, 1.0f },
		};
		{
			D3D11_BUFFER_DESC desc{};
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.ByteWidth = sizeof(vertices);

			D3D11_SUBRESOURCE_DATA data{};
			data.pSysMem = vertices;

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
	}
	//class Rect end

	//class Line begin
	Line::Line(int startX, int startY, int endX, int endY)
	{
		float startX_f = static_cast<float>(startX);
		float startY_f = static_cast<float>(startY);
		float endX_f = static_cast<float>(startX);
		float endY_f = static_cast<float>(endY);
		command.type = Core::CommandType::LINE;

		Vertex vertices[] = {
			{ startX_f, startY_f, 1.0f },
			{   endX_f,   endY_f, 1.0f }
		};

		command.vertexBuffer = createVertexBuffer(vertices, sizeof(vertices));

		Index indices[] = {
			0, 1
		};
		command.indexCount = _countof(indices);
		command.indexBuffer = createIndexBuffer(indices, sizeof(indices));
	}

	//class Circle begin
	Circle::Circle(int radius)
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
			}
			D3D11_BUFFER_DESC desc{};
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.ByteWidth = static_cast<UINT>(vertices.size() * sizeof(Vertex));

			D3D11_SUBRESOURCE_DATA data{};
			data.pSysMem = vertices.data();
			command.vertexBuffer = createVertexBuffer(vertices.data(), vertices.size() * sizeof(Vertex));
//			command.vertexBuffer = createBuffer(&desc, &data);
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

			command.indexBuffer = createIndexBuffer(indices.data(), indices.size() * sizeof(Index));
//			command.indexBuffer = createBuffer(&desc, &data);
		}
	}
	Circle::Circle(int radius, int segment)
	{
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
			}
			D3D11_BUFFER_DESC desc{};
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.ByteWidth = static_cast<UINT>(vertices.size() * sizeof(Vertex));

			D3D11_SUBRESOURCE_DATA data{};
			data.pSysMem = vertices.data();
			command.vertexBuffer = createVertexBuffer(vertices.data(), vertices.size() * sizeof(Vertex));
//			command.vertexBuffer = createBuffer(&desc, &data);
		}

		//インデックス
		{
			std::vector<Index> indices;

			for (int i = 0; i <= segment; i++)
			{
				indices.emplace_back(0);
				indices.emplace_back(i);
				indices.emplace_back(i + 1);
			}
			command.indexCount = static_cast<unsigned int>(indices.size());

			D3D11_BUFFER_DESC desc{};
			desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.ByteWidth = static_cast<UINT>(indices.size() * sizeof(Index));

			D3D11_SUBRESOURCE_DATA data{};
			data.pSysMem = indices.data();

			command.indexBuffer = createIndexBuffer(indices.data(), indices.size() * sizeof(Index));
//			command.indexBuffer = createBuffer(&desc, &data);
		}
	}
	//class Circle end

	//class Texture begin
	Texture::Texture(int width, int height, std::vector<Color> pixels)
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
			nullptr
		);
	}
	Texture::Texture(const Image& image)
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
		command.srv = createShaderResourceView(
			createTexture(
				image.width,
				image.height,
				image.data()
			).Get(), nullptr
		);
	}
	//class Texture end



	//class Text begin
	std::pair<std::wstring, Text::MojiType> Text::getCharImagePath(wchar_t wch)
	{
		MojiType type;

		std::wstring file = L"Null";

		if (dakutenTable.contains(wch))
		{
			type = MojiType::DAKUTEN;
			wch = dakutenTable[wch];
		}
		else if (handakutenTable.contains(wch))
		{
			type = MojiType::HANDAKUTEN;
			wch = handakutenTable[wch];
		}
		else if (symbolTable.contains(wch))
		{
			file = symbolTable[wch];
		}
		else if (lowerTable.contains(wch))
		{
			file = lowerTable[wch];
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

	Image Text::GetTextImage(const wstring& text)
	{
		if (text.length() <= 0)
		{
			return Image{ getCharImagePath(L' ').first };
		}

		auto [path, type] = getCharImagePath(text[0]);
		Image out = Image{ path };
		if (type == MojiType::DAKUTEN)
		{
			if (!Core::textMap.contains(L'゛'))//画像を読み込んでいなかったら
			{
				Core::textMap.emplace(L'゛', Image{ getCharImagePath(L'゛').first });
			}
			out = out + Core::textMap[L'゛'];
		}
		else if (type == MojiType::HANDAKUTEN)//画像を読み込んでいなかったら
		{
			if (!Core::textMap.contains(L'゜'))
			{
				Core::textMap.emplace(L'゜', Image{ getCharImagePath(L'゜').first });
			}
			out = out + Core::textMap[L'゜'];
		}

		for (int i = 1; i < text.length(); i++)
		{
			auto [path2, type2] = getCharImagePath(text[i]);
			if (!Core::textMap.contains(text[i]))//画像を読み込んでいなかったら
			{
				Core::textMap.emplace(text[i], Image{ path2 });
			}
			out = out + Core::textMap[text[i]];

			if (type2 == MojiType::DAKUTEN)
			{
				if (!Core::textMap.contains(L'゛'))//画像を読み込んでいなかったら
				{
					Core::textMap.emplace(L'゛', Image{ getCharImagePath(L'゛').first });
				}
				out = out + Core::textMap[L'゛'];
			}
			else if (type2 == MojiType::HANDAKUTEN)
			{
				if (!Core::textMap.contains(L'゜'))//画像を読み込んでいなかったら
				{
					Core::textMap.emplace(L'゜', Image{ getCharImagePath(L'゜').first });
				}
				out = out + Core::textMap[L'゜'];
			}
		}

		return out.swapped(Color(0), Color(255));
	}
	//class Text end

	//class GraphicsXI begin
	GraphicsXI::GraphicsXI(HWND parentHdl, HINSTANCE parentIns)
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
//#error "aiue"
			Hr = Core::device->QueryInterface(IID_PPV_ARGS(Core::debug.GetAddressOf()));
			assert(SUCCEEDED(Hr));
#endif
		}

		//リソース
		{
			//スワップチェインとバックバッファ作成
			{
				ComPtr<IDXGIFactory7> factory = nullptr;
				Hr = CreateDXGIFactory2(0, IID_PPV_ARGS(factory.GetAddressOf()));
				assert(SUCCEEDED(Hr));

				//スワップチェインの設定とか
				DXGI_SWAP_CHAIN_DESC1 desc{};
				desc.Width = Win32::GetWindowWidth(parentHdl);
				desc.Height = Win32::GetWindowHeight(parentHdl);
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
				Hr = Core::swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));
				assert(SUCCEEDED(Hr));

				//ビューを作成
				Hr = Core::device->CreateRenderTargetView(backBuffer.Get(), nullptr, Core::backBufferView.GetAddressOf());
				assert(SUCCEEDED(Hr));
				AST_NULL(Core::backBufferView.Get());
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
				Core::viewPort.Width = (float)(Win32::GetWindowWidth(parentHdl));
				Core::viewPort.Height = (float)(Win32::GetWindowHeight(parentHdl));
				if(Core::virtualWidth == NULL)
				{
					Core::virtualWidth = (float)(Win32::GetWindowWidth(parentHdl));
				}
				if (Core::virtualHeight == NULL)
				{
					Core::virtualHeight = (float)(Win32::GetWindowHeight(parentHdl));
				}
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

	GraphicsXI::~GraphicsXI()
	{
		dispose();
	}


	void GraphicsXI::SetClearColor(Color color)
	{
		Core::swapChainClearColor = color;
	}

	void GraphicsXI::SetVirtualResolution(int vw, int vh)
	{
		Core::virtualWidth = vw;
		Core::virtualHeight = vh;
	}

	void GraphicsXI::clear()
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
	void GraphicsXI::firstSetting()
	{
		Core::context->IASetInputLayout(Core::inputLayout.Get());
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
	void GraphicsXI::present()
	{
		if (isUpdate)
		{
			firstSetting();
			isUpdate = false;
		}
		for (auto& cmd : cmds)
		{
			if (cmd.isUpdate)
			{
				firstSetting();
				cmd.endUpdate();
			}
			//行列計算とか
			float cx = -cmd.size.w / 2.0f;
			float cy = -cmd.size.h / 2.0f;

			XMMATRIX matrix =
				XMMatrixTranslation(cx, cy, 0.0f) *						//回転用座標
				XMMatrixRotationX(cmd.angleX) *							//X回転
				XMMatrixRotationY(cmd.angleY) *							//Y回転
				XMMatrixRotationZ(cmd.angleZ) *							//Z回転
				XMMatrixScaling(cmd.scale, cmd.scale, 1.0f) *			//スケール
				XMMatrixTranslation(-cx, -cy, 0.0f) *					//回転用座標
				XMMatrixTranslation(cmd.pos.x, cmd.pos.y, cmd.pos.z) *	//座標
				XMMatrixOrthographicOffCenterLH(						//座標変換
					0.0f, Core::virtualWidth,
					Core::virtualHeight, 0.0f,
					0.0f, 1.0f
				);

			float inv255 = 1.0f / 255.0f;
			XMFLOAT4 rgba = cmd.rgba.floats();

			if (!Core::UpdateConstantBuffer(Core::matrixConstantBuffer.Get(), &matrix, sizeof(matrix)))
			{
				throw std::runtime_error("行列計算で失敗しました。哀れ");
			}
			if (!Core::UpdateConstantBuffer(Core::diffuseConstantBuffer.Get(), &rgba, sizeof(rgba)))
			{
				throw std::runtime_error("色設定で失敗しました。哀れ");
			}
			if (!Core::UpdateConstantBuffer(Core::circleDataConstantBuffer.Get(), &cmd.cd, sizeof(cmd.cd)))
			{
				throw std::runtime_error("円を綺麗にする工程で失敗しました。哀れ");
			}
			if (!Core::UpdateConstantBuffer(Core::drawTypeConstantBuffer.Get(), &cmd.type, sizeof(cmd.type)))
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
			if (cmd.type == Core::CommandType::LINE)
			{
				Core::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
			}
			else
			{
				Core::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			}
			Core::context->IASetVertexBuffers(0, _countof(buffers), buffers, strides, offsets);
			Core::context->IASetIndexBuffer(cmd.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
			UINT registerT0 = 0;
			Core::context->PSSetShaderResources(registerT0, 1, cmd.srv.GetAddressOf());

			if (cmd.type == Core::CommandType::LINE)
			{
				Core::context->Draw(2, 0);
			}
			else
			{
				Core::context->DrawIndexed(cmd.indexCount, 0, 0);
			}
		}

		Core::swapChain->Present(1, 0);

		cmds.clear();
	}
	void GraphicsXI::draw(const Core::DrawCommand& cmd)
	{
		cmds.emplace_back(cmd);
	}

	void GraphicsXI::dispose()
	{
		std::cout << " Dest";
		if (Core::blendState.Get())
		{
			Core::blendState.Reset();
		}
		if (Core::samplerState.Get())
		{
			Core::samplerState.Reset();
		}
		if (Core::drawTypeConstantBuffer.Get())
		{
			Core::drawTypeConstantBuffer.Reset();
		}
		if (Core::circleDataConstantBuffer.Get())
		{
			Core::circleDataConstantBuffer.Reset();
		}
		if (Core::diffuseConstantBuffer.Get())
		{
			Core::diffuseConstantBuffer.Reset();
		}
		if (Core::matrixConstantBuffer.Get())
		{
			Core::matrixConstantBuffer.Reset();
		}
		if (Core::rasterizerState.Get())
		{
			Core::rasterizerState.Reset();
		}
		if (Core::pShader.Get())
		{
			Core::pShader.Reset();
		}
		if (Core::vShader.Get())
		{
			Core::vShader.Reset();
		}
		if (Core::inputLayout.Get())
		{
			Core::inputLayout.Reset();
		}
		if (Core::backBufferView.Get())
		{
			Core::backBufferView.Reset();
		}
		if (Core::swapChain.Get())
		{
			Core::swapChain.Reset();
		}
		if (Core::debug.Get())
		{
			Core::debug.Reset();
		}
		if (Core::context.Get())
		{
			Core::context.Reset();
		}
		if (Core::device.Get())
		{
			Core::device.Reset();
		}
	}

	const size_t GraphicsXI::drawCount() const
	{
		return cmds.size();
	}
}