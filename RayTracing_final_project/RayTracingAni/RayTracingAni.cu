
#include "RayTracingAni.h"
#include "VecMath.h"
#include <stdio.h>
#include <iostream>
#include <math.h>

#define NUM_SPHERE 3

#define BACK 0
#define CEIL 1
#define GROUND 2
#define LEFT 3
#define RIGHT 4

#define M_PI 3.14159265358979323846


// Global Variables
__device__ float3 eye_p;
__device__ Screen screen;
__device__ Sphere spheres[NUM_SPHERE];
__device__ Plane plane[5];
__device__ Cone cone;

__device__ float3 light_source;	// Light Position
__device__ float I;		// Light Intensity



__device__ Ray MakeRay(int pixel_x, int pixel_y)
{
	float3 pixel_center;
	pixel_center.x = screen.min_x + pixel_x*screen.pixel_w + (screen.pixel_w/2.f);
	pixel_center.y = screen.max_y - pixel_y*screen.pixel_h + (screen.pixel_h/2.f);
	pixel_center.z = screen.z;

	Ray r;
	r.p0 = eye_p;

	r.v = Sub(pixel_center, r.p0);
	r.v = Normalize(r.v);

	return r;
}


__global__ void InitEnv(int r_w, int r_h, float elapsed_time)
{
	eye_p = make_float3(0.f, 0.f, -200.f);

	
	screen.w = 300.f;
	screen.h = 200.f;
	screen.min_x = -150.f;
	screen.max_y = 100.f;
	screen.z = -400.f;
	screen.resolution_w = r_w;
	screen.resolution_h = r_h;
	screen.pixel_w = screen.w / r_w;
	screen.pixel_h = screen.h / r_h;

	spheres[0].p = make_float3(0.f, 0.f, -1000.f);
	spheres[0].r = 50.;
	spheres[0].kd = make_float3(0.f, 0.f, 1.f);
	spheres[0].ka = Scale(0.1f, spheres[0].kd);
	spheres[0].ks = 0.7f;
	spheres[0].n = 50.0f;
	spheres[0].reflection = true;

	spheres[1].p = make_float3(sin(elapsed_time*2.)*(-200.f), sin(elapsed_time*2.)*200.f, -1000.f+cos(elapsed_time*1.5f)*300.f);
	spheres[1].r = 50.;
	spheres[1].kd = make_float3(1.f, 0.f, 0.f);
	spheres[1].ka = Scale(0.1f, spheres[1].kd);
	spheres[1].ks = 0.7f;
	spheres[1].n = 50.0f;
	spheres[1].reflection = true;

	spheres[2].p = make_float3(sin(elapsed_time*1.5f)*200.f, sin(elapsed_time*2.)*(-200.f), -1000.f+cos(elapsed_time*2.)*300.f);
	spheres[2].r = 50.;
	spheres[2].kd = make_float3(0.f, 1.f, 0.f);
	spheres[2].ka = Scale(0.1f, spheres[2].kd);
	spheres[2].ks = 0.7f;
	spheres[2].n = 50.0f;
	spheres[2].reflection = true;

	plane[BACK].p = make_float3(0.f, 0.f, -1300.f);
	plane[BACK].N = make_float3(0.f, 0.f, 1.f);
	plane[BACK].kd = make_float3(0.7f, 0.7f, 0.7f);
	plane[BACK].ka = Scale(0.1f, plane[BACK].kd);
	plane[BACK].ks = 0.f;
	plane[BACK].n = 0.0f;
	plane[BACK].reflection = false;

	plane[CEIL].p = make_float3(0.f, 250.f, -1000.f);
	plane[CEIL].N = make_float3(0.f, -1.f, 0.f);
	plane[CEIL].kd = make_float3(0.7f, 0.7f, 0.3f);
	plane[CEIL].ka = Scale(0.1f, plane[CEIL].kd);
	plane[CEIL].ks = 0.f;
	plane[CEIL].n = 0.0f;
	plane[CEIL].reflection = false;

	plane[GROUND].p = make_float3(0.f, -250.f, -1000.f);
	plane[GROUND].N = make_float3(0.f, 1.f, 0.f);
	plane[GROUND].kd = make_float3(0.3f, 0.3f, 0.7f);
	plane[GROUND].ka = Scale(0.1f, plane[GROUND].kd);
	plane[GROUND].ks = 0.f;
	plane[GROUND].n = 0.0f;
	plane[GROUND].reflection = false;

	plane[LEFT].p = make_float3(-400.f, 0.f, -1000.f);
	plane[LEFT].N = make_float3(1.f, 0.f, 0.f);
	plane[LEFT].kd = make_float3(0.7f, 0.3f, 0.3f);
	plane[LEFT].ka = Scale(0.1f, plane[LEFT].kd);
	plane[LEFT].ks = 0.7f;
	plane[LEFT].n = 100.0f;
	plane[LEFT].reflection = true;

	plane[RIGHT].p = make_float3(400.f, 0.f, -1000.f);
	plane[RIGHT].N = make_float3(-1.f, 0.f, 0.f);
	plane[RIGHT].kd = make_float3(0.3f, 0.7f, 0.3f);
	plane[RIGHT].ka = Scale(0.1f, plane[RIGHT].kd);
	plane[RIGHT].ks = 0.7f;
	plane[RIGHT].n = 100.0f;
	plane[RIGHT].reflection = true;


	light_source = make_float3(300.f, 200.f, -650.f);
	I = 1.f;
}

