#pragma once

#include "cml/cml.h"


#define CLOTH_W 70.0
#define CLOTH_H 40.0
#define PARTICLE_NUM_W 7
#define PARTICLE_NUM_H 4
#define TIME_STEP 0.0005

#define REST_SPRING_LEN (CLOTH_W/PARTICLE_NUM_W)		// �α��� �� ��ƼŬ ������ ������ ����
#define REST_D_SPRING_LEN (REST_SPRING_LEN*1.41421)	// �밢������ �α��� �� ��ƼŬ ������ ������ ����
#define KS_SPRING 0.8	// ���Ǹ� ���
#define KD_SPRING 0.02	// ���Ǹ� Damping ���

#define P_MASS 0.001 // ��ƼŬ ����

#define ACC_GRAVITY (-9.8 * 10) // �߷� ���ӵ�
#define KD_AERO_DRAG 0.001 // �������� ���

#define KE_GROUND 0.001 // �ٴڰ� �浹�� ź�� ���
#define KF_GROUND 0.001 // �ٴڰ��� ����� ���

#define KE_SPHERE 0.001 // ���� �浹�� ź�� ���
#define KF_SPHERE 0.01 // ������ ����� ���


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