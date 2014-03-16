uint EncodeColor(in float3 color)
{
	int3 iColor  = int3(color * 255.0f);
	uint encoded = iColor.r << 16 | iColor.g << 8 | iColor.b;

	return encoded;
}

float3 DecodeColor(in uint encoded)
{
	float3 color;
	color.r = (encoded >> 16) & 0x000000ff;
	color.g = (encoded >> 8) & 0x000000ff;
	color.b = (encoded) & 0x000000ff;

	color /= 255.0f;
	return color;
}

uint EncodeNormal(in float3 normal)
{
	int3 iNormal = int3(normal * 255.0f);

	uint3 normalSigns;
	normalSigns.x = (iNormal.x >> 5) & 0x04000000;
	normalSigns.y = (iNormal.y >> 14) & 0x0002000;
	normalSigns.z = (iNormal.z >> 23) & 0x0000010;

	iNormal = abs(iNormal);
	uint encoded = normalSigns.x | (iNormal.x << 18) | normalSigns.y | (iNormal.y << 9) | normalSigns.z | iNormal.z;
	return encoded;
}

float3 DecodeNormal(in uint encoded)
{
	int3 iNormal;
	iNormal.x = (encoded >> 18) & 0x000000ff;
	iNormal.y = (encoded >> 9) & 0x000000ff;
	iNormal.z = (encoded) & 0x000000ff;

	int3 normalSigns;
	normalSigns.x = (encoded >> 25) & 0x00000002;
	normalSigns.y = (encoded >> 16) & 0x00000002;
	normalSigns.z = (encoded >> 7) & 0x00000002;
	normalSigns = 1 - normalSigns;

	float3 normal = float3(iNormal )/ 255.0f;
	normal *= normalSigns;

	return normal;
}

//get SH-coeffs for specific direction
float SH(in float3 dir)
{
	float4 result;

	result.x = 0.2820947918f;			// 1 / (2*sqrt(PI))
	result.y = -0.4886025119f * dir.y;	// -sqrt(3/(4*PI))*y
	result.z = 0.4886025119f * dir.z;	// sqrt(3/(4*PI))*z
	result.w = -0.4886025119f * dir.x;	// -sqrt(3/(4*PI))*x

	return result;
}

float4 ClampedCosineCoeffs(in float3 dir)
{
	float4 coeffs;
	coeffs.x = 0.8862269262f;			// PI/(2*sqrt(PI))
	coeffs.y = -1.0233267079f*dir.y;	// -sqrt(Pi/3)
	coeffs.z = 1.0233267079f*dir.z;		// sqrt(Pi/3)
	coeffs.w = -1.0233267079f*dir.x;	// -1sqrt(Pi/3)

	return coeffs;
}

struct VOXEL
{
	uint color; //encoded color
	uint4 normalMasks; //encoded normal
	uint occlusion; //containes geometry only if occlusion > 0
};