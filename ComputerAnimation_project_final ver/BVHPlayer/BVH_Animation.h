#pragma once

#include "ClothAni.h"
#include "ClothSimulation.h"

#include "BaseLib/GL4U/GL_Renderer.h"
#include "BaseLib/GL4U/GL_RenderableObj.h"
#include "BaseLib/GL4U/GL_ResourceManager.h"

#include "GL/freeglut.h"
#include "motion/ml.h"
#include "cml/cml.h"

//
void InitAniation();
void DrawAnimation();
void Timer(int value);
void Keyboard(unsigned char key, int x, int y );
void ClearAnimation();

void DrawSphere();
void Drawback1();
void Drawback2();

extern cml::vector3d g_cape_pos;
extern cml::vector3d g_cape_pos2;
extern cml::vector3d g_spine_pos;