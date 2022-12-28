//------------------------------------------------
// WorldViewProjection Matrix
//------------------------------------------------
float4x4 gWorldViewProj : WorldViewProjection;

//------------------------------------------------
// Textures
//------------------------------------------------
Texture2D gDiffuseMap : DiffuseMap;



//------------------------------------------------
// Sampling
//------------------------------------------------
SamplerState samPoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Wrap;
	AddressV = Wrap;
};

SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

SamplerState samAnisptropic
{
	Filter = ANISOTROPIC;
	AddressU = Wrap;
	AddressV = Wrap;
};


//------------------------------------------------
// Rasterizing
//------------------------------------------------
RasterizerState gRasterizerState
{
	CullMode = none;
	FrontCounterClockwise = false;
};

//------------------------------------------------
// Blending
//------------------------------------------------
BlendState gBlendState
{
	BlendEnable[0] = true;
	SrcBlend = src_alpha;
	DestBlend = inv_src_alpha;
	BlendOp = add;
	SrcBlendAlpha = zero;
	DestBlendAlpha = zero;
	BlendOpAlpha = add;
	RenderTargetWriteMask[0] = 0x0f;
};

//------------------------------------------------
// Depth Stencil
//------------------------------------------------
DepthStencilState gDepthStencilState
{
	DepthEnable = true;
	DepthWriteMask = zero;
	DepthFunc = less;
	StencilEnable = false;
};

//------------------------------------------------
// Input/Output Struct
//------------------------------------------------
struct VS_INPUT
{
	float3 Position : POSITION;
	float3 Color : COLOR;
	float2 Uv : TEXCOORD;
	float3 Tangent : TANGENT;
	float3 Normal : NORMAL;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float2 Uv : TEXCOORD;
};


//------------------------------------------------
// Vertex Shader
//------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	float4 newPosition = float4(input.Position, 1.0f);
	output.Uv = input.Uv;
	output.Position = mul(newPosition, gWorldViewProj);
	return output;
}

//------------------------------------------------
// Pixel Shader
//------------------------------------------------



float4 SampleColor(VS_OUTPUT input, SamplerState textureSampler)
{
	return  gDiffuseMap.Sample(textureSampler, input.Uv);
}



float4 PSPoint(VS_OUTPUT input) : SV_TARGET
{
	return SampleColor(input, samPoint);
}

float4 PSLinear(VS_OUTPUT input) : SV_TARGET
{ 
	return SampleColor(input, samLinear);
}

float4 PSAnisptropic(VS_OUTPUT input) : SV_TARGET
{

	return SampleColor(input, samAnisptropic);
}

//------------------------------------------------
// Technique
//------------------------------------------------
technique11 PointTechnique
{
	pass P0
	{
		SetRasterizerState(gRasterizerState);
		SetDepthStencilState(gDepthStencilState, 0);
		SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PSPoint()));
	}
}

technique11 LinearTechnique
{
	pass P0
	{
		SetRasterizerState(gRasterizerState);
		SetDepthStencilState(gDepthStencilState, 0);
		SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PSLinear()));
	}
}

technique11 AnisptropicTechnique
{
	pass P0
	{
		SetRasterizerState(gRasterizerState);
		SetDepthStencilState(gDepthStencilState, 0);
		SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PSAnisptropic()));
	}
}
