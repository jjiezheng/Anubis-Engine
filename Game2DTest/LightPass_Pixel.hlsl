Texture2D	positionTexture	:	register( t0 );
Texture2D	normalTexture	:	register( t1 );
Texture2D	depthTexture	:	register( t2 );
Texture2D	diffuseTexture	:	register( t3 );
Texture2D	specularTexture	:	register( t4 );
Texture2D	geometryNormalTexture	:	register( t5 );
Texture2D	ambientOcclusionMap	:	register( t6 );
Texture2D	shadowMap		:	register( t7 );
Texture2D	depthMap		:	register( t8 );
TextureCube shadowCubeMap	:	register( t9 );

SamplerState bilinearSampler : register ( s0 )
{
    Filter = min_mag_mip_linear;
    AddressU = MIRROR;
    AddressV = MIRROR;	
};

SamplerState pointSampler : register( s1 );
//{
//    Filter = min_mag_mip_point;
//    AddressU = MIRROR;
//    AddressV = MIRROR;	
//};

SamplerComparisonState cmpSampler : register( s2 )
{
   // sampler state
   Filter = COMPARISON_MIN_MAG_MIP_LINEAR;
   AddressU = MIRROR;
   AddressV = MIRROR;

   // sampler comparison state
   ComparisonFunc = LESS_EQUAL;   
};

cbuffer CameraBuffer	:	register( b0 )
{
	//float4x4 inverseViewProj;
	float4 cameraPos;
};

//Containes variables with general light properties
cbuffer LightBuffer	:	register( b1 )
{
	//Light Color
	float4 lightColor;
	//float  lightColorPad;

	//Light Position
	float4 lightPos;
	//float  lightPosPad;

	//Light Direction
	float4 lightDir;
	//float  lightType;

	//light relatred view and projection transform(shadow mapping)
	matrix lightView;
	matrix lightViewProj;
};

cbuffer PointLightBuffer	:	register( b2 )
{
	//Light Range
	float4 lightRange;
};

cbuffer SpotLightBuffer	:	register( b3 )
{
	//Conus Angles
	float4 spotLightAngles;
	//float2 slAnglesPad;
};

//////////////////////////////////////////
//Help methods
//////////////////////////////////////////
bool POINT_LIGHT()
{
	if (lightDir.w == 1.0f)
		return true;

	return false;
}

bool SPOT_LIGHT()
{
	if (lightDir.w == 2.0f)
		return true;

	return false;
}

bool DIRECTIONAL_LIGHT()
{
	if (lightDir.w == 3.0f)
		return true;

	return false;
}

void GetGBufferAtributes( in float2 screenPos, out float3 position, out float3 normal, out float depth,
						 out float3 diffuse, out float3 specular, out float specPower, out float smoothness)
{
	int3 sampleCoords = int3( screenPos, 0 );

	position	=	positionTexture.Load(sampleCoords).xyz;
	normal		=	normalTexture.Load(sampleCoords).xyz;
	normal		=	normalize(normal);
	depth		=	depthTexture.Load(sampleCoords).x;
	diffuse		=	diffuseTexture.Load(sampleCoords).xyz;

	float4 spec	=	specularTexture.Load(sampleCoords);
	specular	=	spec.xyz;
	//specPower	=	spec.w;

	//convert smoothness to specular power (2-2048)
	specPower = pow(2, (10 * spec.w + 1));
	smoothness = spec.w;
	
	//Gamma correction
	//diffuse = pow(diffuse, 2.2);
	//specular = pow(specular, 2.2);
}

