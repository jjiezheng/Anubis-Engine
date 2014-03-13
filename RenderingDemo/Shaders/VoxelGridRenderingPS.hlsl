#include "GlobalIllumination.hlsli"

struct vs_output
{
	float4 pos : SV_POSITION;
	float3 posWS : POSITION_WS;
};

struct VOXEL
{
	uint color; //encoded color
	uint4 normalMasks; //encoded normal
	uint occlusion; //containes geometry only if occlusion > 0
};

StructuredBuffer<VOXEL> voxelGrid : register( t0 );

//cbuffer GridBuffer : register( b0 )
//{
	//float3 gridCenter;
	//float  invCellSize;
//};

float4 showgrid_ps(vs_output input) : SV_TARGET
{
	float3 gridCenter = float3(0.0f, 500.0f, 0.0f);
	float  invCellSize = 1.0f / 70.5f;

	float3 offset = (input.posWS - gridCenter) * invCellSize;
	offset = round(offset);

	int3 gridPos = int3(16,16,16) + int3(offset);
	uint gridIndex = gridPos.z * 1024 + gridPos.y * 32 + gridPos.x;

	float3 color = float3(0.5f, 0.5f, 0.5f);

	if (gridPos.x < 0 || gridPos.x >= 32)
		color = float3(1.0f, 0.0f, 0.0f);
	else if(gridPos.y < 0 || gridPos.y >= 32)
		color = float3(0.0f, 1.0f, 0.0f);
	else if(gridPos.z < 0 || gridPos.z >= 32)
		color = float3(0.0f, 0.0f, 1.0f);
	else
		color = DecodeColor(voxelGrid[gridIndex].color);
		
	//return float4(1.0f, 1.0f, 1.0f, 1.0f);
	return float4(color, 1.0f);
}