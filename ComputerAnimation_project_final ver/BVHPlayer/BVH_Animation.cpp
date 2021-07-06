#include "ClothAni.h"
#include "ClothSimulation.h"

#include "BaseLib/GL4U/GL_Renderer.h"
#include "BaseLib/GL4U/GL_RenderableObj.h"
#include "BaseLib/GL4U/GL_ResourceManager.h"

#include "GL/freeglut.h"
#include "motion/ml.h"

extern float g_FPS;
extern float g_elapsed_time ;
extern mg::GL_Renderer *g_renderer;


ml::Motion g_motion;
ml::Motion back_motion;
ml::Motion back2_motion;
int g_frame = 0;
static bool g_cloth_display = true;
extern bool g_flag_moving_sphere;
cml::vector3d g_cape_pos;
cml::vector3d g_cape_pos2;
cml::vector3d g_spine_pos;

cml::vector3d v;
cml::vector3d p[5];
cml::vectord x(16);
cml::vectord y(16);
cml::vectord z(16);

// InitAnimation() is called 1 time at the beginning.
void InitAnimation()
{
	g_FPS = 30.;
	double distance_ = 80.;

	// spline
	{
		p[0] = cml::vector3d(distance_, 0, 0);
		p[1] = cml::vector3d(0, 0, -1*distance_);
		p[2] = cml::vector3d(-1*distance_, 0, 0);
		p[3] = cml::vector3d(0, 0, distance_);
		p[4] = cml::vector3d(distance_, 0, 0);

		cml::matrixd matrix(16, 16);
		matrix.zero();

		for(int i=0; i<4; i++)
		{
			matrix.set_basis_element(i*4+0, i*4+0, 1.0f);
			matrix.set_basis_element(i*4+1, i*4+0, 1.0f);
			matrix.set_basis_element(i*4+2, i*4+0, 1.0f);
			matrix.set_basis_element(i*4+3, i*4+0, 1.0f);

			matrix.set_basis_element(i*4+0, i*4+1, 0.0f);
			matrix.set_basis_element(i*4+1, i*4+1, 0.0f);
			matrix.set_basis_element(i*4+2, i*4+1, 0.0f);
			matrix.set_basis_element(i*4+3, i*4+1, 1.0f);

			if ( i<3 ){
				matrix.set_basis_element(i*4+0, i*4+2, 3.0f);
				matrix.set_basis_element(i*4+1, i*4+2, 2.0f);
				matrix.set_basis_element(i*4+2, i*4+2, 1.0f);
				matrix.set_basis_element(i*4+3, i*4+2, 0.0f);
				matrix.set_basis_element(i*4+6, i*4+2, -1.0f);

				matrix.set_basis_element(i*4+0, i*4+3, 6.0f);
				matrix.set_basis_element(i*4+1, i*4+3, 2.0f);
				matrix.set_basis_element(i*4+2, i*4+3, 0.0f);
				matrix.set_basis_element(i*4+3, i*4+3, 0.0f);
				matrix.set_basis_element(i*4+5, i*4+3, -2.0f);
			}
			else {
				matrix.set_basis_element(i*4+0, i*4+2, 3.0f);
				matrix.set_basis_element(i*4+1, i*4+2, 2.0f);
				matrix.set_basis_element(i*4+2, i*4+2, 1.0f);
				matrix.set_basis_element(i*4+3, i*4+2, 0.0f);
				matrix.set_basis_element(2, i*4+2, -1.0f);

				matrix.set_basis_element(i*4+0, i*4+3, 6.0f);
				matrix.set_basis_element(i*4+1, i*4+3, 2.0f);
				matrix.set_basis_element(i*4+2, i*4+3, 0.0f);
				matrix.set_basis_element(i*4+3, i*4+3, 0.0f);
				matrix.set_basis_element(1, i*4+3, -2.0f);
			}
		}

		cml::vectord b_x(16);
		cml::vectord b_y(16);
		cml::vectord b_z(16);

		b_x[0] = 0;	b_y[0] = 0;	b_z[0] = 0;
		for(int i=0; i<4; i++)
		{
			b_x[i*4] = p[i+1][0];		b_x[i*4+1] = p[i][0];
			b_y[i*4] = p[i+1][1];		b_y[i*4+1] = p[i][1];
			b_z[i*4] = p[i+1][2];		b_z[i*4+1] = p[i][2];

			b_x[i*4+2] = 0;				b_x[i*4+3] = 0;
			b_y[i*4+2] = 0;				b_y[i*4+3] = 0;
			b_z[i*4+2] = 0;				b_z[i*4+3] = 0;

		}
		
		x = cml::inverse(matrix) * b_x;
		y = cml::inverse(matrix) * b_y;
		z = cml::inverse(matrix) * b_z;
	}

	// motion
	{
		ml::Motion g_motion0;
		g_motion0.LoadBVH("data/jyp_jyp_kidsSong.bvh");
		g_motion0.CropMotion(100, 200, &g_motion);

		ml::Motion g_motion1;
		ml::Motion jump_motion;
		jump_motion.LoadBVH("data/jyp_jyp_kidsSong.bvh");
		jump_motion.CropMotion(560, 330, &g_motion1);

		ml::Motion g_motion2;
		ml::Motion run_motion;
		run_motion.LoadBVH("data/jyp_jyp_vallet.bvh");
		run_motion.CropMotion(190, 400, &g_motion2);

		g_motion.stitch(g_motion1, 1);
		g_motion.stitch(g_motion2, 1);
	}

	// back motion
	{
		ml::Motion b_motion;
		b_motion.LoadBVH("data/jyp_jyp_vallet.bvh");
		b_motion.CropMotion(405, 25, &back_motion);
		b_motion.CropMotion(405, 25, &back2_motion);

		cml::matrix44d trans;
		cml::matrix_translation(trans, -250., 0., -200.);
		cml::matrix44d trans2;
		cml::matrix_translation(trans2, -250., 0., 200.);

		back_motion.ApplyTransf(trans);
		back2_motion.ApplyTransf(trans2);
	}

	// 처음에 캐릭터의 위치를 알려주기 위해 데이터를 받아옴
	{
		const ml::Body *body = g_motion.body();
		ml::Posture &pose = g_motion.posture(g_frame%g_motion.size());
		for ( int i=1; i<(int)body->num_joint(); i++ )
		{
			if ( i == body->joint_index("RightShoulder") )
			{
				g_cape_pos = pose.GetGlobalTranslation(i);
				if( g_frame == 1)
					std::cout << "pos << " << g_cape_pos << std::endl;
			}
			if ( i == body->joint_index("LeftShoulder") )
			{
				g_cape_pos2 = pose.GetGlobalTranslation(i);
				if( g_frame == 1)
					std::cout << "pos2 << " << g_cape_pos2 << std::endl;
			}
		}
	}

	InitCloth();
	InitSphere();

	// Cloth GL Object
	{
		mg::GL_VBOGroup *vb = mg::GL_ResourceManager::singleton()->CreateVBOGroup("cloth_vbo");
		vb->Init(PARTICLE_NUM_W*PARTICLE_NUM_H);
		vb->BeginVertex();
		for ( int i=0; i<PARTICLE_NUM_W; i++ )
		for ( int j=0; j<PARTICLE_NUM_H; j++ )
		{
			vb->glNormal(g_particles[i][j].normal_);
			vb->glVertex(g_particles[i][j].pos_);
		}
		vb->EndVertex();
		
		// triangle indices
		{
			int num_triangles = (PARTICLE_NUM_W-1)*(PARTICLE_NUM_H-1)*2;
			unsigned int *d = new unsigned int[num_triangles*3];
			int k=0;
			for ( int i=0; i<PARTICLE_NUM_W-1; i++ )
			for ( int j=0; j<PARTICLE_NUM_H-1; j++ )
			{
				d[k++] = (i*PARTICLE_NUM_H) + j;
				d[k++] = (i*PARTICLE_NUM_H) + j+1;
				d[k++] = ((i+1)*PARTICLE_NUM_H) + j;

				d[k++] = (i*PARTICLE_NUM_H) + j+1;
				d[k++] = ((i+1)*PARTICLE_NUM_H) + j+1;
				d[k++] = ((i+1)*PARTICLE_NUM_H) + j;
			}
			vb->SetElementIndices( num_triangles*3, d );
			delete[] d;
		}

		mg::GL_ResourceManager::singleton()->CreateRendarableObj("cloth", vb);
	}


}


