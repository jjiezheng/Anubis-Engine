
//constant buffers
struct matrices_v : register( b0 )
{
	matrix World;
};

struct vs_output
{
	float4 posWS : POSITIONWS;
};


vs_output CubeShadowMap_VS( float3 pos : POSITION )
{
	vs_output output;

	output.posWS = mul(float4(pos, 1.0f), World)
}