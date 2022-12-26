#pragma once

struct SDL_Window;
struct SDL_Surface;


#include "Camera.h"
#include "DataStructures.h"

namespace dae
{

	class Mesh;
	class Texture;

	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Update(const Timer* pTimer);
		void Render() const;

	private:
		SDL_Window* m_pWindow{};

		int m_Width{};
		int m_Height{};

		bool m_IsInitialized{ false };

		ID3D11Device* m_pDevice{ nullptr };
		ID3D11DeviceContext* m_pDeviceContext{ nullptr };
		IDXGISwapChain* m_pSwapChain{ nullptr };
		ID3D11Texture2D* m_pDepthStencilBuffer{ nullptr };
		ID3D11DepthStencilView* m_pDepthStencilView{ nullptr };
		ID3D11Resource* m_pRenderTargetBuffer{ nullptr };
		ID3D11RenderTargetView* m_pRenderTargetView{ nullptr };

		Mesh* m_pMesh{ nullptr };

		Camera m_Camera;


		Texture* m_pDiffuseMap{ };


		//DIRECTX
		HRESULT InitializeDirectX();
		Matrix MakeWorldMatrix();
		void SetTexturesForMesh();

		//...
	};
}
