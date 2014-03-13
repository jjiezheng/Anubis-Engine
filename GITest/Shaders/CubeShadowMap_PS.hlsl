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
	float depth = length(input.lightVec) / 79.5f; //lightFrustumDepth.x;
	
	//calculate depth partial derivatives
	float dx = ddx(depth);
	float dy = ddy(depth);

	float moment1 = depth;
	float moment2 = depth*depth + 0.25f*(dx*dx + dy*dy);

	return float4(moment1, moment2, 0.0f, 1.0f);
}