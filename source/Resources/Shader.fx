//------------------------------------------------
// WorldViewProjection Matrix
//------------------------------------------------
float4x4 gWorldViewProj : WorldViewProjection;
float4x4 gWorldMatrix : WorldMatrix;
float4x4 gInvViewMatrix : InvViewMatrix;

//------------------------------------------------
// Textures
//------------------------------------------------
Texture2D gDiffuseMap : DiffuseMap;
Texture2D gSpecularMap : SpecularMap;
Texture2D gGlossinessMap : GlossinessMap;
Texture2D gNormalMap : NormalMap;

//------------------------------------------------
// Light
//------------------------------------------------
float3 gLightRay : LightRay = normalize(float3(0.577f, -0.577f, 0.577f));
//------------------------------------------------
// Constants
//------------------------------------------------
float gPi : PI = 3.14159265358979323846f;
float gLightIntensity : LightIntensity = 7.0f;
float gShininess : Shininess = 25.0f;

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
	BlendEnable[0] = false;
	RenderTargetWriteMask[0] = 0x0f;
};

//------------------------------------------------
// Depth Stencil
//------------------------------------------------
DepthStencilState gDepthStencilState
{
	DepthEnable = true;
	DepthWriteMask = 1;
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
	float3 Color : COLOR;
	float4 WorldPosition : WORLDPOSITION;
	float2 Uv : TEXCOORD;
	float3 Tangent : TANGENT;
	float3 Normal : NORMAL;
};


//------------------------------------------------
// Vertex Shader
//------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	float4 newPosition = float4(input.Position, 1.0f);
	output.Uv = input.Uv;
	output.Normal = mul(normalize(input.Normal), (float3x3)gWorldMatrix);
	output.Tangent = mul(normalize(input.Tangent), (float3x3)gWorldMatrix);
	output.Position = mul(newPosition, gWorldViewProj);
	output.WorldPosition = mul(newPosition, gWorldMatrix);
	return output;
}

//------------------------------------------------
// Pixel Shader
//------------------------------------------------
float4 Lambert(float kd, float4 cd)
{
	return cd * kd / gPi;
}

float Phong(float ks, float exp, float3 l, float3 v, float3 n)
{
	float3 reflectedLightVector = reflect(l,n);
	float reflectedViewDot = saturate(dot(reflectedLightVector, v));
	float phong = ks * pow(reflectedViewDot, exp);

	return phong;
}


float3 CalculateNormal(float3 normal, float3 tangent, SamplerState textureSampler, float2 uv)
{
	float3 binormal = cross(normal, tangent);
	float4x4 tangentSpaceAxis = float4x4(
		float4(tangent, 0.0f),
		float4(binormal, 0.0f),
		float4(normal, 0.0f),
		float4(0.0f, 0.0f, 0.0f, 1.0f)
		);

	float3 currentNormalMap = 2.0f * gNormalMap.Sample(textureSampler, uv).rgb - float3(1.0f, 1.0f, 1.0f);


	return mul(float4(currentNormalMap, 0.0f), tangentSpaceAxis);
}


float4 SampleColor(VS_OUTPUT input, SamplerState textureSampler)
{
	float3 normal = CalculateNormal(input.Normal, input.Tangent, textureSampler, input.Uv);

	float oa = saturate(dot(normal, -gLightRay));

	float3 viewDirection = normalize(input.WorldPosition.xyz - gInvViewMatrix[3].xyz);

	float phongExp = gShininess * gGlossinessMap.Sample(textureSampler, input.Uv).r;

	float4 specular = gSpecularMap.Sample(textureSampler, input.Uv) * Phong(1.0f, phongExp, -gLightRay, viewDirection, normal);

	float4 lambert = Lambert(1.0f, gDiffuseMap.Sample(textureSampler, input.Uv));

	return  (gLightIntensity * lambert + specular) * oa;
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
