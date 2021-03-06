
#pragma once

#include "GL/glew.h"

namespace mg
{

class GL_Texture
{
	friend class GL_ResourceManager;
private:
	GL_Texture();	

public:
	~GL_Texture();

	/**
	It calls GenAndBind with GL_TEXTURE_2D for target parameter.
	*/
	void Gen2DAndBind(int width, int height, GLint internal_format=GL_RGB8);

	/**
	It calls glGenTextures, glBindTexture, and glTexStorage1D, 2D or 3D.
	@param target GL_TEXTURE_1D, GL_TEXTURE_2D ..
	*/
	void GenAndBind(GLenum target,GLint internal_format,
				GLsizei width, GLsizei height, GLsizei depth,
				GLint border);
	void Bind() const;

	bool IsBound() const;
	bool IsBound(GLuint &out_current_bound) const;


	void SetData(void* data, GLenum format=GL_RGB, GLenum type=GL_UNSIGNED_BYTE);

	GLenum GetTarget() const { return target_; }
	GLint GetInternalFormal(GLint level=0) const;
	GLsizei GetWidth(GLint level=0) const;
	GLsizei GetHeight(GLint level=0) const;
	GLsizei GetDepth(GLint level=0) const;

	/**
	@param active_texture: GL_TEXTURE0, GL_TEXTURE1 .... Default valud is GL_TEXTURE0
	*/
	void gl_active_texture_id(GLenum active_texture) { gl_active_texture_id_ = active_texture; }

protected:
	GLuint id_;
	GLenum target_;
	GLint levels_;
	GLenum gl_active_texture_id_;
};



};





