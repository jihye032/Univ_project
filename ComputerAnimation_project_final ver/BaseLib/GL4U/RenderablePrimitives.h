
#pragma once
#include "BaseLib/GL4U/RenderableObjGL.h"

namespace mg
{

class RenderablePrimitives 
{
public:
	enum { SPHERE, BOX, CYLINDER, CAPSULE };

	RenderablePrimitives(ShaderProgramGLSL *s);
	virtual ~RenderablePrimitives();

	void DrawSphere();
	void DrawBox();
	void DrawCylinder();
	void DrawCapsule();

protected:
	virtual void InitPrimitives() = 0;

	RenderableObjGL *sphere_;
	RenderableObjGL *box_;
	RenderableObjGL *cylinder_;
	RenderableObjGL *capsule_;
};	


};