// UpdateAnimation() is called g_FPS times per 1 second regularly .
void UpdateAnimation()
{
	// std::cout << g_elapsed_time << std::endl;
	g_frame += 1;
	double t = g_elapsed_time;
	t = t;

	// spline
	{
		//////////////////////////////////////////////////////////////
		/////////////////////// Translation
		int i = 0;
		if ( t>=0 && t<1 ) i=0;
		else if ( t>=1 && t<2 ) i=1;
		else if ( t>=2 && t<3 ) i=2;
		else if ( t>=3 && t<4 ) i=3;
		else{
			i = 0;
			g_elapsed_time = 0;
		}

		double time = t - i;
		if( time >=0 && time <1 ){
			v[0] = x[i*4+0]*pow(time,3) + x[i*4+1]*pow(time,2) + x[i*4+2]*(time) + x[i*4+3];
			v[1] = y[i*4+0]*pow(time,3) + y[i*4+1]*pow(time,2) + y[i*4+2]*(time) + y[i*4+3];
			v[2] = z[i*4+0]*pow(time,3) + z[i*4+1]*pow(time,2) + z[i*4+2]*(time) + z[i*4+3];
		}
	}


	// cloth
	{
		int n = (int)((1.0/g_FPS) / TIME_STEP);
		if ( n==0 ) n = 1;

		for ( int k=0; k<n; k++ )
		{
			SimulationStep();
		}

		mg::GL_VBOGroup *vbo = mg::GL_ResourceManager::singleton()->GetRenderableObj("cloth")->vao()->vbo_group();
		vbo->BeginUpdate();
		for ( int i=0; i<PARTICLE_NUM_W; i++ )
		for ( int j=0; j<PARTICLE_NUM_H; j++ )
		{
			vbo->glUpdateNormal(g_particles[i][j].normal_);
			vbo->glUpdateVertex(g_particles[i][j].pos_);
		}
		vbo->EndUpdate();
	}

}


