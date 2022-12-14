#include "pch.h"
#include "Renderer.h"
#include "DataStructures.h"
#include "EffectShader.h"
#include "EffectTransparency.h"
#include "Texture.h"
#include "Utils.h"

#define USE_OBJ

namespace dae {

	Renderer::Renderer(SDL_Window* pWindow) :
		m_pWindow(pWindow)
	{
		SDL_GetWindowSize(pWindow, &m_Width, &m_Height);

		m_Camera.Initialize(45.f, { .0f,.0f, 0.f }, static_cast<float>(m_Width) / m_Height);


		const HRESULT result = InitializeDirectX();
		if (result == S_OK)
		{
			m_IsInitialized = true;
			std::cout << "DirectX is initialized and ready!\n";

			m_pDiffuseMap = Texture::LoadFromFile("Resources/vehicle_diffuse.png", m_pDevice);
			m_pFireDiffuseMap = Texture::LoadFromFile("Resources/fireFX_diffuse.png", m_pDevice);
			m_pGlossinessMap = Texture::LoadFromFile("Resources/vehicle_gloss.png", m_pDevice);
			m_pNormalMap = Texture::LoadFromFile("Resources/vehicle_normal.png", m_pDevice);
			m_pSpecularMap = Texture::LoadFromFile("Resources/vehicle_specular.png", m_pDevice);

			InitMeshes();
			
		}
		else
		{
			std::cout << "DirectX initialization failed!\n";
		}


	}

	Renderer::~Renderer()
	{
		for (auto& mesh : m_pMeshes)
		{
			delete mesh;
		}

		delete m_pDiffuseMap;
		delete m_pFireDiffuseMap;
		delete m_pSpecularMap;
		delete m_pNormalMap;
		delete m_pGlossinessMap;

		if (m_pRenderTargetView)
		{
			m_pRenderTargetView->Release();
		}
		if (m_pRenderTargetBuffer)
		{
			m_pRenderTargetBuffer->Release();
		}
		if (m_pDepthStencilView)
		{
			m_pDepthStencilView->Release();
		}
		if (m_pDepthStencilBuffer)
		{
			m_pDepthStencilBuffer->Release();
		}
		if (m_pSwapChain)
		{
			m_pSwapChain->Release();
		}
		if (m_pDeviceContext)
		{
			m_pDeviceContext->ClearState();
			m_pDeviceContext->Flush();
			m_pDeviceContext->Release();
		}
		if (m_pDevice)
		{
			m_pDevice->Release();
		}

	}

	void Renderer::Update(const Timer* pTimer)
	{
		m_Camera.Update(pTimer);

		const float meshRotation{ 45.0f * pTimer->GetElapsed() * TO_RADIANS };
		for (auto& mesh : m_pMeshes)
		{
			mesh->RotateMesh(meshRotation);
		}
	}


	void Renderer::Render() const
	{
		if (!m_IsInitialized)
			return;


		ColorRGB clearColor = ColorRGB{ 0, 0, 0.3f };
		m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, &clearColor.r);
		m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

		for (auto& mesh : m_pMeshes)
		{
			Matrix worldViewProjectionMatrix{ mesh->GetWorldMatrix() * m_Camera.viewMatrix * m_Camera.projectionMatrix };

			mesh->Render(m_pDeviceContext, worldViewProjectionMatrix, m_Camera.invViewMatrix);
		}
		


