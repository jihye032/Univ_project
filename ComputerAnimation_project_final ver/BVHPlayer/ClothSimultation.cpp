#include "ClothSimulation.h"
#include "BVH_Animation.h"


Particle g_particles[PARTICLE_NUM_W][PARTICLE_NUM_H];
Sphere g_sphere;

bool g_flag_moving_sphere = false;

void InitCloth()
{
	std::cout << "cloth_cape : " << g_cape_pos << std::endl;
	// One corner point of the cloth.
	cml::vector3d cloth_corner_p;
	cloth_corner_p[0] = g_cape_pos[0];
	cloth_corner_p[1] = g_cape_pos[1];
	cloth_corner_p[2] = g_cape_pos[2];



	for ( int i=0; i<PARTICLE_NUM_W; i++ )
	{
		for ( int j=0; j<PARTICLE_NUM_H; j++ )
		{
			Particle &p = g_particles[i][j];

			// Set initial positions of the particles.
			p.pos_[0] = cloth_corner_p[0] - i*(CLOTH_W/PARTICLE_NUM_W);
			p.pos_[1] = cloth_corner_p[1];
			p.pos_[2] = cloth_corner_p[2] - j*(CLOTH_H/PARTICLE_NUM_H);


			// Set initial velocity of the particles.
			p.vel_[0] = 0;
			p.vel_[1] = 0;
			p.vel_[2] = 0;

		}
	}
	UpdateNormals();

}

void InitSphere()
{
	g_sphere.radius_ = 33.0;
	g_sphere.pos_[0] = g_spine_pos[0];
	g_sphere.pos_[1] = g_spine_pos[1];
	g_sphere.pos_[2] = g_spine_pos[2];
}


void SimulationStep()
{
	Particle p0 = g_particles[0][0];
	Particle p1 = g_particles[0][PARTICLE_NUM_H-1];

	p0.pos_ = g_cape_pos;
	p0.pos_[0] = p0.pos_[0]-11;
	p0.pos_[1] = p0.pos_[1]+12;
	p1.pos_ = g_cape_pos2;
	p1.pos_[0] = p1.pos_[0]-11;
	p1.pos_[1] = p1.pos_[1]+12;

	g_sphere.pos_ = g_spine_pos;

	ResetForce();
	AddGravity();
	AddSpringForce();
	AddFriction();
	AddAeroDragForce();
	//EulerMethod();
	MidPointMethod();

	CollisionResponse();

	

	g_particles[0][0] = p0;
	g_particles[0][PARTICLE_NUM_H-1] = p1;


	UpdateNormals();

	if ( g_flag_moving_sphere )
	{
		static double t=0.0;
		t += TIME_STEP;
		g_sphere.pos_[0] = sin(t) * 200;
	}
}


void UpdateNormals()
{
	for ( int i=0; i<PARTICLE_NUM_W; i++ )
	for ( int j=0; j<PARTICLE_NUM_H; j++ )
	{
		cml::vector3d n ;
		int i0 = (i-1 >= 0) ? i-1 : 0;
		int i1 = (i+1 < PARTICLE_NUM_W) ? i+1 : PARTICLE_NUM_W-1;

		int j0 = (j-1 >= 0) ? j-1 : 0;
		int j1 = (j+1 < PARTICLE_NUM_H) ? j+1 : PARTICLE_NUM_H-1;
		

		cml::vector3d p[4];
		p[0] = g_particles[i0][j].pos_;
		p[1] = g_particles[i1][j].pos_;
		p[2] = g_particles[i][j0].pos_;
		p[3] = g_particles[i][j1].pos_;

		g_particles[i][j].normal_ = cml::cross(p[3]-p[2],p[1]-p[0]).normalize();
	
	}
}






void ResetForce()
{
	for ( int i=0; i<PARTICLE_NUM_W; i++ )
	for ( int j=0; j<PARTICLE_NUM_H; j++ )
	{
		Particle &p = g_particles[i][j];

		p.force_[0] = 0.0;
		p.force_[1] = 0.0;
		p.force_[2] = 0.0;
	}
}

void AddGravity()
{
	const static cml::vector3d gravity(0, ACC_GRAVITY, 0);
	
	for ( int i=0; i<PARTICLE_NUM_W; i++ )
	for ( int j=0; j<PARTICLE_NUM_H; j++ )
	{
		Particle &p = g_particles[i][j];

		p.force_ += gravity * P_MASS;
	}
}



