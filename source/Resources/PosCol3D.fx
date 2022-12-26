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
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};


//------------------------------------------------
// Input/Output Struct
//------------------------------------------------
struct VS_INPUT
{
	float3 Position : POSITION;
	float3 Color : COLOR;
	float2 Uv : TEXCOORD;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float3 ColorRGB : COLOR;
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
float4 PS(VS_OUTPUT input) : SV_TARGET
{

	return float4(gDiffuseMap.Sample(samPoint, input.Uv));
}

//------------------------------------------------
// Technique
//------------------------------------------------
technique11 DefaultTechnique
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}



