#include "pch.h"
#include "EffectShader.h"
#include "Texture.h"

dae::EffectShader::EffectShader(ID3D11Device* pDevice, const std::wstring& assetFile)
	:Effect(pDevice, assetFile)
{

#pragma region Matrix
	m_pMatWorldMatrixVariable = m_pEffect->GetVariableByName("gWorldMatrix")->AsMatrix();
	if (!m_pMatWorldMatrixVariable->IsValid()) std::wcout << L"WorldMatrix not valid\n";

	m_pMatInvViewMatrixVariable = m_pEffect->GetVariableByName("gWorldMatrix")->AsMatrix();
	if (!m_pMatInvViewMatrixVariable->IsValid()) std::wcout << L"InvViewMatrix not valid\n";
#pragma endregion



#pragma region Shaders
	m_pNormalResourceVariable = m_pEffect->GetVariableByName("gNormalMap")->AsShaderResource();
	if (!m_pNormalResourceVariable->IsValid()) std::wcout << L"NormalMap not valid\n";

	m_pGlossinessMapResourceVariable = m_pEffect->GetVariableByName("gGlossinessMap")->AsShaderResource();
	if (!m_pGlossinessMapResourceVariable->IsValid()) std::wcout << L"GlossinessMap not valid\n";

	m_pSpecularMapResourceVariable = m_pEffect->GetVariableByName("gSpecularMap")->AsShaderResource();
	if (!m_pSpecularMapResourceVariable->IsValid()) std::wcout << L"SpecularMap not valid\n";
#pragma endregion
}

dae::EffectShader::~EffectShader()
{
}

void dae::EffectShader::SetGlossinessMap(Texture* pTexture)
{
	if (m_pGlossinessMapResourceVariable)
	{
		m_pGlossinessMapResourceVariable->SetResource(pTexture->GetShaderResourceView());
	}
}

void dae::EffectShader::SetNormalMap(Texture* pTexture)
{
	if (m_pNormalResourceVariable)
	{
		m_pNormalResourceVariable->SetResource(pTexture->GetShaderResourceView());
	}
}

void dae::EffectShader::SetSpecularMap(Texture* pTexture)
{
	if (m_pSpecularMapResourceVariable)
	{
		m_pSpecularMapResourceVariable->SetResource(pTexture->GetShaderResourceView());
	}
}
