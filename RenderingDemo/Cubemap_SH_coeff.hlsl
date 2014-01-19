
Texture2D<float4> cubemap_posx	:	register( t0 );
Texture2D<float4> cubemap_negx	:	register( t1 );
Texture2D<float4> cubemap_posy	:	register( t2 );
Texture2D<float4> cubemap_negy	:	register( t3 );
Texture2D<float4> cubemap_posz	:	register( t4 );
Texture2D<float4> cubemap_negz	:	register( t5 );

//RWTexture2D<float4> irradiance_posx	:	register( u0 );
//RWTexture2D<float4> irradiance_negx	:	register( u1 );
//RWTexture2D<float4> irradiance_posy	:	register( u2 );
//RWTexture2D<float4> irradiance_negy	:	register( u3 );
//RWTexture2D<float4> irradiance_posz	:	register( u4 );
//RWTexture2D<float4> irradiance_negz	:	register( u5 );

#define cube_size 512

#define Pi 3.14159265359
#define Pi_over_4 (Pi / 4)
#define dW ( 4*Pi / ( cube_size*cube_size*6) )

#define threads_in_block 32
#define num_coeff 9

RWStructuredBuffer<float3> SH_Coeff : register( u0 );

groupshared float partial_sum[threads_in_block][threads_in_block];

float calc_SH_value(in uint index, in float phi, in float theta)
{
	if      (index == 0) return 0.282095;
	else if (index == 1) return 0.488603*sin(theta)*sin(phi);
	else if (index == 2) return 0.488603*cos(theta);
	else if (index == 3) return 0.488603*sin(theta)*cos(phi);
	else if (index == 4) return 1.092548*sin(theta)*sin(theta)*cos(phi)*sin(phi);
	else if (index == 5) return 1.092548*sin(theta)*sin(phi)*cos(phi);
	else if (index == 6) return 0.315392*(3*cos(theta)*cos(theta)-1);
	else if (index == 7) return 1.092548*sin(theta)*cos(theta)*cos(phi);
	else if (index == 8) return 0.546274*sin(theta)*sin(theta)*(cos(phi)*cos(phi)-sin(phi)*sin(phi));
}

[numthreads(threads_in_block, threads_in_block, 1)]
void generate_spherical_coeff(	uint3 groupId			: SV_GroupID,
								uint3 groupThreadId		: SV_GroupThreadID,
								uint  groupIndex		: SV_GroupIndex,
								uint3 dispatchThreadId	: SV_DispatchThreadID  )
{
	/* ---------------------------------------------------
			Generate spherical harmonics coefficients
	   --------------------------------------------------- */
	uint index_x = groupThreadId.x;
	uint index_y = groupThreadId.y;

	//Calculate all partial sums and wroite thrm to shared memory
	partial_sum[index_x][index_y] = (float3)0;
	uint num_blocks = cube_size / threads_in_block;
	for (uint i = 0; i < num_blocks; i += threads_in_block)
	{
		for (uint j = 0; j < num_blocks; j += threads_in_block)
		{
			//calculate pixel to sample
			uint cube_index_x = i*threads_in_block + index_x;
			uint cube_index_y = j*threads_in_block + index_y;

			//get interpolation coefficients
			float coeff_phi = cube_index_x / cube_size;
			float coeff_theta = cube_index_y / cube_size;

			//get radiance value from each face of cubemap
			float3 pos_x = cubemap_posx.Load(uint3(cube_index_x, cube_index_y, 0)).xyz;
			float3 neg_x = cubemap_posx.Load(uint3(cube_index_x, cube_index_y, 0)).xyz;
			float3 pos_y = cubemap_posx.Load(uint3(cube_index_x, cube_index_y, 0)).xyz;
			float3 neg_y = cubemap_posx.Load(uint3(cube_index_x, cube_index_y, 0)).xyz;
			float3 pos_z = cubemap_posx.Load(uint3(cube_index_x, cube_index_y, 0)).xyz;
			float3 neg_z = cubemap_posx.Load(uint3(cube_index_x, cube_index_y, 0)).xyz;

			//calculate angles value
			float pos_x_phi; 
			if (coeff_phi < 0.5)
				pos_x_phi = lerp(Pi_over_4, 0, coeff_phi*2); 
			else 
				pos_x_phi = lerp(2*Pi, 7*Pi_over_4, (coeff_phi-0.5)*2);

			float pos_x_theta = lerp(Pi_over_4, 3*Pi_over_4, coeff_theta);

			float neg_x_phi = lerp(5*Pi_over_4, 3*Pi_over_4, coeff_phi);
			float neg_x_theta = lerp(Pi_over_4, 3*Pi_over_4, coeff_theta);

			float pos_y_phi;
			float pos_y_theta;
			if (coeff_theta < 0.5)
			{
				pos_y_phi = slerp(Pi, 0, coeff_phi);
				pos_y_theta = lerp(Pi_over_4, 0, coeff_theta*2);
			}
			else
			{
				pos_y_phi = slerp(Pi, 2*Pi, coeff_phi);
				pos_y_theta = lerp(0, Pi_over_4, (coeff_theta-0.5)*2);
			}

			float neg_y_phi;
			float neg_y_theta;
			if (coeff_theta < 0.5)
			{
				neg_y_phi = slerp(Pi, 0, coeff_phi);
				neg_y_theta = lerp(3*Pi_over_4, 2*Pi, coeff_theta*2);
			}
			else
			{
				neg_y_phi = slerp(Pi, 2*Pi, coeff_phi);
				neg_y_theta = lerp(Pi, 3*Pi_over_4, (coeff_theta-0.5)*2);
			}

			float pos_z_phi = lerp(Pi, 0, coeff_phi);
			float pos_z_theta = lerp(Pi_over_4, 3*Pi_over_4, coeff_theta);

			float neg_z_phi = lerp(7*Pi_over_4, 5*Pi_over_4, coeff_phi);
			float neg_z_theta = lerp(Pi_over_4, 3*Pi_over_4, coeff_theta);

			//calulate spherica harmonics coefficients
			uint sh_index = groupId.z;
			float SH_value_posx = calc_SH_value(sh_index, pos_x_phi, pos_x_theta);
			float SH_value_negx = calc_SH_value(sh_index, neg_x_phi, neg_x_theta);
			float SH_value_posy = calc_SH_value(sh_index, pos_y_phi, pos_y_theta);
			float SH_value_negy = calc_SH_value(sh_index, neg_y_phi, neg_y_theta);
			float SH_value_posz = calc_SH_value(sh_index, pos_z_phi, pos_z_theta);
			float SH_value_negz = calc_SH_value(sh_index, neg_z_phi, neg_z_theta);

			//calculate partial sums
			partial_sum[index_x][index_y] += pos_x * SH_value_posx * dW;
			partial_sum[index_x][index_y] += neg_x * SH_value_negx * dW;
			partial_sum[index_x][index_y] += pos_y * SH_value_posy * dW;
			partial_sum[index_x][index_y] += neg_y * SH_value_negy * dW;
			partial_sum[index_x][index_y] += pos_z * SH_value_posz * dW;
			partial_sum[index_x][index_y] += neg_z * SH_value_negz * dW;
		}
	}

	//Let one thread to sum up all results in the group
	if (index_x == 0 && index_y == 0)
	{
		SH_Coeff[groupIndex] = 0;
		for (uint i = 0; i < threads_in_block; i++)  
		{
			for (uint j = 0; j < threads_in_block; j++)
			{
				SH_Coeff[groupIndex] += partial_sum[i][j];
			}
		}
	}
}