float3 CalcLightning( in float3 position,
					  in float3	normal,
					  in float	depth,
					  in float3	diffuse,
					  in float3 specular,
					  in float	specPower )
{
	//firstly handle diffuse term
	float3 L = (float3)0.0f;
	float atten = 1.0f;

	if( POINT_LIGHT() || SPOT_LIGHT() )
	{
		//calc inverse light direction
		L = lightPos - position;

		//calculate attenuation
		float distance = length(L);
		//normalize
		L /= distance;
		//atten = max(0.0f, 1.0f - (distance / 30.0f));
		atten = max(0.0f, 1.0f - (distance / lightRange.x));
		//atten = 1.0f;
	}
	else if( DIRECTIONAL_LIGHT() )
	{
		L = -lightDir;
	}
	

	if( SPOT_LIGHT() )
	{
		//add attenuation for a spot light
		float3 L2 = lightDir;
		L = normalize(L);
		L2 = normalize(L2);
		float angle = acos(dot(-L, L2));
		atten *= ( (angle - spotLightAngles.y) /(spotLightAngles.x - spotLightAngles.y) );
	}
	
	float normalDotL = saturate(dot(normal, L));
	float3 finalDiffuse = diffuse * lightColor * normalDotL;

	//now handle specular term
	float3 V = cameraPos - position; //view vector
	V = normalize(V);
	float3 H = normalize( L + V );
	float3 finalSpecular = pow( saturate( dot(normal, H) ), specPower) * lightColor * specular * normalDotL;

	return ( finalDiffuse + finalSpecular) * atten;
	//return diffuse;
}

float BlinnPhongSpecular( in float specPower, in float n_dot_h)
{
	float normalization = (specPower + 2.0f) / (8.0f);
	float blinn_phong = pow(n_dot_h, specPower);
	return normalization * blinn_phong;
}

float TrowbridgeReitzSpecular( in float smoothness, in float n_dot_h)
{
	const float Pi = 3.14159265f;

	float roughness = 1.0f - smoothness;

	return (roughness * roughness) / (Pi * pow(( n_dot_h * n_dot_h * (roughness * roughness - 1) + 1), 2.0f));
}

float ShlickVisibility( in float specPower, in float n_dot_l, in float n_dot_v )
{
	const float Pi = 3.14159265f;
	float PI_OVER_TWO = Pi / 2.0f;
	float PI_OVER_FOUR = Pi / 4.0f;

	float alpha = 1 / sqrt(PI_OVER_FOUR * specPower + PI_OVER_TWO);
	return 0.5f / ( (n_dot_l * (1.0f - alpha) + alpha) * (n_dot_v * (1.0f - alpha) + alpha) );
}

float3 PhysicallyBasedLightning( in float3 position,
								 in float3 normal,
								 in float  depth,
								 in float3 diffuse,
								 in float3 specular,
								 in float  specPower,
								 in float smoothness)
{
	const float Pi = 3.14159265f;

	//get inverse light direction
	float3 l = lightPos - position;

	//distance to the light source
	float distance = length(l);

	//normalize
	l /= distance;

	//calculate view vector
	float3 v = cameraPos - position;
	v = normalize(v);

	//half vector
	float3 h = normalize(l + v);
		 
	float n_dot_l = saturate(dot(normal, l));
	float n_dot_v = saturate(dot(normal, v));
	float h_dot_l = saturate(dot(h, l));
	float n_dot_h = saturate(dot(normal, h));

	//*****************************************
	//Light attenuation
	//*****************************************
	float intencity = distance / lightRange.x;
	//float atten = max(0.0f, 1.0f - (distance / lightRange.x));
	float atten = pow(saturate(1.0f - pow(intencity, 4.0f)), 2.0f) / (pow(intencity, 2.0f) + 1.0f);
	float3 attenColor = lightColor * atten;

	//*****************************************
	//Specular Term
	//
	//specular = (Pi / 4.0f) * specular_term * cosine_term * fresnel_term * visibility_term * lightColor
	//*****************************************

	//calculate specular term
	//float specular_term = BlinnPhongSpecular(specPower, n_dot_h);
	float specular_term = TrowbridgeReitzSpecular(smoothness, n_dot_h);

	//cosine term
	float cosine_term = n_dot_l;

	//fresnel reflection (Shlick's approximation)
	float base = 1.0f - h_dot_l;
	float exp = pow(base, 5.0f);
	float fresnel_term = specular + (1.0f - specular) * exp;

	//visibility term (Smith shadowing function)
	//float visibility_term = 1.0f; 
	float visibility_term = ShlickVisibility( specPower, n_dot_l, n_dot_v );
	//float visibility_term = 1.0f / (h_dot_l * h_dot_l);

	//!!! - finally calculate specular reflectance
	float3 finalSpecular = specular_term * cosine_term * fresnel_term * visibility_term * attenColor;

	//*****************************************
	//Diffuse Term - Energy Conserving!!!!!
	//*****************************************
	float3 finalDiffuse = n_dot_l * (1.0f - fresnel_term) * attenColor;

	return (finalDiffuse + finalSpecular);
	//return finalDiffuse;
	//return finalSpecular;
}