		m_pSwapChain->Present(0, 0);
	}

	void Renderer::ToggleSampleState()
	{
		for (auto& mesh : m_pMeshes)
		{
			mesh->TogleSampleState();
		}
	
	}

	HRESULT Renderer::InitializeDirectX()
	{
		//Creat device context
		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_1;
		uint32_t createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		HRESULT result = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, createDeviceFlags, &featureLevel,
			1, D3D11_SDK_VERSION, &m_pDevice, nullptr, &m_pDeviceContext);

		if (FAILED(result))
			return result;

		//Create DXGI Factory

		IDXGIFactory* pDXGIFactory{};

		result = CreateDXGIFactory1(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&pDXGIFactory));

		if (FAILED(result))
			return result;

		//Create Swapchain

		DXGI_SWAP_CHAIN_DESC swapChainDesc{};
		swapChainDesc.BufferDesc.Width = m_Width;
		swapChainDesc.BufferDesc.Height = m_Height;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 1;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 60;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.Windowed = true;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = 0;


		//Get The Handle (HWND) from SDL Backbuffer
		SDL_SysWMinfo sysWMInfo{};
		SDL_VERSION(&sysWMInfo.version);
		SDL_GetWindowWMInfo(m_pWindow, &sysWMInfo);

		swapChainDesc.OutputWindow = sysWMInfo.info.win.window;

		result = pDXGIFactory->CreateSwapChain(m_pDevice, &swapChainDesc, &m_pSwapChain);

		if (FAILED(result))
			return result;


		//Create DepthStencil & DepthStencilView

		D3D11_TEXTURE2D_DESC depthStencilDesc{};
		depthStencilDesc.Width = m_Width;
		depthStencilDesc.Height = m_Height;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;


		//View

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		depthStencilViewDesc.Format = depthStencilDesc.Format;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		result = m_pDevice->CreateTexture2D(&depthStencilDesc, nullptr, &m_pDepthStencilBuffer);

		if (FAILED(result))
			return result;

		result = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView);

		if (FAILED(result))
			return result;

		// Create Rendertarget

		//resource
		result = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_pRenderTargetBuffer));

		if (FAILED(result))
			return result;

		//view
		result = m_pDevice->CreateRenderTargetView(m_pRenderTargetBuffer, nullptr, &m_pRenderTargetView);

		if (FAILED(result))
			return result;

		//Bind RTV & DSV to Output Merger Stage

		m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

		//Set Viewport

		D3D11_VIEWPORT viewport{};
		viewport.Width = static_cast<float>(m_Width);
		viewport.Height = static_cast<float>(m_Height);
		viewport.TopLeftX = 0.f;
		viewport.TopLeftY = 0.f;
		viewport.MinDepth = 0.f;
		viewport.MaxDepth = 1.f;
		m_pDeviceContext->RSSetViewports(1, &viewport);

		if (pDXGIFactory)
		{
			pDXGIFactory->Release();
		}

		return S_OK;
	}

	Matrix Renderer::MakeWorldMatrix()
	{
		const Vector3 position{ m_Camera.origin + Vector3{ 0, 0, 50 } };
		const Vector3 rotation{ };
		const Vector3 scale{ Vector3{ 1, 1, 1 } };
		return Matrix::CreateScale(scale) * Matrix::CreateRotation(rotation) * Matrix::CreateTranslation(position);
	}

	void Renderer::SetTexturesForMesh()
	{

	}

	void Renderer::InitMeshes()
	{
#ifdef USE_OBJ


		Matrix worldMatrix = MakeWorldMatrix();

#pragma region Vehicle
		std::vector<uint32_t> indecesVehicle;
		std::vector<Vertex_In> verticesVehicle;
		Utils::ParseOBJ("Resources/vehicle.obj", verticesVehicle, indecesVehicle);
		EffectShader* pShader = new EffectShader(m_pDevice, L"Resources/Shader.fx");

		pShader->SetDiffuseMap(m_pDiffuseMap);
		pShader->SetNormalMap(m_pNormalMap);
		pShader->SetGlossinessMap(m_pGlossinessMap);
		pShader->SetSpecularMap(m_pSpecularMap);

		Mesh* pMeshVehicle = new Mesh(m_pDevice, verticesVehicle, indecesVehicle, pShader);


		pMeshVehicle->SetWorldMatrix(worldMatrix);

		m_pMeshes.emplace_back(pMeshVehicle);
#pragma endregion

#pragma region Fire
		std::vector<uint32_t> indecesFire;
		std::vector<Vertex_In> verticesFire;
		Utils::ParseOBJ("Resources/fireFX.obj", verticesFire, indecesFire);

		EffectTransparency* pTransparent = new EffectTransparency(m_pDevice, L"Resources/Transparency.fx");
		pTransparent->SetDiffuseMap(m_pFireDiffuseMap);

		Mesh* pMeshFire = new Mesh(m_pDevice, verticesFire, indecesFire, pTransparent);

		pMeshFire->SetWorldMatrix(worldMatrix);

		m_pMeshes.emplace_back(pMeshFire);
#pragma endregion




#else
		std::vector<Vertex_In> vertices{
		Vertex_In{
			{-3.f,3.f,-2.f},
			{1},
			{0, 0}
		},
		Vertex_In{
			{0.f,3.f,-2.f},
			{1},
			{0.5f, 0}},
		Vertex_In{
			{3.f,3.f,-2.f},
			{1},
			{1, 0}},
		Vertex_In{
			{-3.f,0.f,-2.f},
			{1},
			{0, 0.5f}},
		Vertex_In{
			{0.f,0.f,-2.f},
			{1},
			{0.5f, 0.5f}},
		Vertex_In{
			{3.f,0.f,-2.f},
			{1},
			{1, 0.5f}},
		Vertex_In{
			{-3.f,-3.f,-2.f},
			{1},
			{0, 1}},
		Vertex_In{
			{0.f,-3.f,-2.f},
			{1},
			{0.5f, 1}},
		Vertex_In{
			{3.f,-3.f,-2.f},
			{1},
			{1,1}}
	};


		std::vector<uint32_t> indices{
			3,0,1,	1,4,3,	4,1,2,
			2,5,4,	6,3,4,	4,7,6,
			7,4,5,	7,5,8
		};

		m_pMesh = new Mesh{ m_pDevice, vertices, indices };
#endif // DEBUG

		

	}


}
