float4x4 gWorldVieuwProj : WorldVieuwProjection;
float4x4 gWorldMatrix : WORLD;
float4x4 gVieuwInverseMatrix : VIEWINVERSE;

float3 gLightDirection = float3(0.577f, -0.577f, 0.577f);

float gShininess = float(25.f);
float gLightIntensity = float(7.f);
float gPi = float(3.14159265359);

Texture2D gDiffuseMap : DiffuseMap;
Texture2D gSpecularMap : SpecularMap;
Texture2D gNormalMap : NormalMap;
Texture2D gGlossinessMap : GlossinessMap;

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
	float4 WorldPosition : COLOR;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float2 Uv : TEXCOORD;
};

//vertex shader
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.Position = mul(float4(input.Position, 1.f), gWorldVieuwProj);
	output.WorldPosition = mul(float4(input.Position, 1.f), gWorldMatrix);
	output.Tangent = mul(normalize(input.Tangent), (float3x3)gWorldMatrix);
	output.Normal = mul(normalize(input.Normal), (float3x3)gWorldMatrix);
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
	//normal map
	float3 binormal = cross(input.Tangent, input.Normal);
	float3x3 tangentSpaceAxis = float3x3(input.Tangent, binormal, input.Normal);

	float3 normalSample = gNormalMap.Sample(samPoint, input.Uv).rgb;
	normalSample = normalSample * 2.f - 1.f;
	
	input.Normal = normalize(mul( normalSample, tangentSpaceAxis));

	//shading
	float3 vieuwDirection = normalize(input.WorldPosition.xyz - gVieuwInverseMatrix[3].xyz);
	float observedArea = dot(-input.Normal, gLightDirection);
	
	float3 finalColor = (0.f, 0.f, 0.f);

	if (observedArea > 0.f)
	{
		//calculate diffuse 
		finalColor = (gLightIntensity / gPi) * gDiffuseMap.Sample(samPoint, input.Uv).rgb * observedArea;

		float3 reflect = normalize(-gLightDirection + 2 * dot(input.Normal, gLightDirection) * input.Normal);
		float cosine = dot(reflect, vieuwDirection);

		if (cosine > 0.f)
		{
			//add the specular
			finalColor += gSpecularMap.Sample(samPoint, input.Uv).rgb * pow(cosine ,gGlossinessMap.Sample(samPoint, input.Uv).r * gShininess);
		}
		saturate(finalColor);
	}
	return float4(finalColor, 1.f);
}

float4 PSL(VS_OUTPUT input) : SV_TARGET
{
	//normal map
	float3 binormal = cross(input.Tangent, input.Normal);
	float3x3 tangentSpaceAxis = float3x3(input.Tangent, binormal, input.Normal);

	float3 normalSample = gNormalMap.Sample(samLinear, input.Uv).rgb;
	normalSample = normalSample * 2.f - 1.f;

	input.Normal = normalize(mul(normalSample, tangentSpaceAxis));

	//shading
	float3 vieuwDirection = normalize(input.WorldPosition.xyz - gVieuwInverseMatrix[3].xyz);
	float observedArea = dot(-input.Normal, gLightDirection);

	float3 finalColor = (0.f, 0.f, 0.f);

	if (observedArea > 0.f)
	{
		//calculate diffuse 
		finalColor = (gLightIntensity / gPi) * gDiffuseMap.Sample(samLinear, input.Uv).rgb * observedArea;

		float3 reflect = normalize(-gLightDirection + 2 * dot(input.Normal, gLightDirection) * input.Normal);
		float cosine = dot(reflect, vieuwDirection);

		if (cosine > 0.f)
		{
			//add the specular
			finalColor += gSpecularMap.Sample(samLinear, input.Uv).rgb * pow(cosine ,gGlossinessMap.Sample(samLinear, input.Uv).r * gShininess);
		}
		saturate(finalColor);
	}
	return float4(finalColor, 1.f);
}

float4 PSA(VS_OUTPUT input) : SV_TARGET
{
	//normal map
	float3 binormal = cross(input.Tangent, input.Normal);
	float3x3 tangentSpaceAxis = float3x3(input.Tangent, binormal, input.Normal);

	float3 normalSample = gNormalMap.Sample(samAnisotropic, input.Uv).rgb;
	normalSample = normalSample * 2.f - 1.f;

	input.Normal = normalize(mul(normalSample, tangentSpaceAxis));

	//shading
	float3 vieuwDirection = normalize(input.WorldPosition.xyz - gVieuwInverseMatrix[3].xyz);
	float observedArea = dot(-input.Normal, gLightDirection);

	float3 finalColor = (0.f, 0.f, 0.f);

	if (observedArea > 0.f)
	{
		//calculate diffuse 
		finalColor = (gLightIntensity / gPi) * gDiffuseMap.Sample(samAnisotropic, input.Uv).rgb * observedArea;

		float3 reflect = normalize(-gLightDirection + 2 * dot(input.Normal, gLightDirection) * input.Normal);
		float cosine = dot(reflect, vieuwDirection);

		if (cosine > 0.f)
		{
			//add the specular
			finalColor += gSpecularMap.Sample(samAnisotropic, input.Uv).rgb * pow(cosine ,gGlossinessMap.Sample(samAnisotropic, input.Uv).r * gShininess);
		}
		saturate(finalColor);
	}
	return float4(finalColor, 1.f);
}

RasterizerState gRasterizerState
{
	CullMode = back;
	FrontCounterClockWise = true;
};

BlendState gBlendState
{
	BlendEnable[0] = false;
};

DepthStencilState gDepthStencilState
{};

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
		SetRasterizerState(gRasterizerState);
		SetDepthStencilState(gDepthStencilState, 0);
		SetBlendState(gBlendState, float4(0.f, 0.f, 0.f, 0.f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PSL()));
	}
	pass P2
	{
		SetRasterizerState(gRasterizerState);
		SetDepthStencilState(gDepthStencilState, 0);
		SetBlendState(gBlendState, float4(0.f, 0.f, 0.f, 0.f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PSA()));
	}
}