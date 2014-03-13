#include "GlobalIlluminations.hlsli"

struct gs_output
{
	float4 pos : SV_POSITION;
	float3 posWS : POSITION;
	float2 texCoords : TEXCOORDS;
	float3 normal : NORMAL;
};

struct VOXEL
{
	uint color; //encoded color
	uint4 normalMasks; //encoded normal
	uint occlusion; //containes geometry only if occlusion > 0
};

Texture2D<float4> diffuseAlbedo : register ( t0 );
RWStructuredBuffer<VOXEL> voxelGrid : register( u0 );
cbuffer GridBuffer : register( b0 )
{
	float3 gridCenter;
	float  invCellSize;
};

static float3 faceVectors[4] = 
{
	float3(0.0f, -0.57735026f, 0.81649661f),
	float3(0.0f, -0.57735026f, -0.81649661f),
	float3(-0.81649661f, 0.57735026f, 0.0f),
	float3(0.81649661f, 0.57735026f, 0.0f)
};

int GetNormalIndex(in float3 normal, float dotProduct)
{
	float4x3 directionsMatrix;
	directionsMatrix[0] = faceVectors[0];
	directionsMatrix[1] = faceVectors[1];
	directionsMatrix[2] = faceVectors[2];
	directionsMatrix[3] = faceVectors[3];

	float4 dotProducts = mul(directionsMatrix, normal);
	float maximum = max(max(dotProducts.x, dotProducts.y), max(dotProducts.z, dotProducts.w));

	uint index;
	if (maximum == dotProducts.x)
		index = 0;
	else if (maximum == dotProducts.y)
		index = 1;
	else if (maximum == dotProducts.z)
		index = 2;
	else if (maximum == dotProducts.w)
		index = 3;

	dotProduct = dotProducts[index];
	return index;
}

void voxelgrid_ps(gs_output input)
{
	float3 color = diffuseAlbedo.Sample(samplerState, input.texCoords).xyz;

	//encode color to lower 24 bits of mask
	uint colorMask = EncodeColor(color);

	//encode cotrast to higher 8 bits of memory(high contrast values preferred)
	float contrast = length(base.rrg - base.bgg) / (sqrt(2.0f) + base.r + base.g + base.b);
	int iContrast = int(contrast*255.0f);
	colorMask |= iContrast << 24;

	//encode normal to lower 27 bits of uint using 8 bit for value and 1 bit for sign
	float3 normal = normalize(input.normal);
	uint normalMask = EncodeNormal(normal);

	//calc to which face of tetrahedron the normal is closest and write dotProduct to highest 5 bits
	float dotProduct;
	uint normalIndex = GetNormalIndex(normal, dotProduct);
	int iDotProduct = int(dotProduct);
	normalMask |= iDotProduct << 27;

	float3 offset = (input.posWS - gridCenter) * invCellSize;
	offset = round(offset);

	int3 gridPos = int3(16,16,16) + int3(offset);

	if ( (voxelPos.x > -1) && (voxelPos.x < 32) &&
		 (voxelPos.y > -1) && (voxelPos.y < 32) &&
		 (voxelPos.z > -1) && (voxelPos.z < 32) )
	{
		uint gridIndex = gridPos.z * 1024 + gridPos.y * 32 + gridPos.x;

		InterlockedMax(voxelGrid[gridIndex].color, colorMask);

		InterlockedMax(voxelGrid[gridIndex].normalMasks[normalIndex], normalMask);

		InterlockedMax(voxelGrid[gridIndex].occlusion, 1);
	}
}