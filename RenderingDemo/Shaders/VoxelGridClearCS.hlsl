#include "GlobalIllumination.hlsli"

RWStructuredBuffer<VOXEL> voxelGrid : register( u0 );

[numthreads(8, 8, 8)]
void voxel_grid_clearCS( uint3 threadId : SV_DispatchThreadID )
{
	VOXEL clean;

	clean.color = 0;
	clean.normalMasks = uint4(0,0,0,0);
	clean.occlusion = 0;

	int gridIndex = threadId.z * 4096 + threadId.y * 64 + threadId.x;
	voxelGrid[gridIndex] = clean;
}