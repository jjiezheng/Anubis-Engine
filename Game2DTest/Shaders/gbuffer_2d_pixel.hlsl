/*************************************************
==			Resource Declarations				==
*************************************************/

/*
	== Constant Buffers
*/

cbuffer CameraBuffer
{
	float4 cameraPos;
};


/*
	== Textures and Sampler States
*/
Texture2D		diffuseTexture		:	register( t0 );
Texture2D		specularMap			:	register( t1 );
Texture2D		smoothnessMap		:	register( t2 );
Texture2D		normalMap			:	register( t3 );
SamplerState	anisotropicSampler	:	register( s0 );


/*************************************************
==			Shader Input Structures				==
*************************************************/

/* 
	== Pixel Shader Input
*/
struct ps_input
{
	float4 posCS		:	SV_POSITION;
	float2 texCoords	:	TEXCOORDS;
	float3 normalWS		:	NORMALWS;
	float3 tangentWS	:	TANGENTWS;
	float3 binormalWS	:	BINORMALWS;
	float4 posWS		:	POSITIONWS;
	float4 posVS		:	POSITIONVS;
};

/*
	== Pixel Shader Output
*/
struct ps_output
{
	float4 position			:	SV_Target0;
	float4 normal			:	SV_Target1;
	float4 diffuseAlbedo	:	SV_Target2;
	float4 specAlbedo		:	SV_Target3;
};

/*************************************************
==			Pixel Shader						==
*************************************************/
ps_output	PS( ps_input input )
{
	const float MAX_DISTANCE = 500.0f;

	ps_output output;

	//get diffuse color
	float3 diffuse = diffuseTexture.Sample(anisotropicSampler, input.texCoords).rgb;
	float3 specular = specularMap.Sample(anisotropicSampler, input.texCoords).rgb;
	float smoothness = smoothnessMap.Sample(anisotropicSampler, input.texCoords).r;

	//handle tangent space and normal mapping
	float3 normalWS		= normalize(input.normalWS);
	float3 tangentWS	= normalize(input.tangentWS);
	float3 binormalWS	= normalize(input.binormalWS);
	float3x3 TangentToWorldSpace = float3x3(	(float3)tangentWS,
												(float3)binormalWS,
												(float3)normalWS	);

	float3 normalTS	= normalMap.Sample(anisotropicSampler, input.texCoords).rgb;
	float3 bumpNormalWS = normalize( normalTS * 2.0f - 1.0f);
	bumpNormalWS = mul(bumpNormalWS, TangentToWorldSpace); 
	bumpNormalWS = normalize(bumpNormalWS);

	//fill the g-buffer
	output.position = input.posWS;
	output.normal			= float4(bumpNormalWS, 0.0f);
	//output.normal = half4((half2(atan2(bumpNormalWS.y, bumpNormalWS.x)/3.1415926536f, bumpNormalWS.z)+1.0)*0.5, 0,0);
	output.diffuseAlbedo	= float4(diffuse, 1.0f);
	output.specAlbedo		= float4(specular, smoothness);

	return output;
}