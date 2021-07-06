
#pragma once

#include <cuda.h>
#include <cuda_runtime.h>
#include <cuda_runtime_api.h>


struct Screen
{
	float w;	// horizontal length
	float h;	// vertical length
	
	float min_x;	// left most point
	float max_y;	// top point
	float z;	// dist from eye

	

	int resolution_w;
	int resolution_h;

	float pixel_w;	// width of a pixel (w/resolution_w)
	float pixel_h;	// height of a pixel (h/resolution_h)
};

struct Ray
{
	float3 p0;	//	starting point (Eye)
	float3 v;	// Normalized Direction
};



struct Sphere
{
	float3 p;	// center position
	float r;	// radius

	// phong shading values
	float3 kd;
	float3 ka;
	float ks;	// 항상 무채색이라고 가정하고 1채널로 나타낸다.
	float n;

	bool reflection;
};

struct Plane
{
	float3 p;
	float3 N; // normal vector
	float d;

	float3 kd;
	float3 ka;
	float ks;
	float n;

	bool reflection;
};

struct Cone
{
	float3 p;
	float3 N;
	float angle;

	float3 kd;
	float3 ka;
	float ks;
	float n;

	bool reflection;
};

void AllocGPUBuffer(int r_w, int r_h);
void UpdateImage(uchar3* h_color_buffer, float elapsed_time);
void FreeGPUBuffer();
