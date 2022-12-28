#pragma once
#include "Effect.h"


namespace dae
{

	class EffectTransparency final :
		public Effect
	{
	public:
		EffectTransparency(ID3D11Device* pDevice, const std::wstring& assetFile);

		~EffectTransparency();
	};

}

