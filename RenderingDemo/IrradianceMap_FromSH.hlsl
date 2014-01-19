RWTexture2D<float4> irradiance_posx	:	register( u0 );
RWTexture2D<float4> irradiance_negx	:	register( u1 );
RWTexture2D<float4> irradiance_posy	:	register( u2 );
RWTexture2D<float4> irradiance_negy	:	register( u3 );
RWTexture2D<float4> irradiance_posz	:	register( u4 );
RWTexture2D<float4> irradiance_negz	:	register( u5 );

RWStructuredBuffer<float3> SH_Coeff;

#define cube_size 512

#define Pi 3.14159265359
#define Pi_over_4 (Pi / 4)
#define dW ( 4*Pi / ( cube_size*cube_size*6) )

#define threads_in_block 32

groupshared float partial_sum[threads_in_block][threads_in_block];

[numthreads(threads_in_block, threads_in_block, 1)]
void irradiance_map_SH(	uint3 groupId			: SV_GroupID,
								uint3 groupThreadId		: SV_GroupThreadID,
								uint  groupIndex		: SV_GroupIndex,
								uint3 dispatchThreadId	: SV_DispatchThreadID  )
{
}