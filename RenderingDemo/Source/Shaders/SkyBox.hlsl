
TextureCube skyBox	: register( t0 );
SamplerState defaultSampler : register( s0 );

//////////////////////////////////////////
//Constant buffers
cbuffer matrixBuffer
{
	matrix worldViewProj;
};

//////////////////////////////////////////
//Shader Structures
struct vs_output
{
	float4 pos		: SV_POSITION;
	float3 posWS	: POSITION_WS;
};

vs_output SkyBox_VS( float3 pos : POSITION )
{
	vs_output res;

	res.pos = mul(float4(pos, 1.0f), worldViewProj);
	res.posWS = pos;

	return res;
}

float4 SkyBox_PS( vs_output input ) : SV_Target
{
	float3 samplePos = normalize(input.posWS);
	float4 color = skyBox.Sample(defaultSampler, samplePos);

	return color;
}