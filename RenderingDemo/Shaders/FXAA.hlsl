struct vertex_output
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
};

vertex_output FXAA_VS( uint id : SV_VertexID )
{
	vertex_output output;

	output.tex = float2((id << 1) & 2, id & 2);
	output.pos = float4(output.tex * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);

	return output;
}

#define EDGE_THRESHOLD      (1.0/8.0)
#define EDGE_THRESHOLD_MIN  (1.0/24.0)
#define SEARCH_STEPS        16
#define SEARCH_ACCELERATION 1
#define SEARCH_THRESHOLD    (1.0/4.0)
#define SUBPIX              1
#define SUBPIX_FASTER       0
#define SUBPIX_CAP          (3.0/4.0)
#define SUBPIX_TRIM         (1.0/4.0)

#define SUBPIX_TRIM_SCALE (1.0/(1.0 - SUBPIX_TRIM))

Texture2D inputTexture;
SamplerState anisotropicSampler;

float CalcLuminance(in float3 rgb)
{
	return rgb.y * (0.587/0.299) + rgb.x; 
}

float3 Lerp3(float3 a, float3 b, float amountOfA) {
    return (float3(-amountOfA, -amountOfA, -amountOfA) * b) + 
        ((a * float3(amountOfA, amountOfA, amountOfA)) + b); } 

