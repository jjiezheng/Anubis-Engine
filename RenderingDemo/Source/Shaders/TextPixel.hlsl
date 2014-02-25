struct ps_input
{
	float4 pos	:	SV_POSITION;
	float2 texCoord	:	TEXCOORD;
};

float4 PS_Inside(ps_input input) : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}

float4 PS_Convex(ps_input input) : SV_TARGET
{
	if (input.texCoord.x*input.texCoord.x <= input.texCoord.y) 
		return float4(1.0f, 1.0f, 1.0f, 1.0f); 
	else
		return float4(0.0f, 0.0f, 0.0f, 0.0f);
}

float4 PS_Concave(ps_input input) : SV_TARGET
{
	if (input.texCoord.x * input.texCoord.x > input.texCoord.y)
		return float4(1.0f, 1.0f, 1.0f, 1.0f);
	else
		return float4(0.0f, 0.0f, 0.0f, 0.0f);
}