float4x4 gWorldVieuwProj : WorldVieuwProjection;

Texture2D gDiffuseMap : DiffuseMap;

//input output sturcts
struct VS_INPUT
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float2 Uv : TEXCOORD;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float2 Uv : TEXCOORD;
};

//vertex shader
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.Position = mul(float4(input.Position, 1.f), gWorldVieuwProj);
	output.Uv = input.Uv;

	return output;
}

//samplerstates
SamplerState samPoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Border; //or Mirror or Clamp or Border
	AddressV = Clamp; //or Mirror or Clamp or Border.
	BorderColor = float4(0.0f, 0.0f, 1.0f, 1.0f);
};

SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Border; //or Mirror or Clamp or Border
	AddressV = Clamp; //or Mirror or Clamp or Border.
	BorderColor = float4(0.0f, 0.0f, 1.0f, 1.0f);
};

SamplerState samAnisotropic
{
	Filter = ANISOTROPIC;

	AddressU = Border; //or Mirror or Clamp or Border
	AddressV = Clamp; //or Mirror or Clamp or Border.
	BorderColor = float4(0.0f, 0.0f, 1.0f, 1.0f);
};

//pixel shader
float4 PSP(VS_OUTPUT input) : SV_TARGET
{
		return  gDiffuseMap.Sample(samPoint, input.Uv);
}

float4 PSL(VS_OUTPUT input) : SV_TARGET
{
		return  gDiffuseMap.Sample(samLinear, input.Uv);
}

float4 PSA(VS_OUTPUT input) : SV_TARGET
{
		return  gDiffuseMap.Sample(samAnisotropic, input.Uv);
}

RasterizerState gRasterizerState
{
	CullMode = none;
	FrontCounterClockWise = true;
};

BlendState gBlendState
{
	BlendEnable[0] = true;
	SrcBlend = src_alpha;
	DestBlend = inv_src_alpha;
	BlendOp = add;
	SrcBlendAlpha = zero;
	DestBlendAlpha = zero;
	BlendOpAlpha = add;
	RenderTargetWriteMask[0] = 0x0F;
};

DepthStencilState gDepthStencilState
{
	DepthEnable = true;
	DepthWriteMask = zero;
	DepthFunc = Less;
	StencilEnable = false;

	StencilReadMask = 0x0F;
	StencilWriteMask = 0x0F;

	FrontFaceStencilFunc = always;
	BackFaceStencilFunc = always;

	FrontFaceStencilDepthFail = keep;
	BackFaceStencilDepthFail = keep;

	FrontFaceStencilPass = keep;
	BackFaceStencilPass = keep;

	FrontFaceStencilFail = keep;
	BackFaceStencilFail = keep;
};
//technique
technique11 DefaultTechnique
{
	pass P0
	{
		SetRasterizerState(gRasterizerState);
		SetDepthStencilState(gDepthStencilState, 0);
		SetBlendState(gBlendState, float4(0.f, 0.f, 0.f, 0.f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PSP()));
	}
	pass P1
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetDepthStencilState(gDepthStencilState, 0);
		SetBlendState(gBlendState, float4(0.f, 0.f, 0.f, 0.f), 0xFFFFFFFF);
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PSL()));
	}
	pass P2
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetDepthStencilState(gDepthStencilState, 0);
		SetBlendState(gBlendState, float4(0.f, 0.f, 0.f, 0.f), 0xFFFFFFFF);
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PSA()));
	}
}