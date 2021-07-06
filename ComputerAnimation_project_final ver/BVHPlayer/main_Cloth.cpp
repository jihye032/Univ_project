
#include <iostream>
#include "GL/glew.h"
#include "GL/freeglut.h"
#include "BaseLib/CmlExt/CmlCamera.h"
#include "BaseLib/GL4U/GL_Renderer.h"
#include "ClothAni.h"


// Camera 
static cml::Camera g_camera;


// Window and User Interface
static bool g_left_button_pushed;
static bool g_right_button_pushed;
static int g_last_mouse_x;
static int g_last_mouse_y;
static int g_window_w;
static int g_window_h;


void InitWindow();
void Reshape( int w, int h );


// Keyboard and Mouse
void Keyboard(unsigned char key, int x, int y );
void SpeicalKeyboard(int key, int x, int y );
void SpeicalKeyboardUp(int key, int x, int y );
void Mouse(int button, int state, int x, int y);
void MouseMotion(int x, int y);





// OpenGL
mg::GL_Renderer *g_renderer = 0;
void InitOpenGL();
void Display();


// Shader Program ID
int g_shader_id;


// Animtion Timer
float g_FPS = 60.f;
float g_elapsed_time = 0.0f;  // sec
void Timer(int value);



// Animation
void InitAnimation();
void UpdateAnimation();
void DrawAnimation();


