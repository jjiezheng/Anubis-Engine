
TextureCube skyBox	: register( t0 );
SamplerState defaultSampler : register( s0 );

//////////////////////////////////////////
//Constant buffers
cbuffer matrixBuffer : register( cb0 )
{
	matrix worldViewProj;
};

//////////////////////////////////////////
//Shader Structures
struct vs_output
{
	float4 pos		: SV_POSITION;
	float3 posMS	: POSITION_MS;
};

vs_output SkyBox_VS( float3 pos : POSITION )
{
	vs_output res;

	res.pos = mul(float4(pos, 1.0f), worldViewProj);
	res.posMS = pos;

	return res;
}

float4 SkyBox_PS( vs_output input ) : SV_Target
{
	float3 samplePos = normalize(input.posMS);
	float4 color = skyBox.Sample(defaultSampler, samplePos);
	//return float4(1.0f, 1.0f, 1.0f, 1.0f);
	return color;
}