__device__ bool TestIntersection(Ray r, Sphere s, float3 &out_P, float3 &out_N)
{
	float a = 1;
	float b = 2*Dot(r.v, Sub(r.p0, s.p));
	float c = Length2(Sub(r.p0, s.p)) - (s.r*s.r);


	// Determinant Test
	float d = b*b - 4*a*c;
	if ( d < 0 ) return false;

	float t = (-b - sqrt(d))/2.f;

	if ( t < 0.01 ) return false;

	out_P = Add(r.p0, Scale(t, r.v));
	out_N = Normalize( Sub(out_P, s.p) );

	return true;
}

__device__ bool TestIntersection(Ray r, Plane p, float3 &out_P, float3 &out_N)
{
	p.d = -Dot(p.p, p.N);
	if( (Dot(p.p, p.N) + p.d) != 0 ) return false;
	
	float tmp = Dot(r.v, p.N);
	if( tmp == 0 ) return false;

	float t = -(Dot(r.p0, p.N) + p.d) / tmp;
	if( t < 0.01 ) return false;

	//if( Dot(p.N, Sub(r.p0, p.p)) != 0 ) return false;

	out_P = Add( r.p0, Scale(t, r.v) );
	out_N = Normalize( p.N );

	return true;
}

__device__ bool TestIntersection(Ray r, Cone c, float3 &out_P, float3 &out_N)
{
	if ( Length(Sub(r.p0, c.N)) == 0 ) return false;


}

__device__ bool TestIntersection(Ray r, float3 &out_P, float3 &out_N, float3 &out_kd, float3 &out_ka, float &out_ks, float &out_n, float &dis)
{
	bool flag_hit = false;
	float distance = 99999;

	for ( int i=0; i<NUM_SPHERE; i++ )
	{
		float3 tmp_P;
		float3 tmp_N;
		if ( TestIntersection(r, spheres[i], tmp_P, tmp_N) )
		{
			flag_hit = true;
			float tmp_dist = Length(Sub(eye_p, tmp_P));

			if ( distance > tmp_dist )
			{
				dis = Length(Sub(r.p0, tmp_P));

				distance = tmp_dist;
				out_P = tmp_P;
				out_N = tmp_N;
				out_kd = spheres[i].kd;
				out_ka = spheres[i].ka;
				out_ks = spheres[i].ks;
				out_n = spheres[i].n;
			}
		}
	}

	for( int i=0; i<5; i++ )
	{
		float3 tmp_P;
		float3 tmp_N;
		if ( TestIntersection(r, plane[i], tmp_P, tmp_N) )
		{
			float tmp_dist = Length(Sub(eye_p, tmp_P));
			flag_hit = true;

			if ( distance > tmp_dist )
			{
				dis = Length(Sub(r.p0, tmp_P));

				distance = tmp_dist;
				out_P = tmp_P;
				out_N = tmp_N;
				out_kd = plane[i].kd;
				out_ka = plane[i].ka;
				out_ks = plane[i].ks;
				out_n = plane[i].n;

			}
		}
	}

	return flag_hit;
}


__device__ bool TestIntersection(Ray r, float &dis)
{
	float3 P, N;
	float3 kd, ka;
	float ks, n;

	return TestIntersection(r, P, N, kd, ka, ks, n, dis);
}


__device__ float3 DiffuseReflection(float3 L, float3 N, float3 kd)
{
	if ( Dot(N, L) < 0.f) return make_float3(0.f, 0.f, 0.f);
	float3 d = Scale(I*Dot(N, L), kd);
	return d;
}

__device__ float SpecularReflection(float3 L, float3 R, float3 V, float ks, float n)
{
	float s = ks*I*pow(Dot(R, V), n);

	if ( Dot(R, V) < 0 ) return 0;

	return s;
}

