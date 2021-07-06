
#pragma once
#include "BaseLib/CmlExt/CmlExt.h"
#include "GL/glew.h"
#include <string>

namespace mg
{

class GL_Material;
class GL_Texture;

class GL_Light
{
public:
	// 0: Directionl Light
	// 1: Point Light
	// 2: Spot Light
	int type;


	cml::vector3f dir;
	cml::vector3f position;
	cml::vector4f intensity; // I_l
	float cos_cutoff;	// Spot Light
};

class GL_ShaderProgram
{
public:
	enum { V_POSITION_LOC=0, V_COLOR_LOC, V_NORMAL_LOC, V_TEX_COORD_LOC, V_BONE_ID_LOC, V_BONE_WEIGHT_LOC };
	enum { NO_SHADING=0, PHONG_SHADING, FLAT_SHADING };

	GL_ShaderProgram();
	~GL_ShaderProgram();

	void CreateDefaultProgram();
	GLuint CreateFromFiles(const std::string &v_shader_file, const std::string &f_shader_file);
	GLuint CreateFromSource(const std::string &v_shader_src, const std::string &f_shader_src);
	void UseProgram();
	bool IsUsing() const;
	void Delete();

	// Set Default Vertex Attributes
	void SetDefaultVertexColor(double r, double g, double b, double a = 1.0);

	// Set Uniform Values
	void SetShadingMode(int m = PHONG_SHADING);
	
	// Set Uniform Values : Matricies
	void SetProjectionMatrix(const float *m44);
	void SetModelviewMatrix(const float *m44);

	// Set Uniform Values : Mateirals
	void SetMaterial(const GL_Material &m);
	void SetPhongMaterial(const cml::vector4f &k_a, const cml::vector4f &k_d, const cml::vector4f &k_s, const float shinness);
	void EnableTexture(bool f);
	void EnableVertexColor(bool f);
	void SetTexture(const GL_Texture &t);

	// Set Uniform Values : Lighting
	void SetAmbientLightIntensity(float l);
	void EnableLight(int light_id, bool f = true);
	void SetLight(int light_id, const GL_Light &l);
	void SetLight(int light_id, int type, cml::vector3f dir, cml::vector3f position = cml::vector3f(0.f, 0.f, 0.f), cml::vector4f intensity = cml::vector4f(1.f, 1.f, 1.f, 1.f), float cos_cutoff = 0.f);
	void DisableAllLight();

	// Set Uniform Values : Skinning 
	void EnableSkinning(bool f);
	void SetBoneMatrix(int bone_id, float *m44);
	

	// Set Uniform Values
	bool IsSkinningOn();

	inline GLint GetModelviewMatrixLoc();
	inline GLint GetProjectionMatrixLoc();

	inline GLuint program_handle() const { return program_handle_; }

protected:
	bool ReadSourceFromFile(const std::string &in_filename, std::string &out_src);
	GLint GetBoneMatrixLoc(int bone_id);

protected:
	GLuint program_handle_;
	GLint projection_matrix_loc_;
	GLint modelview_matrix_loc_;
	GLint bone_matrix_loc_;

	int num_lights_;
};


};