void AddSpringForce()
{
	for ( int i=0; i<PARTICLE_NUM_W-1; i++ )
	for ( int j=0; j<PARTICLE_NUM_H-1; j++ )
	{
		Particle &p1 = g_particles[i][j];

		for ( int ii=0; ii<2; ii++ )
		for ( int jj=0; jj<2; jj++ )
		{
			if ( ii == 0 && jj == 0 ) continue;
			
			Particle &p2 = g_particles[i+ii][j+jj];



			double spring_len;

			if ( ii==1 && jj==1 )
				spring_len = REST_D_SPRING_LEN;
			else
				spring_len = REST_SPRING_LEN;


			

			cml::vector3d p1_to_p2 = p2.pos_ - p1.pos_;
			double len = p1_to_p2.length();
			cml::vector3d dir = p1_to_p2 / len;

			cml::vector3d delta_v = p1.vel_ - p2.vel_;

			cml::vector3d f1 = 
							( KS_SPRING * (len-spring_len) // spring force
							+ KD_SPRING * cml::dot(delta_v, -1*dir) ) // damping
							* dir;

			cml::vector3d f2 = -1*f1;


			p1.force_ += f1;
			p2.force_ += f2;

		}
	}
}

void AddAeroDragForce()
{
	for ( int i=0; i<PARTICLE_NUM_W; i++ )
	for ( int j=0; j<PARTICLE_NUM_H; j++ )
	{
		Particle &p = g_particles[i][j];

		cml::vector3d f = KD_AERO_DRAG * -1 * (g_particles[i][j].vel_);
		p.force_ += f;
	}
}


void AddFriction()
{
	const static cml::vector3d gravity(0, ACC_GRAVITY, 0);

	// with Ground
	for ( int i=0; i<PARTICLE_NUM_W; i++ )
	for ( int j=0; j<PARTICLE_NUM_H; j++ )
	{
		Particle &p = g_particles[i][j];

		if ( p.pos_[1] <= 0.0 )
		{

			// Moving Direction
			cml::vector3d dir;
			dir[0] = p.vel_[0];
			dir[1] = 0;
			dir[2] = p.vel_[2];

			cml::vector3d f = KF_GROUND * gravity.length() * dir * -1;

			p.force_ += f;
		}
	}


	// with Sphere
	for ( int i=0; i<PARTICLE_NUM_W; i++ )
	for ( int j=0; j<PARTICLE_NUM_H; j++ )
	{
		Particle &p = g_particles[i][j];

		cml::vector3d p_to_sphere = g_sphere.pos_ - p.pos_;

		if ( cml::length( p_to_sphere  ) <= g_sphere.radius_  )
		{

			cml::vector3d normal = p_to_sphere.normalize();

			// Normal Force
			cml::vector3d normal_force = normal * cml::dot(p.force_, normal);

			// Moving Direction
			cml::vector3d v_n = normal * cml::dot(p.vel_, normal);
			cml::vector3d v_t = p.vel_ - v_n;
			cml::vector3d dir = v_t.normalize();

			cml::vector3d f = KF_SPHERE * normal_force.length() * dir * -1;

			p.force_ += f;
		}
	}
}


void EulerMethod()
{
	// Euler Step
	for ( int i=0; i<PARTICLE_NUM_W; i++ )
	for ( int j=0; j<PARTICLE_NUM_H; j++ )
	{
		Particle &p = g_particles[i][j];

		cml::vector3d acc_ = p.force_ / P_MASS;

		p.vel_ = p.vel_ + TIME_STEP * acc_;
		p.pos_ = p.pos_ + TIME_STEP * p.vel_;
	}
}


void MidPointMethod()
{
	for ( int i=0; i<PARTICLE_NUM_W; i++ )
	for ( int j=0; j<PARTICLE_NUM_H; j++ )
	{
		Particle &p = g_particles[i][j];

		cml::vector3d acc_ = p.force_ / P_MASS;

		cml::vector3d euler_v = p.vel_ + 0.5*TIME_STEP * acc_;
		cml::vector3d euler_p = p.pos_ + 0.5*TIME_STEP * euler_v;

		cml::vector3d mid_v =  0.5*(p.vel_ + euler_v);

		p.vel_ = p.vel_ + TIME_STEP * acc_;
		p.pos_ = p.pos_ + TIME_STEP * mid_v;

	}
}




void CollisionResponse()
{
	// with Ground 
	for ( int i=0; i<PARTICLE_NUM_W; i++ )
	for ( int j=0; j<PARTICLE_NUM_H; j++ )
	{
		Particle &p = g_particles[i][j];

		if ( p.pos_[1] < 0.0 && p.vel_[1] < 0 )
		{
			p.vel_[1] = KE_GROUND * -1 * p.vel_[1];
			p.pos_[1] = 0.0;
		}

	}


	// with Sphere
	for ( int i=0; i<PARTICLE_NUM_W; i++ )
	for ( int j=0; j<PARTICLE_NUM_H; j++ )
	{
		Particle &p = g_particles[i][j];

		cml::vector3d p_to_sphere = g_sphere.pos_ - p.pos_;
		double collision_depth = g_sphere.radius_ - p_to_sphere.length();

		if ( collision_depth > 0 )
		{

			cml::vector3d normal = p_to_sphere.normalize();

			cml::vector3d v_n = normal * cml::dot(p.vel_, normal);
			cml::vector3d v_t = p.vel_ - v_n;

			p.vel_ = (KE_SPHERE * -1 * v_n) + v_t;

			p.pos_ += collision_depth * -1 *  normal;
		}
	}
}