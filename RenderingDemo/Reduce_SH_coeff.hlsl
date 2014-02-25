#define num_coeff 9
#define cube_size 512
#define threads_in_block 32
#define blocks ( (cube_size / threads_in_block) * (cube_size / threads_in_block) )

StructuredBuffer<float3> SH_Coeff : register( s0 );
RWStructuredBuffer<float3> Reduced_SH_Coeff : register( u0 );

[numthreads(1, 1, numcoeff)]
void Reduce_SH_coeff( uint3 groupThreadId : SV_GroupThreadID )
{
	uint index = groupThreadId.z;

	Reduced_SH_Coeff[index] = 0;
	for (uint i = 0; i < blocks; i++)
	{
		Reduced_SH_Coeff[index] += SH_Coeff[i*num_coeff + index];
	}
}