struct vs_input
{
	float3 pos	:	POSITION;
	float2 texCoord	:	TEXCOORD;
};

struct ps_input
{
	float4 pos	:	SV_POSITION;
	float2 texCoord	:	TEXCOORD;
};

cbuffer MatrixBuffer : register ( b0 )
{
	matrix WVP;
};

ps_input VS( vs_input input )
{
	ps_input output;
	output.pos = mul(float4(input.pos, 1.0f), WVP);
	output.texCoord = input.texCoord;

	return output;
}