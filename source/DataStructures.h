#pragma once
#include "Vector3.h"
#include "ColorRGB.h"
#include <d3dx11effect.h>
#include <d3dx11effect.h>

namespace dae
{
	class Effect;


	struct Vertex
	{
		Vector3 position;
		ColorRGB color;
		Vector2 uv;
	};


	class Mesh final
	{
	public:
		Mesh(ID3D11Device* pDevice, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
		~Mesh();

		Mesh(const Mesh& other) = delete;
		Mesh& operator=(const Mesh& other) = delete;
		Mesh(Mesh&& other) = delete;
		Mesh& operator=(Mesh&& other) = delete;

		void Render(ID3D11DeviceContext* pDeviceContext, Matrix worldView
		) const;

	private:
		Effect* m_pEffect{};

		ID3D11InputLayout* m_pInputLayout{};

		ID3D11Buffer* m_pVertexBuffer{};

		uint32_t m_NumIndices{};
		ID3D11Buffer* m_pIndexBuffer{};

	};
}

