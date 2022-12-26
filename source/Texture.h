#pragma once
#include <SDL_surface.h>
#include <string>
#include "ColorRGB.h"
#include <d3d11.h>

namespace dae
{
	struct Vector2;

	class Texture
	{
	public:
		~Texture();

		static Texture* LoadFromFile(const std::string& path, ID3D11Device* pDevice);
		ColorRGB Sample(const Vector2& uv) const;

		ID3D11ShaderResourceView* GetShaderResourceView();

	private:
		Texture(SDL_Surface* pSurface);

		SDL_Surface* m_pSurface{ nullptr };
		ID3D11ShaderResourceView* m_pShaderResourceView{};
		ID3D11Texture2D* m_pShaderResource{};

		uint32_t* m_pSurfacePixels{ nullptr };
	};
}
