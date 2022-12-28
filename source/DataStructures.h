#pragma once
#include "Vector3.h"
#include "ColorRGB.h"
#include <d3dx11effect.h>

namespace dae
{
	class Effect;
	class Texture;


	struct Vertex_In
	{
		Vector3 position;
		ColorRGB color;
		Vector2 uv;
		Vector3 tangent;
		Vector3 normal;
	};
	
	struct Vertex_Out
	{
		Vector3 position;
		ColorRGB color;
		Vector2 uv;
		Vector3 tangent;
		Vector3 normal;
	};


	class Mesh final
	{
	public:
		Mesh(ID3D11Device* pDevice, const std::vector<Vertex_In>& vertices, const std::vector<uint32_t>& indices, Effect* effect);
		~Mesh();

		Mesh(const Mesh& other) = delete;
		Mesh& operator=(const Mesh& other) = delete;
		Mesh(Mesh&& other) = delete;
		Mesh& operator=(Mesh&& other) = delete;

		void Render(ID3D11DeviceContext* pDeviceContext, Matrix worldView, Matrix invView) const;

		void TogleSampleState();

		Matrix GetWorldMatrix();
		void  SetWorldMatrix(Matrix& m_WorldMatrix);

		void RotateMesh(float rotation);

	private:
		Effect* m_pEffect{};

		ID3D11InputLayout* m_pInputLayout{};

		ID3D11Buffer* m_pVertexBuffer{};

		uint32_t m_NumIndices{};
		ID3D11Buffer* m_pIndexBuffer{};

		Matrix m_WorldMatrix{};
	};
}