float CalcShadowing(in float3 worldPos)
{
	//convert coordinates to light's post-projection
	float4 shadowMapPos = mul(float4(worldPos, 1.0f), lightViewProj);
	shadowMapPos /= shadowMapPos.w;

	//get atual depth from light
	float lightDepth = shadowMapPos.z;
	lightDepth -= 0.001; //bias(to avoid shadow acne)

	float2 samplingCoords = float2( (shadowMapPos.x + 1.0f) * 0.5f, (shadowMapPos.y - 1.0f) * (-0.5) );
	float shadowMapDepth = shadowMap.Sample(pointSampler, samplingCoords);

	if (lightDepth > shadowMapDepth) 
	{
		return 0.0f; //shadowed
	}
	else
	{
		return 1.0f;
	}
}

float CalcShadowingCube(in float3 worldPos, in float3 dir)
{
	//convert coordinates to light's post-projection
	//float4 shadowMapPos = mul(float4(worldPos, 1.0f), lightViewProj);
	//shadowMapPos /= shadowMapPos.w;

	//get atual depth from light
	//float lightDepth = shadowMapPos.z;
	//lightDepth -= 0.001; //bias(to avoid shadow acne)

	//float2 samplingCoords = float2( (shadowMapPos.x + 1.0f) * 0.5f, (shadowMapPos.y - 1.0f) * (-0.5) );
	float lightDepth = length(dir) / 79.5f  - 0.001;
	dir = normalize(dir);
	float shadowMapDepth = shadowCubeMap.Sample(pointSampler, dir);

	//return lightDepth;
	//return shadowMapDepth;

	if (lightDepth > shadowMapDepth) 
	{
		return 0.0f; //shadowed
	}
	else
	{
		return 1.0f;
	}
}

float CalcShadowingPCF16(in float3 worldPos)
{
	float2 recipShadowMapSize = float2(1.0f / 1280.0f, 1.0f / 720.0f);

	//Calculate coordinates in light space
	float4 shadowMapPos = mul(float4(worldPos, 1.0f), lightViewProj);
	shadowMapPos /= shadowMapPos.w;

	//if it is not visible to the light then just return ambient lighting
	if (shadowMapPos.x <= -1.0f || shadowMapPos.x >= 1.0f ||
		shadowMapPos.y <= -1.0f || shadowMapPos.y >= 1.0f ||
		shadowMapPos.z <= 0.0f  || shadowMapPos.z >= 1.0f )
	{
		return 0.0f;
	}

	float lightDepth = shadowMapPos.z;
	float2 samplingCoords = float2( (shadowMapPos.x + 1.0f) * 0.5f, (shadowMapPos.y - 1.0f) * (-0.5) );
	lightDepth -= 0.01;

	//temporary variable to compute PCF
	float Sum = 0;
	float x, y;
	for (y = -1.5; y <= 1.5; y += 1.0)
	{
		for (x = -1.5; x <= 1.5; x += 1.0)
		{
			Sum += shadowMap.SampleCmpLevelZero( cmpSampler, samplingCoords + recipShadowMapSize * float2(x, y), lightDepth);
		}
	}

	Sum /= 16.0;
	return Sum;
}

