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

StructuredBuffer<VOXEL> voxelGrid : register( u0 );

cbuffer GridBuffer : register( b0 )
{
	float3 gridCenter;
	float  invCellSize;
};

float4 showgrid_ps(vs_output input) : SV_TARGET
{

	float3 offset = (input.posWS - gridCenter) * invCellSize;
	offset = round(offset);

	int3 gridPos = int3(16,16,16) + int3(offset);
	uint gridIndex = gridPos.z * 1024 + gridPos.y * 32 + gridPos.x;
	
	float4 color = DecodeColor(voxelGrid[gridIndex].color);

	return color;
}