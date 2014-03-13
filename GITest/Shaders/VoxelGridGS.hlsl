struct vs_output 
{
	float4 position : POSITION;
	float2 texCoords : TEXCOORDS;
	float3 normal : NORMAL;
};

struct gs_output
{
	float4 pos : SV_POSITION;
	float3 posWS : POSITION;
	float2 texCoords : TEXCOORDS;
	float3 normal : NORMAL;
};

static float3 viewDirections[3] = 
{
	float3(0.0f, 0.0f, -1.0f), //back to front
	float3(-1.0f, 0.0f, 0.0f), //right to left
	float3(0.0f, -1.0f, 0.0f) //top to down
};

cbuffer MatrixBuffer : register( cb0 )
{
	matrix gridViewProjMatrices[3];
};

int GetViewIndex(in float3 normal)
{
	float3x3 directionsMatrix;
	directionsMatrix[0] = -viewDirections[0];
	directionsMatrix[1] = -viewDirections[1];
	directionsMatrix[2] = -viewDirections[2];

	float3 dotProducts = abs(mul(directionsMatrix, normal));
	float maximum = max(max(dotProducts.x, dotProducts.y), dotProducts.z);

	if (maximum == dotProducts.x)
		return 0;
	else if (maximum == dotProducts.y)
		return 1;
	else 
		return 2;
}

[maxvertexcount(3)]
void voxelgrid_gs(triangle vs_output input[3], inout TriangleStream< gs_output > outputStream)
{
	float3 faceNormal = normalize(input[0].normal + input[1].normal + input[2].normal);

	int viewIndex = GetViewIndex(faceNormal);

	gs_output output[3];
	[unroll]
	for (uint i = 0; i < 3; i++)
	{
		output[i].pos = mul(gridViewProjMatrices[viewIndex], input[i].position);
		output[i].posWS = input[i].position.xyz;
		output[i].texCoords = input[i].texCoords;
		output[i].normal = input[i].normal;
	}

	//increase triangle size in normalized device coordinates by the texel size
	float2 side0 = normalize(output[1].pos.xy - output[0].pos.xy);
	float2 side1 = normalize(output[2].pos.xy - output[1].pos.xy);
	float2 side2 = normalize(output[0].pos.xy - output[2].pos.xy);

	float texelSize = 1.f / 64.0f;
	output[0].pos.xy += normalize(side2 - side0)*texelSize;
	output[1].pos.xy += normalize(side0 - side1)*texelSize;
	output[2].pos.xy += normalize(side1 - side2)*texelSize;

	[unroll]
	for(uint i = 0; i < 3; i++)
		outputStream.Append(output[i]);

	outputStream.RestartStrip();
}