// DrawAnimation() is called many times but you don't know how many times and when it is called.
void DrawSphere()
{
	// sphere
	{
		double radius_ = 3.0;
		cml::matrix44d body_t_m;
		v = v + g_spine_pos;
		v[1] = 20 * cos(M_PI * g_elapsed_time) + g_spine_pos[1];
		cml::matrix_translation(body_t_m, v);
		body_t_m = body_t_m;

		cml::matrix44d s_m;
		cml::matrix_scale(s_m, radius_, radius_, radius_);
		cml::matrix44d body_m = body_t_m * s_m;
		g_renderer->SetModelMatrix(body_m);
		g_renderer->SetColor(1., 1., 1.);
		g_renderer->DrawSphere();
	}
	// sphere 2
	{
		double radius_ = 3.0;
		cml::matrix44d body_t_m;
		v[1] = 20 * sin(M_PI * g_elapsed_time) + g_spine_pos[1];
		cml::matrix_translation(body_t_m, v);
		body_t_m = body_t_m;

		cml::matrix44d s_m;
		cml::matrix_scale(s_m, radius_, radius_, radius_);
		cml::matrix44d body_m = body_t_m * s_m;
		g_renderer->SetModelMatrix(body_m);
		g_renderer->SetColor(0.6, 0.85, 0.9);
		g_renderer->DrawSphere();
	}
}

