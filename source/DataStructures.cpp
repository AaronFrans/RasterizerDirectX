#include "pch.h"
#include "DataStructures.h"
#include "EffectShader.h"
#include "EffectTransparency.h"
#include "Texture.h"
#include <tchar.h>


namespace dae
{
	Mesh::Mesh(ID3D11Device* pDevice, const std::vector<Vertex_In>& vertices, const std::vector<uint32_t>& indices, Effect* effect)
		:m_pEffect{ effect}
	{
		static constexpr uint32_t numElements{ 5 };
		D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

		vertexDesc[0].SemanticName = "POSITION";
		vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[0].AlignedByteOffset = 0;
		vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[1].SemanticName = "COLOR";
		vertexDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[2].SemanticName = "TEXCOORD";
		vertexDesc[2].Format = DXGI_FORMAT_R32G32_FLOAT;
		vertexDesc[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[3].SemanticName = "TANGENT";
		vertexDesc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		vertexDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[4].SemanticName = "NORMAL";
		vertexDesc[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		vertexDesc[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		D3DX11_PASS_DESC passDesc{};
		m_pEffect->GetTechnique()->GetPassByIndex(0)->GetDesc(&passDesc);

		HRESULT result{ pDevice->CreateInputLayout
			(
				vertexDesc,
				numElements,
				passDesc.pIAInputSignature,
				passDesc.IAInputSignatureSize,
				&m_pInputLayout
			) };
		if (FAILED(result)) return;

		D3D11_BUFFER_DESC bd{};
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(Vertex_In) * static_cast<uint32_t>(vertices.size());
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData{};
		initData.pSysMem = vertices.data();

		result = pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
		if (FAILED(result)) return;

		m_NumIndices = static_cast<uint32_t>(indices.size());
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(uint32_t) * m_NumIndices;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		initData.pSysMem = indices.data();


		result = pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);
		if (FAILED(result)) return;

	}

	Mesh::~Mesh()
	{

		delete m_pEffect;

		if (m_pIndexBuffer) 
			m_pIndexBuffer->Release();
		if (m_pVertexBuffer) 
			m_pVertexBuffer->Release();

		if (m_pInputLayout)
			m_pInputLayout->Release();
	}

	void Mesh::Render(ID3D11DeviceContext* pDeviceContext, Matrix worldViewProj, Matrix invView) const
	{

		m_pEffect->SetWorldViewProjMatrixData(worldViewProj);
		m_pEffect->SetWorldMatrixData(m_WorldMatrix);

		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		pDeviceContext->IASetInputLayout(m_pInputLayout);

		constexpr UINT stride{ sizeof(Vertex_In) };
		constexpr UINT offset{};
		pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

		pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);


		D3DX11_TECHNIQUE_DESC techniqueDesc{};
		m_pEffect->GetTechnique()->GetDesc(&techniqueDesc);
		for (UINT p{}; p < techniqueDesc.Passes; ++p)
		{
			m_pEffect->GetTechnique()->GetPassByIndex(p)->Apply(0, pDeviceContext);
			pDeviceContext->DrawIndexed(m_NumIndices, 0, 0);
		}
	}

	

	void Mesh::TogleSampleState()
	{
		m_pEffect->TogleSampleState();
	}

	Matrix Mesh::GetWorldMatrix()
	{
		return m_WorldMatrix;
	}

	void Mesh::SetWorldMatrix(Matrix& worldMatrix)
	{

		m_WorldMatrix = worldMatrix;
	}

	void Mesh::RotateMesh(float rotation)
	{
		m_WorldMatrix = Matrix::CreateRotationY(rotation) * m_WorldMatrix;
	}





}