float CalcShadowingVariance(in float3 worldPos)
{
	const float lightVSMEpsilon = 0.0000000001;
	float2 recipShadowMapSize = float2(1.0f / 1280.0f, 1.0f / 720.0f);

	//Calculate coordinates in light space
	float4 shadowMapPos = mul(float4(worldPos, 1.0f), lightViewProj);
	shadowMapPos /= shadowMapPos.w;

	//if it is not visible to the light then just return ambient lighting
	if (shadowMapPos.x <= -1.0f || shadowMapPos.x >= 1.0f ||
		shadowMapPos.y <= -1.0f || shadowMapPos.y >= 1.0f ||
		shadowMapPos.z <= 0.0f  || shadowMapPos.z >= 1.0f )
	{
		return 1.0f;
	}

	float lightDepth = shadowMapPos.z;
	float2 samplingCoords = float2( (shadowMapPos.x + 1.0f) * 0.5f, (shadowMapPos.y - 1.0f) * (-0.5) );
	lightDepth -= 0.0001;

	//Retrieve data
	float4 moments = shadowMap.Sample(pointSampler, samplingCoords);

	//Only lit if depth <= estimated value in shadow map
	/*float litFactor = (lightDepth <= moments.x);
	//if (litFactor == 0) return 0.0f;
	//float litFactor;

	//Calculate moments
	float E_x = moments.x * moments.x;
	float E_x2 = moments.y;

	float variance = min(max(E_x2 - E_x, 0.0) , 1.0);
	float m_d = (moments.x - lightDepth);
	float p = variance / (variance + m_d * m_d);

	litFactor = max(p, 1.0f);
	return litFactor; */
	float variance = moments.y - moments.x * moments.x;
	float diff = lightDepth - moments.x;
	if (diff >= 0)
	{
		return variance / (variance + diff * diff);
	}
	else
	{
		return 1.0f;
	}
}

float CalcShadowingVariance(in float3 worldPos, in float3 dir)
{
	const float lightVSMEpsilon = 0.0000000001;
	float2 recipShadowMapSize = float2(1.0f / 1280.0f, 1.0f / 720.0f);

	//Calculate coordinates in light space
	float4 shadowMapPos = mul(float4(worldPos, 1.0f), lightViewProj);
	shadowMapPos /= shadowMapPos.w;

	//if it is not visible to the light then just return ambient lighting
	if (shadowMapPos.x <= -1.0f || shadowMapPos.x >= 1.0f ||
		shadowMapPos.y <= -1.0f || shadowMapPos.y >= 1.0f ||
		shadowMapPos.z <= 0.0f  || shadowMapPos.z >= 1.0f )
	{
		return 1.0f;
	}

	//float lightDepth = shadowMapPos.z;
	float lightDepth = length(dir) / 79.5f;
	dir = normalize(dir);
	float2 samplingCoords = float2( (shadowMapPos.x + 1.0f) * 0.5f, (shadowMapPos.y - 1.0f) * (-0.5) );
	//lightDepth -= 0.0001;

	//Retrieve data
	//float4 moments = shadowMap.Sample(pointSampler, samplingCoords);
	float4 moments = shadowCubeMap.Sample(bilinearSampler, dir);
	//return moments.x;

	//Only lit if depth <= estimated value in shadow map
	/*float litFactor = (lightDepth <= moments.x);
	//if (litFactor == 0) return 0.0f;
	//float litFactor;

	//Calculate moments
	float E_x = moments.x * moments.x;
	float E_x2 = moments.y;

	float variance = min(max(E_x2 - E_x, 0.0) , 1.0);
	float m_d = (moments.x - lightDepth);
	float p = variance / (variance + m_d * m_d);

	litFactor = max(p, 1.0f);
	return litFactor; */
	//float variance = max(0.00000001f, moments.y - moments.x * moments.x);
	float variance = moments.y - moments.x * moments.x;
	float diff = lightDepth - moments.x;
	if (diff >= 0)
	{
		return variance / (variance + diff * diff);
	}
	else
	{
		return 1.0f;
	}
}

