#pragma once

namespace dae
{
	class Effect
	{
	public:
		Effect(ID3D11Device* pDevice, const std::wstring& assetFile);
		~Effect();

		Effect(const Effect& other) = delete;
		Effect& operator=(const Effect& other) = delete;
		Effect(Effect&& other) = delete;
		Effect& operator=(Effect&& other) = delete;

		ID3DX11Effect* GetEffect() const;
		ID3DX11EffectTechnique* GetTechnique() const;
	private:
		ID3DX11Effect* m_pEffect{};
		ID3DX11EffectTechnique* m_pTechnique{};

		ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);
	};
}

