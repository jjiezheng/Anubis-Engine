
cbuffer Matrix : register( b0 )
{
	matrix WorldView;
	matrix WorldViewProj;
};

struct ps_input
{
	float4 posCS : SV_POSITION;
	float4 posVS : POSITION_VS;
};

ps_input zprepass_vs( float3 pos : POSITION )
{
	ps_input output;

	output.posCS = mul(float4(pos, 1.0f), WorldViewProj);
	output.posVS = mul(float4(pos, 1.0f), WorldView);

	return output;
}

float4 zprepass_ps( ps_input input ) : SV_Target
{
	float depth = length(input.posVS.xyz);
	return float4(depth, depth, depth, 1.0f);
}