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