cbuffer LightProperties : register( b0 )
{
	float4 lightFrustumDepth;
}

struct gs_output
{
	float4 pos : SV_POSITION;
	float3 lightVec : TEXCOORD0;
	uint RTIndex : SV_RenderTargetArrayIndex;
};

float4 CubeShadowMap_PS( gs_output input ) : SV_Target
{
	float depth = length(input.lightVec) / lightFrustumDepth.x;

	return float4(depth, depth * depth, 0.0f, 1.0f);
}