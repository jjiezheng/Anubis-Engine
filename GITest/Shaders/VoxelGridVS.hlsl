struct vs_input 
{
	float3 position : POSITION;
	float2 texCoords : TEXCOORDS;
	float3 normal : NORMAL;
};

struct vs_output 
{
	float4 position : POSITION;
	float2 texCoords : TEXCOORDS;
	float3 normal : NORMAL;
};

vs_output voxelgrid_vs( vs_input input )
{
	vs_output output;

	output.position = float4(input.position, 1.0f);
	output.texCoords = input.texCoords;
	output.normal = input.normal;

	return output;
}