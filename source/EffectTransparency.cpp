#include "pch.h"
#include "EffectTransparency.h"



dae::EffectTransparency::EffectTransparency(ID3D11Device* pDevice, const std::wstring& assetFile)
	:Effect(pDevice, assetFile)
{
}

dae::EffectTransparency::~EffectTransparency()
{
}