int main( int argc, char** argv )
{
	glutInit( &argc, argv );

	

	InitWindow();
	
	glewExperimental = TRUE;
	if (  glewInit() != GLEW_OK )
	{
		std::cerr << "Unable to initialize GLEW ... exiting" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
	{
		std::cout << "GLEW OK\n";
	}

	std::cout << "r: Reset" << std::endl;
	std::cout << "m: Move Sphere Toggle" << std::endl;
	std::cout << "c: Display Cloth Toggle" << std::endl;

	InitOpenGL();

	

	
	glutReshapeFunc( Reshape );
	glutDisplayFunc( Display );
	glutKeyboardFunc( Keyboard );
	glutSpecialFunc( SpeicalKeyboard );
	glutSpecialUpFunc( SpeicalKeyboardUp );
	glutMouseFunc( Mouse );
	glutMotionFunc( MouseMotion );
	glutTimerFunc((unsigned int)(1000.f/g_FPS), Timer, 0);
	
	glutMainLoop();

	
	return 0;
}



void InitOpenGL()
{
	g_renderer = new mg::GL_Renderer;
	
	glClearColor(1, 1, 1, 1);
	glClearDepth(1.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	//glEnable(GL_CULL_FACE);

	// Initial State of Camera
	g_camera.setFov(45.0f);
	g_camera.setNearFar(0.1, 10000);
	g_camera.setRotation(cml::quaterniond(cos(M_PI/2), 0, sin(M_PI/2), 0));
	g_camera.setTranslation(cml::vector3d(0, 110, 590));

	InitAnimation();
}

void Timer(int value)
{
	g_elapsed_time += 1.0f/g_FPS;

	
	UpdateAnimation();

	glutPostRedisplay();
	glutTimerFunc((unsigned int)(1000.f/g_FPS), Timer, 0);
}





void Display()
{
	if ( g_renderer==0 ) return;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	cml::matrix44d projection_m = g_camera.GetGLProjectionMatrix();  // Mouse-Controlling Camera Projection
	cml::matrix44d view_m  = g_camera.GetGLViewMatrix();        // Mouse-Controlling Camera View Matrix
	
	g_renderer->SetProjectionMatrix(projection_m);
	g_renderer->SetViewMatrix(view_m);
	
	
	DrawAnimation();
	

	glutSwapBuffers();
}





/////////////////////////////////////////////////////////////////////////////////////////
/// Window

void InitWindow()
{
	// specifies display mode
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	// specifies the size, in pixels, of your window
	glutInitWindowSize(1080 ,720);

	glutInitContextVersion( 3, 2 );
    glutInitContextProfile( GLUT_CORE_PROFILE );


	// creates a window with an OpenGL context
	glutCreateWindow( "Computer Graphics" );
}

void Reshape( int w, int h )
{     
	//  w : window width   h : window height
	g_window_w = w;
	g_window_h = h;
	g_left_button_pushed = false;
	g_right_button_pushed = false;

	
	//  w : window width   h : window height
	glViewport(0,0,(GLsizei)w,(GLsizei)h);
	g_camera.setAspectRatio((double)w/(double)h);
	
} 









/////////////////////////////////////////////////////////////////////////////////////////
/// Keyboard and Mouse Input

void Mouse(int button, int state, int x, int y)
{
	double mouse_xd = (double)x / g_window_w;
	double mouse_yd = 1 - (double)y / g_window_h;
	double last_mouse_xd = (double)g_last_mouse_x / g_window_w;
	double last_mouse_yd = 1 - (double)g_last_mouse_y / g_window_h;


	if ( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		g_left_button_pushed = true;

	else if ( button == GLUT_LEFT_BUTTON && state == GLUT_UP)
		g_left_button_pushed = false;

	else if ( button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
		g_right_button_pushed = true;

	else if ( button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
		g_right_button_pushed = false;
	else if ( button == 3 )
	{
		g_camera.inputMouse(cml::Camera::IN_ZOOM, 0, 1);
		glutPostRedisplay();
	}
	else if ( button == 4 )
	{
		g_camera.inputMouse(cml::Camera::IN_ZOOM, 0, -1);
		glutPostRedisplay();
	}

	g_last_mouse_x = x;
	g_last_mouse_y = y;
}

void MouseMotion(int x, int y)
{
	double mouse_xd = (double)x / g_window_w;
	double mouse_yd = 1 - (double)y / g_window_h;
	double last_mouse_xd = (double)g_last_mouse_x / g_window_w;
	double last_mouse_yd = 1 - (double)g_last_mouse_y / g_window_h;

	if ( g_left_button_pushed && g_right_button_pushed )
	{
		g_camera.inputMouse(cml::Camera::IN_ZOOM, last_mouse_xd, last_mouse_yd, mouse_xd, mouse_yd);
		glutPostRedisplay();
	}
	else if ( g_left_button_pushed )
	{
		g_camera.inputMouse(cml::Camera::IN_ROTATION_Y_UP, last_mouse_xd, last_mouse_yd, mouse_xd, mouse_yd);
		glutPostRedisplay();
	}
	else if ( g_right_button_pushed )
	{
		g_camera.inputMouse(cml::Camera::IN_TRANS, last_mouse_xd, last_mouse_yd, mouse_xd, mouse_yd);
		glutPostRedisplay();
	}

	g_last_mouse_x = x;
	g_last_mouse_y = y;
}





void SpeicalKeyboard(int key, int x, int y )
{
	switch (key)						
	{
	case GLUT_KEY_DOWN:
		std::cout << "down-key pushed" << std::endl;
		break;

	case GLUT_KEY_UP:
		std::cout << "up-key pushed" << std::endl;
		break;

	case GLUT_KEY_LEFT:
		std::cout << "left-key pushed" << std::endl;
		break;

	case GLUT_KEY_RIGHT:
		std::cout << "right-key pushed" << std::endl;
		break;

	}

	glutPostRedisplay();
	return;
}

void SpeicalKeyboardUp(int key, int x, int y )
{
	switch (key)						
	{
	case GLUT_KEY_DOWN:
		std::cout << "down-key released" << std::endl;
		break;

	case GLUT_KEY_UP:
		std::cout << "up-key released" << std::endl;
		break;

	case GLUT_KEY_LEFT:
		std::cout << "left-key released" << std::endl;
		break;

	case GLUT_KEY_RIGHT:
		std::cout << "right-key released" << std::endl;
		break;

	}

	glutPostRedisplay();
	return;
}











