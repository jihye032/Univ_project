#include "ClothAni.h"
#include "ClothSimulation.h"


#include "BaseLib/GL4U/GL_Renderer.h"
#include "BaseLib/GL4U/GL_RenderableObj.h"
#include "BaseLib/GL4U/GL_ResourceManager.h"
#include "GL/freeglut.h"

extern float g_FPS;
extern float g_elapsed_time ;
extern mg::GL_Renderer *g_renderer;
void DrawCar();



int g_frame = 0;
static bool g_cloth_display = true;
extern bool g_flag_moving_sphere;

void InitAnimation()
{
	g_FPS = 30.;

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



void DrawAnimation()
{
	// Ground Grid
	g_renderer->Draw("ground");

	// Draw Particles
	{
		g_renderer->SetColor(0, 0, 0);
		glPointSize(5);
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		g_renderer->Draw("cloth");
		
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}


	// Draw cloth
	if ( g_cloth_display  )
	{
		g_renderer->SetColor(0.9, 0.75, 0.75);
		g_renderer->Draw("cloth");
	}


	// Draw Sphere
	g_renderer->SetColor(1, 1, 1);
	g_renderer->DrawSphere(g_sphere.radius_, g_sphere.pos_);
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

	return;
}


