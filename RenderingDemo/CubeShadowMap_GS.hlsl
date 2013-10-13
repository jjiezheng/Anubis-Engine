struct matrices_g : register( b0 )
{
	matrix View[6];
	matrix Projection;
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
			output.lightVec = mul(input[v].posWS, View[f]);
			output.pos = mul(output.lightVec, Projection);
			
			CubeShadowMapStream.Append(output);
		}
		CubeShadowMapStream.RestartStrip();
	}
}