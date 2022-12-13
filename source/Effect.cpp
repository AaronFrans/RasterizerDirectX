#include "pch.h"
#include "Effect.h"

namespace dae
{
	Effect::Effect(ID3D11Device* pDevice, const std::wstring& assetFile)
		:m_pEffect{ LoadEffect(pDevice, assetFile) }
	{
		m_pTechnique = m_pEffect->GetTechniqueByName("DefaultTechnique");
		if (!m_pTechnique->IsValid()) std::wcout << L"Technique not valid\n";

		m_pMatWorldViewProjVariable = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
		if (!m_pMatWorldViewProjVariable->IsValid()) std::wcout << L"Technique not valid\n";
	}

	Effect::~Effect()
	{
		if (m_pEffect) m_pEffect->Release();
	}

	ID3DX11Effect* Effect::GetEffect() const
	{
		return m_pEffect;
	}

	ID3DX11EffectTechnique* Effect::GetTechnique() const
	{
		return m_pTechnique;
	}

	void Effect::SetMatrixData(Matrix worldViewProjectionMatrix)
	{
		m_pMatWorldViewProjVariable->SetMatrix(Matrix::CreateDirectXMatrix(worldViewProjectionMatrix));
	}

	ID3DX11Effect* Effect::LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
	{
		HRESULT result;
		ID3D10Blob* pErrorBlob{ nullptr };
		ID3DX11Effect* pEffect;

		DWORD shaderFlags{ 0 };

#if defined(DEBUG) || defined(_DEBUG)
		shaderFlags |= D3DCOMPILE_DEBUG;
		shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		result = D3DX11CompileEffectFromFile
		(
			assetFile.c_str(),
			nullptr,
			nullptr,
			shaderFlags,
			0,
			pDevice,
			&pEffect,
			&pErrorBlob
		);

		if (FAILED(result))
		{
			if (pErrorBlob != nullptr)
			{
				const char* pErrors{ static_cast<char*>(pErrorBlob->GetBufferPointer()) };

				std::wstringstream ss;
				for (unsigned int i{}; i < pErrorBlob->GetBufferSize(); ++i)
				{
					ss << pErrors[i];
				}

				OutputDebugStringW(ss.str().c_str());
				pErrorBlob->Release();
				pErrorBlob = nullptr;

				std::wcout << ss.str() << "\n";
			}
			else
			{
				std::wstringstream ss;
				ss << "EffectLoader: Failed to CreateEffectFromFile!\nPath: " << assetFile;
				std::wcout << ss.str() << "\n";
				return nullptr;
			}
		}

		return pEffect;
	}

}
