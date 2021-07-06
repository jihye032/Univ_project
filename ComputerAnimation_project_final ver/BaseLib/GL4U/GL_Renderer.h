
#pragma once
#include "BaseLib/CmlExt/CmlExt.h"
#include "GL/glew.h"
#include <map>
#include <string>
#include <stack>



namespace mg
{

class GL_RenderableObj;
class GL_ShaderProgram;
class GL_Material;
class GL_Texture;
class GL_VAO;
class PrimitiveShape;
class PrimitiveComposition;


/**
This class includes algorithms and unitility function to render a instance of GL_RenderableObj.
The rendering processes are (1)reading the rendering information in the GL_RenderableObj instance,
(2)setting default_shader_ (or shader_) parameters and (3)calling profer glew functions.

And it provides unitility functions that makes easier to set and manage the shader parameters 
and glew paramemters. (e.g. managing matrix stack - Push/PopModelViewMatrix)
*/

class GL_Renderer
{
public:
	GL_Renderer();
	GL_Renderer(GL_ShaderProgram* shader);
	virtual ~GL_Renderer();
	void Clear();

	// void CreateAndAddRenderableObj(const std::string &name, GL_VBOGroup *vbo, GL_Material *mat=0);
	// void DeleteRenderableObj(const std::string &name);
	
	void Draw(const std::string &renderable_obj_name);
	void Draw(const GL_RenderableObj *obj);
	void DrawSphere();
	void DrawSphere(double radius, cml::vector3d &center_p=cml::vector3d(0, 0, 0));
	void DrawCapsule();
	void DrawCapsule(double cylinder_len, double radius);
	void DrawHemisphere();
	void DrawHemisphere(double radius, cml::vector3d &center_p=cml::vector3d(0, 0, 0));
	void DrawHead();
	void DrawCylinder();
	void DrawCylinder(double cylinder_len, double radius);
	void DrawCylinder(cml::vector3d &bottom_p, cml::vector3d &top_p, double radius=1.0);
	void DrawOpenedCylinder();
	void DrawBox(double width=1.0, double height=1.0, double depth=1.0);
	void Draw(const PrimitiveShape* p);
	void Draw(const PrimitiveComposition* p);

	void ApplyMaterial(const GL_Material *m=nullptr);
	void DrawVAO(const GL_VAO *vao);

	// Default Material Color
	void SetColor(int i, float a=1.0);
	// Default Material Color
	void SetColor(float r, float g, float b, float a=1.0);
	// Default Material Color
	int  GetRecentlyUsedColorIndex();
	// Default Material Alpha
	void SetAlpha(float a);
	// Default Material Tramsparency
	void EnableTransparency(bool t);

	void SetProjectionMatrix(const cml::matrix44d_c &m);
	void SetViewMatrix(const cml::matrix44d_c &m);
	void SetModelMatrix(const cml::matrix44d_c &m);
	void SetModelviewMatrix(const cml::matrix44d_c &m);
	void PushModelviewMatrix();
	void MultiModelviewMatrix(const cml::matrix44d_c &m);
	void PopModelviewMatrix();

	//void SetBoneMatrix(int bone_id, cml::matrix44d_c m) { shader_->SetBoneMatrix(bone_id, ((cml::matrix44f_c)m).data()); }

	GL_ShaderProgram* shader() const { return shader_; }


protected:
	void CreateDefaultMateiral();

protected:
	GL_ShaderProgram* default_shader_;
	GL_ShaderProgram* shader_;

	cml::matrix44d_c view_mat_;
	cml::matrix44d_c model_mat_;
	cml::matrix44d_c modelview_mat_;
	std::stack<cml::matrix44d_c> modelview_mat_stack_;


	GL_Material *default_material_;
};	


mg::GL_Renderer* GetDefaultRenderer();
};

