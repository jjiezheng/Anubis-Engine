cbuffer matrices_g : register( b0 )
{
	matrix View[6];
	matrix Projection;
};

struct vs_output
{
	float4 posWS : POSITIONWS;
};

struct gs_output
{
	float4 pos : SV_POSITION;
	float3 lightVec : TEXCOORD0;
	uint RTIndex : SV_RenderTargetArrayIndex;
};

[maxvertexcount(18)]
void CubeShadowMap_GS(
	triangle vs_output input[3], 
	inout TriangleStream< gs_output  > CubeShadowMapStream
)
{
	for (uint f = 0; f < 6; f++)
	{
		gs_output output;
		output.RTIndex = f;
		for (int i = 0; i < 3; i++)
		{
			float4 viewPos = mul(input[i].posWS, View[f]);
			output.lightVec = float3(viewPos.x, viewPos.y, viewPos.z);
			//output.lightVec = float3(60.0f, 0.0f, 0.0f);
			output.pos = mul(viewPos, Projection);
			
			CubeShadowMapStream.Append(output);
		}
		CubeShadowMapStream.RestartStrip();
	}
}