void DrawAnimation()
{
	// Ground Grid
	g_renderer->Draw("ground");


	// Motion
	{
		g_renderer->SetColor(0.4, 0.85, 0.9);

		const ml::Body *body = g_motion.body();
		ml::Posture &pose = g_motion.posture(g_frame%g_motion.size());
		for ( int i=1; i<(int)body->num_joint(); i++ )
		{
			cml::vector3d p1 = pose.GetGlobalTranslation(i);
			cml::vector3d p2 = pose.GetGlobalTranslation(body->parent(i));

			cml::matrix44d m;
			m.identity();

			g_renderer->SetModelMatrix(m);
			g_renderer->DrawCylinder(p1, p2, 3.);

			if ( i == body->joint_index("Spine") )
			{
				cml::vector3d p1 = pose.GetGlobalTranslation(i);
				cml::vector3d p2 = pose.GetGlobalTranslation(body->parent(i));

				g_renderer->SetModelMatrix(m);
				g_renderer->DrawCylinder(p1, p2, 3.);

				 // 치마
				 cml::matrix44d e0 = pose.GetGlobalTransf(i);
				 cml::matrix44d skr_s, skr_t, skr_r;
				 cml::matrix_translation(skr_t, -5., 0., 0.);
				 cml::matrix_scale(skr_s, 25., 10., 25.);
				 cml::matrix_rotation_world_z(skr_r, M_PI / 2.0);
				 g_renderer->SetModelMatrix(e0*skr_t*skr_r*skr_s);
				 g_renderer->DrawCylinder();

				 cml::matrix44d skr2_s, skr2_t, skr2_r;
				 cml::matrix_translation(skr2_t, -15., 0., 0.);
				 cml::matrix_scale(skr2_s, 30., 10., 30.);
				 cml::matrix_rotation_world_z(skr2_r, M_PI / 2.0);
				 g_renderer->SetModelMatrix(e0*skr2_t*skr2_r*skr2_s);
				 g_renderer->DrawCylinder();

			}
			if ( i == body->joint_index("Spine1"))
			{
				g_spine_pos = pose.GetGlobalTranslation(i);
				
				if( g_frame == 1)
					std::cout << "spine << " << g_cape_pos << std::endl;

				cml::vector3d p1 = pose.GetGlobalTranslation(i);
				cml::vector3d p2 = pose.GetGlobalTranslation(body->parent(i));

				g_renderer->SetModelMatrix(m);
				g_renderer->DrawCylinder(p1, p2, 20.);
			}

			if ( i == body->joint_index("Neck") )
			{

				cml::vector3d p1 = pose.GetGlobalTranslation(i);
				cml::vector3d p2 = pose.GetGlobalTranslation(body->parent(i));

				g_renderer->SetColor(1., 1., 1.);
				g_renderer->SetModelMatrix(m);
				g_renderer->DrawCylinder(p1, p2, 15.);

				cml::matrix44d t0 = pose.GetGlobalTransf(i); 	// get the 4x4 transformation matrix in the homogenious form (Translation + Rotation);

				// 목
				{
					cml::matrix44d e1, e2, e3;
					cml::matrix_translation(e1, 0., 0., 0.);
					cml::matrix_scale(e2, 10., 10., 10.);
					cml::matrix_rotate_about_local_x(e2, 33.);
					g_renderer->SetModelMatrix(t0*e1*e2);
					g_renderer->DrawSphere();
				}
			}

			if ( i == body->joint_index("RightArm") )
			{
				g_cape_pos = pose.GetGlobalTranslation(i);
				if( g_frame == 1)
					std::cout << "cape << " << g_cape_pos << std::endl;

				// Draw Particles
				/*
				g_renderer->SetColor(0, 0, 0);
				glPointSize(5);
				glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
				g_renderer->Draw("cloth");
		
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				*/

				// Draw cloth
				if ( g_cloth_display  )
				{
					cml::matrix44d t1, t2, t3;
					cml::matrix_translation(t1, 0., 0., 0.);
					cml::matrix_scale(t2, 1., 1., 1.);
					cml::matrix_rotate_about_local_z(t2, 0.);
					g_renderer->SetModelMatrix(t1*t2);

					g_renderer->SetColor(0.9, 0.95, 1.);
					g_renderer->Draw("cloth");
				}
			
			}
			if ( i == body->joint_index("LeftArm") )
			{
				g_cape_pos2 = pose.GetGlobalTranslation(i);
				if( g_frame == 1)
					std::cout << "cape2 << " << g_cape_pos2 << std::endl;
			}

			// Shere Head
			if ( i == body->joint_index("Head") )
			{
				cml::matrix44d t0 = pose.GetGlobalTransf(i); 	// get the 4x4 transformation matrix in the homogenious form (Translation + Rotation);

				cml::matrix44d t1, t2, t3;
				cml::matrix_translation(t1, 20., 0., 0.);
				cml::matrix_scale(t2, 20., 20., 20.);
				cml::matrix_rotate_about_local_z(t2, 0.);
				g_renderer->SetColor(0.4, 0.85, 0.9);
				g_renderer->SetModelMatrix(t0*t1*t2);
				g_renderer->DrawSphere();

				// 코
				cml::matrix44d e1, e2, e3;
				cml::matrix_translation(e1, 20., 0., -20.);
				cml::matrix_scale(e2, 1.5, 1.5, 1.5);
				cml::matrix_rotate_about_local_x(e2, 33.);
				g_renderer->SetColor(0., 0., 0.);
				g_renderer->SetModelMatrix(t0*e1*e2);
				g_renderer->DrawSphere();

				// 왼쪽 눈
				cml::matrix_translation(e1, 25., -8., -18.5);
				cml::matrix_scale(e2, 1.5, 1.5, 1.5);
				cml::matrix_rotate_about_local_x(e2, 33.);
				g_renderer->SetColor(0., 0., 0.);
				g_renderer->SetModelMatrix(t0*e1*e2);
				g_renderer->DrawSphere();

				// 오른쪽 눈
				cml::matrix_translation(e1, 25., 8., -18.5);
				cml::matrix_scale(e2, 1.5, 1.5, 1.5);
				cml::matrix_rotate_about_local_x(e2, 33.);
				g_renderer->SetColor(0., 0., 0.);
				g_renderer->SetModelMatrix(t0*e1*e2);
				g_renderer->DrawSphere();

				// 입
				cml::matrix_translation(e1, 15., 0., -18.5);
				cml::matrix_scale(e2, 1., 5., 1.);
				cml::matrix_rotate_about_local_x(e2, 33.);
				g_renderer->SetColor(0., 0., 0.);
				g_renderer->SetModelMatrix(t0*e1*e2);
				g_renderer->DrawSphere();

				cml::matrix_translation(e1, 35., 5., 0.);
				cml::matrix_scale(e2, 8., 2., 2.);
				cml::matrix_rotation_world_z(e3, -1*M_PI / 2.0);
				g_renderer->SetColor(1., 1., 1.);
				g_renderer->SetModelMatrix(t0*e1*e2*e3);
				g_renderer->DrawHemisphere();

				cml::matrix_translation(e1, 35., -5., 0.);
				cml::matrix_scale(e2, 8., 2., 2.);
				cml::matrix_rotation_world_z(e3, -1*M_PI / 2.0);
				g_renderer->SetColor(1., 1., 1.);
				g_renderer->SetModelMatrix(t0*e1*e2*e3);
				g_renderer->DrawHemisphere();
			}
		
			// Box Bodies
			else
			{
				g_renderer->SetColor(0.4, 0.85, 0.9);

				cml::matrix44d t = pose.GetGlobalTransf(i); // get the 4x4 transformation matrix in the homogenious form (Translation + Rotation);
				g_renderer->SetModelMatrix(t);
				g_renderer->DrawSphere(3.1);

			}
			if ( i == body->joint_index("RightLeg") || i == body->joint_index("LeftLeg"))
			{
				cml::vector3d p1 = pose.GetGlobalTranslation(i);
				cml::vector3d p2 = pose.GetGlobalTranslation(body->parent(i));

				g_renderer->SetColor(1., 1., 1.);
				g_renderer->SetModelMatrix(m);
				g_renderer->DrawCylinder(p1, p2, 3.);
			}
		}
	}
}

