#pragma once

#include "cml/cml.h"


#define CLOTH_W 70.0
#define CLOTH_H 40.0
#define PARTICLE_NUM_W 7
#define PARTICLE_NUM_H 4
#define TIME_STEP 0.0005

#define REST_SPRING_LEN (CLOTH_W/PARTICLE_NUM_W)		// 인근한 두 파티클 사이의 스프링 길이
#define REST_D_SPRING_LEN (REST_SPRING_LEN*1.41421)	// 대각선으로 인근한 두 파티클 사이의 스프링 길이
#define KS_SPRING 0.8	// 스피링 계수
#define KD_SPRING 0.02	// 스피링 Damping 계수

#define P_MASS 0.001 // 파티클 질량

#define ACC_GRAVITY (-9.8 * 10) // 중력 가속도
#define KD_AERO_DRAG 0.001 // 공기저항 계수

#define KE_GROUND 0.001 // 바닥과 충돌시 탄성 계수
#define KF_GROUND 0.001 // 바닥과의 운동마찰 계수

#define KE_SPHERE 0.001 // 구와 충돌시 탄성 계수
#define KF_SPHERE 0.01 // 구와의 운동마찰 계수


struct Particle
{
	cml::vector3d pos_;
	cml::vector3d normal_;
	cml::vector3d vel_;
	cml::vector3d force_;
};

struct Sphere
{
	cml::vector3d pos_;
	double radius_;
};



extern Particle g_particles[PARTICLE_NUM_W][PARTICLE_NUM_H];
extern Sphere g_sphere_1;
extern Sphere g_sphere_2;


void InitCloth();
void InitSphere();

void CollisionResponse();
void ResetForce();
void AddGravity();
void AddSpringForce();
void AddFriction();
void AddAeroDragForce();

void SimulationStep();
void EulerMethod();
void MidPointMethod();

void UpdateNormals();