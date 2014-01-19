/*************************************************
==			Resource Declarations				==
*************************************************/

/*
	== Constant Buffers
*/

cbuffer MatrixBuffer
{
	float4x4 World;	//World Transform
	float4x4 WorldView; //World * View transform
	float4x4 WorldViewProjection; // World * View * Projection
};

/*************************************************
==			Shader Input Structures				==
*************************************************/

/*
	== Vertex Shader Input
*/
struct vs_input
{
	float3	pos			:	POSITION;
	float2	texCoords	:	TEXCOORDS;
	float3	normal		:	NORMAL;
	float3	tangent		:	TANGENT;
	float3	binormal	:	BINORMAL;
};

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

/*************************************************
==			Vertex Shader						==
*************************************************/
ps_input	VS( vs_input input )
{
	ps_input output;

	//calculate clip-space position
	output.posCS = mul(float4(input.pos, 1.0f), WorldViewProjection);

	//pass texture coordinates
	output.texCoords = input.texCoords;

	//calculte normal and position in world space
	float3 normalWS = normalize(mul(input.normal, (float3x3)World));

	//calculate tangent vectors in the world space
	float3 tangentWS = normalize(mul(input.tangent, (float3x3)World));

	//assign tangent space
	output.normalWS = normalWS;
	output.tangentWS = tangentWS;
	output.binormalWS = normalize(cross(normalWS, tangentWS));

	//calculate position
	output.posWS  = mul(float4(input.pos, 1.0f), World);
	output.posVS  = mul(float4(input.pos, 1.0f), WorldView);

	return output;
};