float3 Uncharted2Tonemap(float3 color)
{
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;
	
	return ((color*(A*color + C*B)+D*E)/(color*(A*color+B)+D*F))-E/F;
}

float4 PS ( float4 screenPos : SV_POSITION ) : SV_Target
{
	//return float4(1.0f, 1.0f, 0.0f, 1.0f);
	float3	position;
	float3	normal;
	float	depth;
	float3	diffuse;
	float3  ambient;
	float3	specular;
	float	specPower;
	float smoothness;

	float3 color = float3(0.0f, 0.0f, 0.0f);
	GetGBufferAtributes(screenPos.xy, 
						position, normal,
						depth, diffuse,
						specular, specPower, smoothness);
						
	float d = depthMap.Load(int3(screenPos.xy, 0.0f));
	if (d == 1.0f) //sky
	{
		return float4(diffuse,1.0f);
	}
	else
	{
	
	float3 dir = position - float3(lightPos.x, lightPos.y, lightPos.z);
	//dir = normalize(dir);
	
	//float cubeDepth = shadowCubeMap.Sample(bilinearSampler, dir).x;
	//float shadowAtten = CalcShadowingCube(position, dir);
	float shadowAtten = CalcShadowingVariance(position, dir);
	//return float4(shadowAtten, shadowAtten, shadowAtten, 1.0f);
	
	ambient = diffuse;
	color += ambient * float3(0.15f, 0.15f, 0.15f);

	const float lightVSMEpsilon = 0.0001;
	float2 recipShadowMapSize = float2(1.0f / 1280.0f, 1.0f / 720.0f);

	//Calculate coordinates in light space
	float4 shadowMapPos = mul(float4(position, 1.0f), lightViewProj);
	shadowMapPos /= shadowMapPos.w;

	float lightDepth = shadowMapPos.z;
	float2 samplingCoords = float2( (shadowMapPos.x + 1.0f) * 0.5f, (shadowMapPos.y - 1.0f) * (-0.5) );
	lightDepth -= 0.01;

	float depthSM = shadowMap.Sample(pointSampler, samplingCoords).x;
	//return float4(depthSM, depthSM, depthSM, 1.0f);

	//float shadowAtten = CalcShadowingPCF16(position);
	//float shadowAtten = CalcShadowingVariance(position);
	//if (shadowAtten > 0.0f)
	//{
	//	color += shadowAtten * CalcLightning(position, normal, depth,
	//						diffuse, specular, specPower);
	//}
	//color = CalcLightning(position, normal, depth, diffuse, specular, specPower);
	color += shadowAtten*PhysicallyBasedLightning(position, normal, depth, diffuse, specular, specPower, smoothness);
	float3 hdrcolor = Uncharted2Tonemap(color) / Uncharted2Tonemap(float3(1.0f, 1.0f, 1.0f));
	//return float4(color, 1.0f)
	//hdrcolor = pow(color, 1.0 / 2.2);
	//hdrcolor = Uncharted2Tonemap(color) / Uncharted2Tonemap(float3(1.0f, 1.0f, 1.0f));
	return float4(hdrcolor, 1.0f);
	
	float ao = ambientOcclusionMap.Load(int3(screenPos.xy, 0.0f)).r;

	float shadow = shadowMap.Load(int3(screenPos.xy, 0.0f)).x;
	
	//return float4(lightViewPos.x, lightViewPos.y, lightViewPos.z, 1.0f);
	//return float4(samplingCoords.x, samplingCoords.x, samplingCoords.x, 1.0f);
	//return float4(shadow, shadow, shadow, 1.0f);
	//return float4(earlierDepth, earlierDepth, earlierDepth, 1.0f);
	//return float4(lightDepth, lightDepth, lightDepth, 1.0f);
	//return float4(ao, ao, ao, 1.0f);
	//return float4(ao, 1.0f);
	//return float4(depth, depth, depth, 1.0f);
	//return float4(Sum, Sum, Sum, 1.0f);
	}
}