__device__ float SpecularReflection2(float3 L, float3 N, float3 P, float ks, float n)
{
	float3 R = Normalize(Sub(Scale(2.f*Dot(L, N), N), L));
	float3 V = Normalize(Sub(eye_p, P));

	return SpecularReflection(L, R, V, ks, n);
}


__device__ bool RayCast(Ray ray, float3& out_color, Ray &out_reflected_ray)
{
	float3 color;

	float3 P, N;
	float3 kd, ka;
	float ks;
	float n, distance;


	if ( TestIntersection(ray, P, N, kd, ka, ks, n, distance) )
	{
		float3 L = Normalize(Sub(light_source, P));
		float3 diffuse = DiffuseReflection(L, N, kd);	//diffuse
		float3 ambient = ka;	//ambient
		float3 specular = Scale(SpecularReflection2(L, N, P, ks, n), make_float3(1.f, 1.f, 1.f));

		
		bool flag_hit = false;

		Ray P_to_L;
		P_to_L.p0 = P;
		P_to_L.v = L;

		float tmp_dis;
		flag_hit = TestIntersection(P_to_L, tmp_dis);

		float LtoP = Length(Sub(light_source, P));

		if ( flag_hit )
		{
			if ( LtoP > tmp_dis )
				color = ambient;

			else
			{
				color = Add(ambient, diffuse);
				color = Add(color, specular);
			}
		}
		else
		{
			color = Add(ambient, diffuse);
			color = Add(color, specular);
		}
		
		out_reflected_ray.p0 = P;
		float3 in_R = Scale(-1.f, ray.v);
		out_reflected_ray.v = Sub(Scale(2*Dot(in_R, N), N), in_R);
	}
	else
	{
		return false;
	}

	out_color = color;


	return true;
}

__device__ bool RayReflection(Ray ray, float3 &out_color, Ray &reflected_ray)
{
	float3 color = make_float3(0.f, 0.f, 0.f);
	for ( int j=0; j<7; j++ )
	{
		float3 tmp_color;
		if ( RayCast(ray, tmp_color, reflected_ray)  )
		{
			ray = reflected_ray;
			color = Add(color, Scale(0.4f, tmp_color));

		}
		else
		{
			break;
		}
	}

	out_color = color;

	return true;
}

__global__ void RayTracing(uchar3* d_color_buffer)
{
	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	int pixel_x_index = tid%screen.resolution_w;
	int pixel_y_index = tid/screen.resolution_w;
	
	if ( tid < screen.resolution_w*screen.resolution_h )
	{
		
		float3 color;  
		color = make_float3(0.f, 0.f, 0.f);

		Ray ray = MakeRay(pixel_x_index, pixel_y_index);
		Ray next_ray;
		
		for ( int j=0; j<1; j++ )
		{
			float3 tmp_color;
			if ( RayReflection(ray, tmp_color, next_ray) )
			{
				color = Add(color, tmp_color);
				ray = next_ray;
			}
			else
			{
				break;
			}
		}


		// Scale up the color values to 0~255 and Copy the result color
		// R
		int tmp = round(color.x*255);
		if ( tmp < 0 ) tmp = 0;
		if ( tmp > 255 ) tmp = 255;
		d_color_buffer[tid].x = tmp;

		// G
		tmp = round(color.y*255);
		if ( tmp < 0 ) tmp = 0;
		if ( tmp > 255 ) tmp = 255;
		d_color_buffer[tid].y = tmp;

		// B
		tmp = round(color.z*255);
		if ( tmp < 0 ) tmp = 0;
		if ( tmp > 255 ) tmp = 255;
		d_color_buffer[tid].z = tmp;
	}
}


uchar3 *d_color_buffer;	// GPU buffer
int win_resolution_w, win_resolution_h;

void AllocGPUBuffer(int r_w, int r_h)
{
	win_resolution_w = r_w;
	win_resolution_h = r_h;

	cudaMalloc(&d_color_buffer, r_w*r_h*sizeof(uchar3));
}

void UpdateImage(uchar3* h_color_buffer, float elapsed_time)
{
    cudaError_t cudaStatus;
	int num_rays = win_resolution_w*win_resolution_h;

	InitEnv<<<1, 1>>>(win_resolution_w, win_resolution_h, elapsed_time);
	RayTracing<<<num_rays/512+1, 512>>>(d_color_buffer);

	// Check for any errors launching the kernel
    cudaStatus = cudaGetLastError();
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "addKernel launch failed: %s\n", cudaGetErrorString(cudaStatus));
    }

	cudaDeviceSynchronize();

	cudaMemcpy(h_color_buffer, d_color_buffer, win_resolution_w*win_resolution_h*sizeof(uchar3), cudaMemcpyDeviceToHost);
}



void FreeGPUBuffer()
{
	cudaFree(d_color_buffer);
}