void DrawBack1()
{
	// Motion
	{
		const ml::Body *body = back_motion.body();
		ml::Posture &pose = back_motion.posture(g_frame%back_motion.size());
		g_renderer->SetColor(0.9, 0.55, 0.6);
		
		for ( int i=1; i<(int)body->num_joint(); i++ )
		{
			cml::vector3d p1 = pose.GetGlobalTranslation(i);
			cml::vector3d p2 = pose.GetGlobalTranslation(body->parent(i));

			cml::matrix44d m;
			m.identity();
			m = m;

			g_renderer->SetModelMatrix(m);
			g_renderer->DrawCylinder(p1, p2, 3.);

			if ( i == body->joint_index("Spine") )
			{
				cml::vector3d p1 = pose.GetGlobalTranslation(i);
				cml::vector3d p2 = pose.GetGlobalTranslation(body->parent(i));

				g_renderer->SetModelMatrix(m);
				g_renderer->DrawCylinder(p1, p2, 3.);

				 // 치마
				 cml::matrix44d e0 = pose.GetGlobalTransf(i);
				 cml::matrix44d skr_s, skr_t, skr_r;
				 cml::matrix_translation(skr_t, -5., 0., 0.);
				 cml::matrix_scale(skr_s, 25., 10., 25.);
				 cml::matrix_rotation_world_z(skr_r, M_PI / 2.0);
				 g_renderer->SetModelMatrix(e0*skr_t*skr_r*skr_s);
				 g_renderer->DrawCylinder();

				 cml::matrix44d skr2_s, skr2_t, skr2_r;
				 cml::matrix_translation(skr2_t, -15., 0., 0.);
				 cml::matrix_scale(skr2_s, 30., 10., 30.);
				 cml::matrix_rotation_world_z(skr2_r, M_PI / 2.0);
				 g_renderer->SetModelMatrix(e0*skr2_t*skr2_r*skr2_s);
				 g_renderer->DrawCylinder();

			}
			if ( i == body->joint_index("Spine1"))
			{
				cml::vector3d p1 = pose.GetGlobalTranslation(i);
				cml::vector3d p2 = pose.GetGlobalTranslation(body->parent(i));

				g_renderer->SetModelMatrix(m);
				g_renderer->DrawCylinder(p1, p2, 20.);
			}

			if ( i == body->joint_index("Neck") )
			{
				cml::vector3d p1 = pose.GetGlobalTranslation(i);
				cml::vector3d p2 = pose.GetGlobalTranslation(body->parent(i));

				g_renderer->SetColor(1., 1., 1.);
				g_renderer->SetModelMatrix(m);
				g_renderer->DrawCylinder(p1, p2, 15.);

				cml::matrix44d t0 = pose.GetGlobalTransf(i); 	// get the 4x4 transformation matrix in the homogenious form (Translation + Rotation);
				// 목
				{
					cml::matrix44d e1, e2, e3;
					cml::matrix_translation(e1, 0., 0., 0.);
					cml::matrix_scale(e2, 10., 10., 10.);
					cml::matrix_rotate_about_local_x(e2, 33.);
					g_renderer->SetModelMatrix(t0*e1*e2);
					g_renderer->DrawSphere();
				}
			}

			// Shere Head
			if ( i == body->joint_index("Head") )
			{
				cml::matrix44d t0 = pose.GetGlobalTransf(i); 	// get the 4x4 transformation matrix in the homogenious form (Translation + Rotation);

				cml::matrix44d t1, t2, t3;
				cml::matrix_translation(t1, 20., 0., 0.);
				cml::matrix_scale(t2, 20., 20., 20.);
				cml::matrix_rotate_about_local_z(t2, 0.);
				g_renderer->SetColor(0.9, 0.55, 0.6);
				g_renderer->SetModelMatrix(t0*t1*t2);
				g_renderer->DrawSphere();

				// 코
				cml::matrix44d e1, e2, e3;
				cml::matrix_translation(e1, 20., 0., -20.);
				cml::matrix_scale(e2, 1.5, 1.5, 1.5);
				cml::matrix_rotate_about_local_x(e2, 33.);
				g_renderer->SetColor(0., 0., 0.);
				g_renderer->SetModelMatrix(t0*e1*e2);
				g_renderer->DrawSphere();

				// 왼쪽 눈
				cml::matrix_translation(e1, 25., -8., -18.5);
				cml::matrix_scale(e2, 1.5, 1.5, 1.5);
				cml::matrix_rotate_about_local_x(e2, 33.);
				g_renderer->SetColor(0., 0., 0.);
				g_renderer->SetModelMatrix(t0*e1*e2);
				g_renderer->DrawSphere();

				// 오른쪽 눈
				cml::matrix_translation(e1, 25., 8., -18.5);
				cml::matrix_scale(e2, 1.5, 1.5, 1.5);
				cml::matrix_rotate_about_local_x(e2, 33.);
				g_renderer->SetColor(0., 0., 0.);
				g_renderer->SetModelMatrix(t0*e1*e2);
				g_renderer->DrawSphere();

				// 입
				cml::matrix_translation(e1, 15., 0., -18.5);
				cml::matrix_scale(e2, 1., 5., 1.);
				cml::matrix_rotate_about_local_x(e2, 33.);
				g_renderer->SetColor(0., 0., 0.);
				g_renderer->SetModelMatrix(t0*e1*e2);
				g_renderer->DrawSphere();
			}
		
			// Box Bodies
			else
			{
				g_renderer->SetColor(0.9, 0.55, 0.6);

				cml::matrix44d t = pose.GetGlobalTransf(i); // get the 4x4 transformation matrix in the homogenious form (Translation + Rotation);
				g_renderer->SetModelMatrix(t);
				g_renderer->DrawSphere(3.1);

			}
			if ( i == body->joint_index("RightLeg") || i == body->joint_index("LeftLeg"))
			{
				cml::vector3d p1 = pose.GetGlobalTranslation(i);
				cml::vector3d p2 = pose.GetGlobalTranslation(body->parent(i));

				g_renderer->SetColor(1., 1., 1.);
				g_renderer->SetModelMatrix(m);
				g_renderer->DrawCylinder(p1, p2, 3.);
			}
		}
	}
}
void DrawBack2()
{
	// Motion
	{
		const ml::Body *body = back2_motion.body();
		ml::Posture &pose = back2_motion.posture(g_frame%back2_motion.size());
		g_renderer->SetColor(0.9, 0.85, 0.6);
		
		for ( int i=1; i<(int)body->num_joint(); i++ )
		{
			cml::vector3d p1 = pose.GetGlobalTranslation(i);
			cml::vector3d p2 = pose.GetGlobalTranslation(body->parent(i));

			cml::matrix44d m;
			m.identity();
			m = m;

			g_renderer->SetModelMatrix(m);
			g_renderer->DrawCylinder(p1, p2, 3.);

			if ( i == body->joint_index("Spine") )
			{
				cml::vector3d p1 = pose.GetGlobalTranslation(i);
				cml::vector3d p2 = pose.GetGlobalTranslation(body->parent(i));

				g_renderer->SetModelMatrix(m);
				g_renderer->DrawCylinder(p1, p2, 3.);

				 // 치마
				 cml::matrix44d e0 = pose.GetGlobalTransf(i);
				 cml::matrix44d skr_s, skr_t, skr_r;
				 cml::matrix_translation(skr_t, -5., 0., 0.);
				 cml::matrix_scale(skr_s, 25., 10., 25.);
				 cml::matrix_rotation_world_z(skr_r, M_PI / 2.0);
				 g_renderer->SetModelMatrix(e0*skr_t*skr_r*skr_s);
				 g_renderer->DrawCylinder();

				 cml::matrix44d skr2_s, skr2_t, skr2_r;
				 cml::matrix_translation(skr2_t, -15., 0., 0.);
				 cml::matrix_scale(skr2_s, 30., 10., 30.);
				 cml::matrix_rotation_world_z(skr2_r, M_PI / 2.0);
				 g_renderer->SetModelMatrix(e0*skr2_t*skr2_r*skr2_s);
				 g_renderer->DrawCylinder();

			}
			if ( i == body->joint_index("Spine1"))
			{
				cml::vector3d p1 = pose.GetGlobalTranslation(i);
				cml::vector3d p2 = pose.GetGlobalTranslation(body->parent(i));

				g_renderer->SetModelMatrix(m);
				g_renderer->DrawCylinder(p1, p2, 20.);
			}

			if ( i == body->joint_index("Neck") )
			{
				cml::vector3d p1 = pose.GetGlobalTranslation(i);
				cml::vector3d p2 = pose.GetGlobalTranslation(body->parent(i));

				g_renderer->SetColor(1., 1., 1.);
				g_renderer->SetModelMatrix(m);
				g_renderer->DrawCylinder(p1, p2, 15.);

				cml::matrix44d t0 = pose.GetGlobalTransf(i); 	// get the 4x4 transformation matrix in the homogenious form (Translation + Rotation);
				// 목
				{
					cml::matrix44d e1, e2, e3;
					cml::matrix_translation(e1, 0., 0., 0.);
					cml::matrix_scale(e2, 10., 10., 10.);
					cml::matrix_rotate_about_local_x(e2, 33.);
					g_renderer->SetModelMatrix(t0*e1*e2);
					g_renderer->DrawSphere();
				}
			}

			// Shere Head
			if ( i == body->joint_index("Head") )
			{
				cml::matrix44d t0 = pose.GetGlobalTransf(i); 	// get the 4x4 transformation matrix in the homogenious form (Translation + Rotation);

				cml::matrix44d t1, t2, t3;
				cml::matrix_translation(t1, 20., 0., 0.);
				cml::matrix_scale(t2, 20., 20., 20.);
				cml::matrix_rotate_about_local_z(t2, 0.);
				g_renderer->SetColor(0.9, 0.85, 0.6);
				g_renderer->SetModelMatrix(t0*t1*t2);
				g_renderer->DrawSphere();

				// 코
				cml::matrix44d e1, e2, e3;
				cml::matrix_translation(e1, 20., 0., -20.);
				cml::matrix_scale(e2, 1.5, 1.5, 1.5);
				cml::matrix_rotate_about_local_x(e2, 33.);
				g_renderer->SetColor(0., 0., 0.);
				g_renderer->SetModelMatrix(t0*e1*e2);
				g_renderer->DrawSphere();

				// 왼쪽 눈
				cml::matrix_translation(e1, 25., -8., -18.5);
				cml::matrix_scale(e2, 1.5, 1.5, 1.5);
				cml::matrix_rotate_about_local_x(e2, 33.);
				g_renderer->SetColor(0., 0., 0.);
				g_renderer->SetModelMatrix(t0*e1*e2);
				g_renderer->DrawSphere();

				// 오른쪽 눈
				cml::matrix_translation(e1, 25., 8., -18.5);
				cml::matrix_scale(e2, 1.5, 1.5, 1.5);
				cml::matrix_rotate_about_local_x(e2, 33.);
				g_renderer->SetColor(0., 0., 0.);
				g_renderer->SetModelMatrix(t0*e1*e2);
				g_renderer->DrawSphere();

				// 입
				cml::matrix_translation(e1, 15., 0., -18.5);
				cml::matrix_scale(e2, 1., 5., 1.);
				cml::matrix_rotate_about_local_x(e2, 33.);
				g_renderer->SetColor(0., 0., 0.);
				g_renderer->SetModelMatrix(t0*e1*e2);
				g_renderer->DrawSphere();
			}
		
			// Box Bodies
			else
			{
				g_renderer->SetColor(0.9, 0.85, 0.6);

				cml::matrix44d t = pose.GetGlobalTransf(i); // get the 4x4 transformation matrix in the homogenious form (Translation + Rotation);
				g_renderer->SetModelMatrix(t);
				g_renderer->DrawSphere(3.1);

			}
			if ( i == body->joint_index("RightLeg") || i == body->joint_index("LeftLeg"))
			{
				cml::vector3d p1 = pose.GetGlobalTranslation(i);
				cml::vector3d p2 = pose.GetGlobalTranslation(body->parent(i));

				g_renderer->SetColor(1., 1., 1.);
				g_renderer->SetModelMatrix(m);
				g_renderer->DrawCylinder(p1, p2, 3.);
			}
		}
	}
}




void Keyboard(unsigned char key, int x, int y )
{
	switch (key)						
	{
	case 27 : exit(0); break;			
	case 'r':
		InitCloth();
		InitSphere();
		std::cout << "key: " << key << std::endl;
		break;

	case 'm':
		g_flag_moving_sphere = !g_flag_moving_sphere;
		std::cout << "key: " << key << std::endl;
		break;

	case 'c':
		g_cloth_display = !g_cloth_display;
		std::cout << "key: " << key << std::endl;
		break;


	default : 
		std::cout << "key: " << key << std::endl;
		break;
	}

	glutPostRedisplay();
	return;
}