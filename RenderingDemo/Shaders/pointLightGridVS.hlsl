struct vs_input
{
	float3 position : POSITION;
	uint instanceID : SV_InstanceID;
};

struct vs_output
{
	float4 position : POSITION;
	uint instanceID : INSTANCEID;
};

cbuffer MatrixBuffer : register( cb0 )
{
	matrix World;
};

vs_output point_light_gridVS( vs_input input)
{
	vs_output output;

	output.position = mul(float4(input.position, 1.0f), World);
	output.instanceID = input.instanceID;

	return output;
}