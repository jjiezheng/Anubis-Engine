cbuffer MatrixBuffer	:	register( c0 )
{
	float4x4 WVP;
};

////////////////////////////////////////
//shaders input structures
////////////////////////////////////////
struct vs_input
{
	float3 pos		: POSITION;
	float2 texCoord : TEXCOORDS;
};

struct ps_input
{	float4 pos : SV_POSITION;
	float2 texCoord : TEXCOORD;};
ps_input VS( vs_input input )
{	ps_input output;
	
	output.pos = mul(float4(input.pos, 1.0f), WVP);
	output.texCoord = input.texCoord;
	
	return output;
}