float3 FXAA(in float3 pos, in float2 tex)
{
	//get reciprocial texture width and height
	float2 recipFrame; 
	inputTexture.GetDimensions(recipFrame.x, recipFrame.y);
	recipFrame.x = 1.0f / recipFrame.x;
	recipFrame.y = 1.0f / recipFrame.y;

	// ************************************************* //
	//			Local Contrast 
	// - Early exit if the difference in local max and min
	// luminance is lower than a threshold proportional to
	// the maximum local luma
	float3 rgbN = inputTexture.SampleLevel(anisotropicSampler, tex, 0.0f, float2(0, -1)).xyz;
	float3 rgbW = inputTexture.SampleLevel(anisotropicSampler, tex, 0.0f, float2(-1, 0)).xyz;
	float3 rgbM = inputTexture.SampleLevel(anisotropicSampler, tex, 0.0f, float2(0, 0)).xyz;
	float3 rgbE = inputTexture.SampleLevel(anisotropicSampler, tex, 0.0f, float2(1, 0)).xyz;
	float3 rgbS = inputTexture.SampleLevel(anisotropicSampler, tex, 0.0f, float2(0, 1)).xyz;

	//calculate luminance
	float lumaN = CalcLuminance(rgbN);
	float lumaW = CalcLuminance(rgbW);
	float lumaM = CalcLuminance(rgbM);
	float lumaE = CalcLuminance(rgbE);
	float lumaS = CalcLuminance(rgbS);

	float rangeMin = min(lumaM, min(min(lumaN, lumaW), min(lumaS, lumaE)));
	float rangeMax = max(lumaM, max(max(lumaN, lumaW), min(lumaS, lumaE)));
	float range = rangeMax - rangeMin;

	if (range < max(EDGE_THRESHOLD_MIN, rangeMax * EDGE_THRESHOLD))
	{
		return rgbM;
	}

	#if SUBPIX > 0
		#if SUBPIX_FASTER
			float3 rgbL = (rgbN + rgbW + rgbE + rgbS + rgbM) * float3(1.0 / 5.0);
		#else
			float3 rgbL = rgbN + rgbW + rgbE + rgbS + rgbM;
		#endif
	#endif

	// ************************************************ //
	//				Compute Lowpass 
	//
	//			(N+W+E+S) / 4
	#if SUBPIX != 0
		float lumaL = (lumaN + lumaW + lumaE + lumaS) * 0.25;
		float rangeL = abs(lumaL - lumaM);
	#endif
	#if SUBPIX == 1
		float blendL = max(0.0, (rangeL / range) - SUBPIX_TRIM) * SUBPIX_TRIM_SCALE;
		blendL = min(SUBPIX_CAP, blendL);
	#endif
	#if SUBPIX == 2
		float blendL = rangeL / range;
	#endif

	// ************************************************ //
	//		Choose vertical or horizontal search
	//
	//			NW	N  NE
	//			W   M   E
	//			SW  S  SE
	// ************************************************ //
	float3 rgbNW = inputTexture.SampleLevel(anisotropicSampler, tex, 0.0f, int2(-1,-1)).xyz;
	float3 rgbNE = inputTexture.SampleLevel(anisotropicSampler, tex, 0.0f, int2(1, -1)).xyz;
	float3 rgbSW = inputTexture.SampleLevel(anisotropicSampler, tex, 0.0f, int2(-1, 1)).xyz;
	float3 rgbSE = inputTexture.SampleLevel(anisotropicSampler, tex, 0.0f, int2(1, 1)).xyz;
	#if (SUBPIX_FASTER == 0) && (SUBPIX > 0)
		rgbL += (rgbNW + rgbNE + rgbSW + rgbSE);
		rgbL *= float3(1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0);
	#endif

	float lumaNW = CalcLuminance(rgbNW);
	float lumaNE = CalcLuminance(rgbNE);
	float lumaSW = CalcLuminance(rgbSW);
	float lumaSE = CalcLuminance(rgbSE);
	float edgeVer = 
        abs((0.25 * lumaNW) + (-0.5 * lumaN) + (0.25 * lumaNE)) +
        abs((0.50 * lumaW ) + (-1.0 * lumaM) + (0.50 * lumaE )) +
        abs((0.25 * lumaSW) + (-0.5 * lumaS) + (0.25 * lumaSE));
    float edgeHor = 
        abs((0.25 * lumaNW) + (-0.5 * lumaW) + (0.25 * lumaSW)) +
        abs((0.50 * lumaN ) + (-1.0 * lumaM) + (0.50 * lumaS )) +
        abs((0.25 * lumaNE) + (-0.5 * lumaE) + (0.25 * lumaSE));
	bool horSpan = edgeHor >= edgeVer;

	float lengthSign = horSpan ? -recipFrame.y : -recipFrame.x;
	if (!horSpan) lumaN = lumaW;
	if (!horSpan) lumaS = lumaE;
	float gradientN = abs(lumaN - lumaM);
	float gradientS = abs(lumaS - lumaM);
	lumaN = (lumaN + lumaM) * 0.5;
	lumaS = (lumaS + lumaM) * 0.5;

	// ************************************************ //
	//	Choose side of the pixel where gradient is highest
	//This chooses a pixel pair. 
	//For "horzSpan == true" this will be a vertical pair,
 
    //[N]     N
    //[M] or [M]
    //S     [S]

	bool pairN = gradientN >= gradientS;
	if (!pairN) lumaN = lumaS;
	if (!pairN) gradientN = gradientS;
	if (!pairN) lengthSign *= -1.0f;
	float2 posN;
	posN.x = pos.x + (horSpan ? 0.0 : lengthSign * 0.5);
	posN.y = pos.y + (horSpan ? lengthSign * 0.5 : 0.0);

	//////////////////////////////////////////////////////////
	//Search limit
	gradientN *= SEARCH_THRESHOLD;

	//////////////////////////////////////////////////////////
	//Search in both directionbs until find luma pair average is out of range
	float2 posP = posN;
	float2 offNP = horSpan ? float2(recipFrame.x, 0.0) :
								float2(0.0, recipFrame.y);
	float lumaEndN = lumaN;
	float lumaEndP = lumaN;
	bool doneN = false;
	bool doneP = false;
	#if FXAA_SEARCH_ACCELERATION == 1
        posN += offNP * FxaaFloat2(-1.0, -1.0);
        posP += offNP * FxaaFloat2( 1.0,  1.0);
    #endif
    #if FXAA_SEARCH_ACCELERATION == 2
        posN += offNP * FxaaFloat2(-1.5, -1.5);
        posP += offNP * FxaaFloat2( 1.5,  1.5);
        offNP *= FxaaFloat2(2.0, 2.0);
    #endif
    #if FXAA_SEARCH_ACCELERATION == 3
        posN += offNP * FxaaFloat2(-2.0, -2.0);
        posP += offNP * FxaaFloat2( 2.0,  2.0);
        offNP *= FxaaFloat2(3.0, 3.0);
    #endif
    #if FXAA_SEARCH_ACCELERATION == 4
        posN += offNP * FxaaFloat2(-2.5, -2.5);
        posP += offNP * FxaaFloat2( 2.5,  2.5);
        offNP *= FxaaFloat2(4.0, 4.0);
    #endif
		for (int i = 0; i < SEARCH_STEPS; i++)
		{
		#if SEARCH_ACCELERATION == 1
			if (!doneN) lumaEndN =
				CalcLuminance(inputTexture.SampleLevel(anisotropicSampler, posN.xy, 0.0).xyz);
			if (!doneP) lumaEndP =
				CalcLuminance(inputTexture.SampleLevel(anisotropicSampler, posP.xy, 0.0f).xyz);
		#else
			if (!doneN) lumeEndN =
				CalcLuminance(inputTexture.SampleGrad(anisotropicSampler, posN.xy, offNP, offNP).xyz);
			if (!doneP) lumeEndP =
				CalcLuminance(inputTexture.SampleGrad(anisotropicSampler, posP.xy, offNP, offNP).xyz);
		#endif
	    doneN = doneN || (abs(lumaEndN - lumaN) >= gradientN);
        doneP = doneP || (abs(lumaEndP - lumaN) >= gradientN);
        if(doneN && doneP) break;
        if(!doneN) posN -= offNP;
        if(!doneP) posP += offNP; }

		/*----------------------------------------------------------------------------
               HANDLE IF CENTER IS ON POSITIVE OR NEGATIVE SIDE 
		------------------------------------------------------------------------------ */

		float dstN = horSpan ? pos.x - posN.x : pos.y - posN.y;
		float dstP = horSpan ? posP.x - pos.x : posP.y - pos.y;
		bool directionN = dstN < dstP;
		lumaEndN = directionN ? lumaEndN : lumaEndP;



		if(((lumaM - lumaN) < 0.0) == ((lumaEndN - lumaN) < 0.0)) 
        lengthSign = 0.0;



		float spanLength = (dstP + dstN);
		dstN = directionN ? dstN : dstP;
		float subPixelOffset = (0.5 + (dstN * (-1.0/spanLength))) * lengthSign;

		//float3 rgbF = FxaaTexLod0(tex, FxaaFloat2(
        //pos.x + (horSpan ? 0.0 : subPixelOffset),
        //pos.y + (horSpan ? subPixelOffset : 0.0))).xyz;
		float3 rgbF = inputTexture.SampleLevel(anisotropicSampler,
			float2( pos.x + (horSpan ? 0.0 : subPixelOffset),
					pos.y + (horSpan ? subPixelOffset : 0.0)), 0.0).xyz;
		#if FXAA_SUBPIX == 0
			//return FxaaFilterReturn(rgbF); 
			return rgbF; 
		#else        
			//return FxaaFilterReturn(FxaaLerp3(rgbL, rgbF, blendL)); 
			return Lerp3(rgbL, rgbF, blendL);
		#endif
 
}

float4 FXAA_PS(vertex_output input) : SV_Target
{
	return float4(FXAA(input.pos, input.tex), 1.0f);
}