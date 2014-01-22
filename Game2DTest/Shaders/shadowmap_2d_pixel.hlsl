#define Pi 3.14
#define THRESHOLD 0.75

Texture2D<float4> occlusionMap : register( t0 );
SamplerState linearSampler : register ( s0 );

cbuffer DataBuffer
{
	float4 resolution;
};

struct ps_input
{
	float4 pos : SV_POSITION;
	float2 texCoord : TEXCOORD;
};

float4 PS(ps_input input) : SV_TARGET
{
	float distance = 1.0f;
	float num = resolution.y;

	[unroll(256)]
	for (float y = 0.0; y < 256; y+= 1.0f)
	{
		//rectangular to polar
		float2 norm = float2(input.texCoord.x, y/256) * 2.0f - 1.0f;
		float theta = Pi*1.5 + norm.x * Pi;
		float r = (1.0f + norm.y) * 0.5;
		
		//coord to sample from Occlusion map
		float2 coord = float2(-r * sin(theta), -r * cos(theta)) / 2 + 0.5f;
	
		//sample occlusion map
		float4 occ = occlusionMap.Sample(linearSampler, coord);
		
		//current distance
		float dst = y / 256;
		
		//check if we hit new object
		float caster = occ.a;
		//if (caster > THRESHOLD)
		{
			distance = min(distance, dst);
		}
	}
	
	return float4((float3)distance, 1.0f);
}