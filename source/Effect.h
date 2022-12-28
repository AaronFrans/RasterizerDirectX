#pragma once

namespace dae
{
	class Texture;
	class Effect
	{
	public:

		enum class SampleState
		{
			Point,
			Linear,
			Anisotropic
		};


		Effect(ID3D11Device* pDevice, const std::wstring& assetFile);
		virtual ~Effect();

		Effect(const Effect& other) = delete;
		Effect& operator=(const Effect& other) = delete;
		Effect(Effect&& other) = delete;
		Effect& operator=(Effect&& other) = delete;

		ID3DX11Effect* GetEffect() const;
		ID3DX11EffectTechnique* GetTechnique() const;

		void SetWorldViewProjMatrixData(Matrix worldViewProjectionMatrix);
		void SetWorldMatrixData(Matrix worldMatrix);
		void SetInvViewMatrixData(Matrix invViewMatrix);

		void SetDiffuseMap(Texture* pTexture);

		void TogleSampleState();

	protected:

		SampleState m_SampleState{SampleState::Anisotropic};


		ID3DX11Effect* m_pEffect{};
		ID3DX11EffectTechnique* m_pPointTechnique{};
		ID3DX11EffectTechnique* m_pLinearTechnique{};
		ID3DX11EffectTechnique* m_pAnisptropicTechnique{};

		ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable{};
		ID3DX11EffectMatrixVariable* m_pMatWorldMatrixVariable{};
		ID3DX11EffectMatrixVariable* m_pMatInvViewMatrixVariable{};

		ID3DX11EffectShaderResourceVariable* m_pDiffuseMapResourceVariable{};

		ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);
	};
}

