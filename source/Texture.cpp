#include "pch.h"
#include "Texture.h"
#include "Vector2.h"
#include <SDL_image.h>
#include <cassert>

namespace dae
{
	Texture::Texture(SDL_Surface* pSurface) :
		m_pSurface{ pSurface },
		m_pSurfacePixels{ (uint32_t*)pSurface->pixels }
	{
	}

	Texture::~Texture()
	{
		if (m_pSurface)
		{
			SDL_FreeSurface(m_pSurface);
			m_pSurface = nullptr;
		}
		if (m_pShaderResourceView)
		{
			m_pShaderResourceView->Release();
		}

		if (m_pShaderResource)
		{
			m_pShaderResource->Release();
		}
	}

	Texture* Texture::LoadFromFile(const std::string& path, ID3D11Device* pDevice)
	{
		SDL_Surface* loadSurface = IMG_Load(path.c_str());

		Texture* toReturn{ new Texture{ loadSurface } };

		//if loadloadSurface == null throw assert
		assert(loadSurface && "Image failed to load.");

		DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
		D3D11_TEXTURE2D_DESC desc{};
		desc.Width = loadSurface->w;
		desc.Height = loadSurface->h;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = format;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;


		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = loadSurface->pixels;
		initData.SysMemPitch = static_cast<UINT>(loadSurface->pitch);
		initData.SysMemSlicePitch = static_cast<UINT>(loadSurface->pitch * loadSurface->h);

		HRESULT result = pDevice->CreateTexture2D(&desc, &initData, &toReturn->m_pShaderResource);

		if (FAILED(result))
		{
			delete toReturn;
			return nullptr;
		}
		
		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
		SRVDesc.Format = format;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = 1;
		
		result = pDevice->CreateShaderResourceView(toReturn->m_pShaderResource, &SRVDesc, &toReturn->m_pShaderResourceView);
		if (FAILED(result))
		{
			delete toReturn;
			return nullptr;
		}

		SDL_FreeSurface(toReturn->m_pSurface);
		toReturn->m_pSurface = nullptr;

		return toReturn;
	}

	ColorRGB Texture::Sample(const Vector2& uv) const
	{
		Uint32 x{ Uint32(uv.x * m_pSurface->w) }, y{ Uint32(uv.y * m_pSurface->h) };
		uint8_t r, g, b;

		SDL_GetRGB(m_pSurfacePixels[static_cast<uint32_t>(x + (y * m_pSurface->w))],
			m_pSurface->format,
			&r,
			&g,
			&b);

		ColorRGB pixelColor{ r / 255.f, g / 255.f, b / 255.f };
		return pixelColor;

		return {};
	}
	ID3D11ShaderResourceView* Texture::GetShaderResourceView()
	{
		return m_pShaderResourceView;
	}
}
