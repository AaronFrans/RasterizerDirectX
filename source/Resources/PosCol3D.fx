//------------------------------------------------
// WorldViewProjection Matrix
//------------------------------------------------
float4x4 gWorldViewProj : WorldViewProjection;

//------------------------------------------------
// Input/Output Struct
//------------------------------------------------
struct VS_INPUT
{
	float3 Position : POSITION;
	float3 Color : COLOR;
	float2 Uv : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float3 Color : COLOR;
	float2 Uv : TEXCOORD0;
};


//------------------------------------------------
// Vertex Shader
//------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	float4 newPosition = float4(input.Position, 1.0f);
	output.Position = mul(newPosition, gWorldViewProj);
	output.Color = input.Color;
	output.Uv = input.Uv;
	return output;
}

//------------------------------------------------
// Pixel Shader
//------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_TARGET
{
	return float4(input.Color, 1.0f);
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

