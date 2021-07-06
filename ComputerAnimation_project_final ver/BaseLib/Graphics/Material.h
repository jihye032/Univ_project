
#pragma once
#include <vector>
#include "BaseLib/CmlExt/CmlExt.h"

namespace mg
{

class Texture
{
public:
	enum Format {
		GL_STENCIL_INDEX= 0x1901	   ,
		GL_DEPTH_COMPONENT= 0x1902	   ,
		GL_RED= 0x1903				   ,
		GL_ALPHA= 0x1906			   ,
		GL_RGB= 0x1907				   ,
		GL_RGBA= 0x1908
	};		// Same as the openGL(glew) foramt values.

	enum Internal_Format {
		GL_ALPHA8= 0x803C					  ,
		GL_LUMINANCE8= 0x8040				  ,
		GL_LUMINANCE8_ALPHA8= 0x8045		  ,
		GL_INTENSITY= 0x8049				  ,
		GL_INTENSITY8= 0x804B				  ,
		GL_RGB8= 0x8051						  ,
		GL_RGBA8= 0x8058					  ,
	};		// Same as the openGL(glew) foramt values.

	Texture();
	~Texture();

	inline int GetElementBytes() const
	{ 
		switch (i_format_)
		{ 
		case GL_ALPHA8:
		case GL_LUMINANCE8:
		case GL_INTENSITY:
		case GL_INTENSITY8: 
			return 1; 
		case GL_LUMINANCE8_ALPHA8: 
			return 2; 
		case GL_RGB8: 
			return 3; 
		case GL_RGBA8: 
			return 4; 
		default: 
			return 0; 
		} 
	}

	inline int GetTotalByte() const 
	{
		return dim_ * GetElementBytes();
	}

	int dim_;	// 1, 2, or 3 dimension
	Format format_;
	Internal_Format i_format_;
	void* data_;
};

class PhongMaterial
{
public:
	PhongMaterial();
	~PhongMaterial();

	inline void diffuse_color(double r, double g, double b, double a) {diffuse_color_.set(r, g, b, a);}
	inline void specular_color(double r, double g, double b, double a) {specular_color_.set(r, g, b, a);}
	inline void ambient_color(double r, double g, double b, double a) {ambient_color_.set(r, g, b, a);}
	inline void emission_color(double r, double g, double b, double a) {emission_color_.set(r, g, b, a);}
	inline void shininess(double s) {shininess_=s;}

protected:
	cml::vector4d diffuse_color_;
	cml::vector4d specular_color_;
	cml::vector4d ambient_color_;
	cml::vector4d emission_color_;
	double shininess_;

};



};