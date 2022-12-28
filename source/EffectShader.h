#pragma once
#include "Effect.h"

namespace dae 
{

    class EffectShader :
        public Effect
    {

    public:

        EffectShader(ID3D11Device* pDevice, const std::wstring& assetFile);
        ~EffectShader();



        void SetGlossinessMap(Texture* pTexture);
        void SetNormalMap(Texture* pTexture);
        void SetSpecularMap(Texture* pTexture);


    private:


        ID3DX11EffectShaderResourceVariable* m_pNormalResourceVariable{};
        ID3DX11EffectShaderResourceVariable* m_pGlossinessMapResourceVariable{};
        ID3DX11EffectShaderResourceVariable* m_pSpecularMapResourceVariable{};
    };
}

