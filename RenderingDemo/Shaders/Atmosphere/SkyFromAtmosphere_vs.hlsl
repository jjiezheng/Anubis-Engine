#include "Common.hlsli"


//////////////////////////////////////////
//constant buffers
cbuffer atmosphere : register( b0 )
{
	/*float3	lightPos;			//directional vector to the light source
	float	scale;				// 1 / (outerRadius - innerRadius)
	float3	invWavelength;		// 1 / pow(wavelength, 4)
	float	scaleOverScaleDepth;// scale / scaleDepth;
	float	cameraHeight;
	float	cameraHeight2;
	float	outerRadius;
	float	outerRadius2;
	float	innerRadius;
	float	innerRadius2;
	float	KrESun;				// Kr * Sun
	float	KmESun;				// Km * Sun
	float	Kr4Pi;				// Kr * 4 * Pi;
	float	Km4Pi;				// Km * 4 * Pi;
	float2	atmPad; */
};

cbuffer world	:	register( b1 )
{
	matrix worldViewProj;//	:	packoffset(c0);
	//float3 cameraPos		:	packoffset(c4.x);
	//float  worldpad			:	packoffset(c4.w);
};

vs_output SkyFromAtmosphere_vs( float3 pos : POSITION )
{
	float3 lightPos = float3(0.2f, 0.0f, 0.8f);
	lightPos = normalize(lightPos);
	float Pi = 3.14f;
	float Kr = 0.0025f;
	float Kr4Pi = Kr * 4.0f * Pi;
	float Km = 0.0010f;
	float Km4Pi = Km * 4.0f * Pi;
	float eSun = 20.0f;
	float g = -0.990f;
	float fExposure = 2.0f;
	float KrESun = Kr * eSun;
	float KmESun = Km * eSun;

	float innerRadius = 6000000;
	float outerRadius = 6100000;
	float scale = 1 / (outerRadius - innerRadius);
	float scaleDepth = 0.2f;
	float scaleOverScaleDepth = scale / scaleDepth;

	float3 wavelength = float3(0.650f, 0.570f, 0.475f);
	float3 invWavelength = 1 / pow(wavelength, 4);

	float cameraHeight = 100.0f;
	float3 cameraPos = float3(0.0, 100.0f, 0.0f) + float3(0, innerRadius, 0);
	//ray from camera to the vertex
	pos = pos * 6100000;
	float3	rayToVertex = pos - cameraPos;
	float	fFar  = length(rayToVertex);
	rayToVertex /= fFar;

	//calc ray's starting position and scattering offset
	float3 start = cameraPos;
	float fHeight = length(start);
	float depth = exp(scaleOverScaleDepth * (innerRadius - cameraHeight));
	float startAngle = dot(rayToVertex, start) / fHeight;
	float startOffset = depth * scaleFunc(startAngle);

	//init loop variables
	float sampleLength = fFar / fSamples;
	float scaledLength = sampleLength * scale;
	float3 sampleRay = rayToVertex * sampleLength;
	float3 samplePoint = start + sampleRay * 0.5;

	//sample
	float3 frontColor = float3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < nSamples; i++)
	{
		float height = length(samplePoint);
		float depth = exp(scaleOverScaleDepth * (innerRadius - height));
		float lightAngle = dot(lightPos, samplePoint) / fHeight;
		float cameraAngle = dot(rayToVertex, samplePoint) / fHeight;
		float scatter = startOffset + depth * (scaleFunc(lightAngle) - scaleFunc(cameraAngle));
		float3 atten = exp(-scatter * (invWavelength * Kr4Pi + Km4Pi));

		frontColor += atten * (depth * scaledLength);
		samplePoint += sampleRay;
	}

	//scale Rayleigph and Mie colors and return
	vs_output output;
	output.pos = mul(float4(pos, 1.0f), worldViewProj);
	output.rColor.xyz = frontColor * (invWavelength * KrESun); output.rColor.w = 1.0f;
	output.mColor.xyz = frontColor * KmESun; output.mColor.w = 1.0f;
	output.dirToCam = cameraPos - pos;

	return output;
}