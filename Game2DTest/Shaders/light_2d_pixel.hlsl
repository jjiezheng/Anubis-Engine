Texture2D	positionTexture	:	register( t0 );
Texture2D	normalTexture	:	register( t1 );
Texture2D	diffuseTexture	:	register( t2 );
Texture2D	specularTexture	:	register( t3 );

SamplerState bilinearSampler : register ( s0 )
{
    Filter = min_mag_mip_linear;
    AddressU = MIRROR;
    AddressV = MIRROR;	
};

struct ps_input
{	float4 pos : SV_POSITION;
	float2 texCoord : TEXCOORD;};

cbuffer CameraBuffer	:	register( b0 )
{
	float4 cameraPos;
};

//Containes variables with general light properties
cbuffer LightBuffer	:	register( b1 )
{
	//Light Color
	float4 lightColor;

	//Light Position
	float4 lightPos;

	//Light Direction
	float4 lightDir;

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
	position	=	positionTexture.Load(float3(screenPos, 0)).xyz;
	//position = positionTexture.Sample(bilinearSampler, screenPos);
	//normal = normalTexture.Sample(bilinearSampler, screenPos);
	normal		=	normalTexture.Load(float3(screenPos, 0)).xyz;
	normal		=	normalize(normal);
	//depth = depthTexture.Sample(bilinearSampler, screenPos);
	//diffuse = diffuseTexture.Sample(bilinearSampler, screenPos);
	depth = 1.0f;
	//depth		=	depthTexture.Load(float3(screenPos, 0)).x;
	diffuse		=	diffuseTexture.Load(float3(screenPos, 0)).xyz;

	float4 spec	=	specularTexture.Load(float3(screenPos, 0));
	//float4 spec = specularTexture.Sample(bilinearSampler, screenPos);
	specular	=	spec.xyz;
	//specPower	=	spec.w;

	//convert smoothness to specular power (2-2048)
	specPower = pow(2, (10 * spec.w + 1));
	smoothness = spec.w;
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
	float3 finalDiffuse = n_dot_l /* (1.0f - fresnel_term) */* attenColor * diffuse;

	return (finalDiffuse + finalSpecular);
	//return finalDiffuse;
	//return finalSpecular;
}

float4 PS ( ps_input input ) : SV_Target
{
	float2 screenPos = input.texCoord * float2(1280, 720);
	float3	position = (float3)0.0f;
	float3	normal = (float3)0.0f;
	float	depth = 0.0f;
	float3	diffuse = (float3)0.0f;
	float3  ambient = (float3)0.0f;
	float3	specular = (float3)0.0f;
	float	specPower = 0.0f;
	float smoothness = 0.0f;

	int3 sampleCoord = int3(input.pos.xy, 0);

	//return float4(input.texCoord.y, input.texCoord.y, input.texCoord.y, 1.0f);
	
	position	=	positionTexture.Load(sampleCoord).xyz;
	//position = positionTexture.Sample(bilinearSampler, screenPos);
	//normal = normalTexture.Sample(bilinearSampler, screenPos);
	normal		=	normalTexture.Load(sampleCoord).xyz;
	normal		=	normalize(normal);
	//depth = depthTexture.Sample(bilinearSampler, screenPos);
	//diffuse = diffuseTexture.Sample(bilinearSampler, screenPos);
	//depth		=	depthTexture.Load(sampleCoord).x;
	diffuse		=	diffuseTexture.Load(sampleCoord).xyz;

	float4 spec	=	specularTexture.Load(sampleCoord);
	//float4 spec = specularTexture.Sample(bilinearSampler, screenPos);
	specular	=	spec.xyz;
	//specPower	=	spec.w;

	//convert smoothness to specular power (2-2048)
	specPower = pow(2, (10 * spec.w + 1));
	smoothness = spec.w;

	//return float4(smoothness, smoothness, smoothness, 1.0f);
	//normal = float3(0.0f, 0.0f, -1.0f);
	float3 color = float3(0.0f, 0.0f, 0.0f);
	//GetGBufferAtributes(screenPos.xy, 
	//					position, normal,
	//					depth, diffuse,
	//					specular, specPower, smoothness);
		//color = CalcLightning(position, normal, depth, diffuse, specular, specPower);

	color += PhysicallyBasedLightning(position, normal, depth, diffuse, specular, specPower, smoothness);

	return float4(color, 1.0f);
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}