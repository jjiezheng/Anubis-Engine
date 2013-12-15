struct vs_output
{
	float4 pos		:	SV_POSITION;
	float4 rColor	:	COLOR_R; //Rayleigh color
	float4 mColor	:	COLOR_M; //Mie color
	float3 dirToCam	:	TEXCOORD;
};

//number of samples
static const int nSamples = 3;
static const float fSamples = 3.0f;

//altitude at which the average atmospheric density is found
static const float fScaleDepth = 0.25;
static const float fInvScaleDepth = 1.0 / fScaleDepth;

// scale function
float scaleFunc(float fCos)
{
	float x = 1.0 - fCos;
	return fScaleDepth * exp(-0.00287 + x*(0.459 + x*(3.83 + x*(-6.80 + x*5.25))));
}

// Calculates the Mie phase function
float calcMiePhase(float fCos, float g, float g2)
{
	return 1.5 * ((1.0 - g2) / (2.0 + g2)) * (1.0 + fCos*fCos) / pow(1.0 + g2 - 2.0*g*fCos, 1.5);
}

// Calculates the Rayleigh phase function
float calcRayleighPhase(float fCos)
{
	//return 1.0;
	return 0.75 + 0.75*fCos*fCos;
}