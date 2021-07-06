

#include "BaseLib/GL4U/GL_ShaderProgram.h"
#include "BaseLib/GL4U/GL_Material.h"
#include "BaseLib/GL4U/GL_Texture.h"
#include <sstream>
#include <fstream>
#include <iostream>

namespace mg
{

GL_ShaderProgram::GL_ShaderProgram()
{
	program_handle_ = 0;
}

GL_ShaderProgram::~GL_ShaderProgram()
{
	Delete();
}

bool
GL_ShaderProgram::ReadSourceFromFile(const std::string &in_filename, std::string &out_src)
{
	out_src.clear();
	std::ifstream fin(in_filename.c_str());

	if ( fin.fail() )
	{
		std::cerr << "Unable to open file '" << in_filename << "'" << std::endl;
		return false;
	}
	else
	{
		while (true)
		{
			char c = char(fin.get());
			if (!fin)
				return fin.eof();
			out_src += c;
		}
	}
}


GLuint 
GL_ShaderProgram::CreateFromFiles(const std::string &v_shader_file, const std::string &f_shader_file)
{
	std::string v_shader_src;
	std::string f_shader_src;

	
	if ( !ReadSourceFromFile(v_shader_file, v_shader_src) )
	{
		return 0;
	}

	if ( !ReadSourceFromFile(f_shader_file, f_shader_src) )
	{
		return 0;
	}

	program_handle_ = CreateFromSource(v_shader_src, f_shader_src);

	return program_handle_;
}

GLuint
GL_ShaderProgram::CreateFromSource(const std::string &v_shader_src, const std::string &f_shader_src)
{
	GLuint program = glCreateProgram();

	// vertex shader
	GLuint v_shader = glCreateShader(GL_VERTEX_SHADER);
	{
		const GLchar *v_src = v_shader_src.c_str();
		glShaderSource(v_shader, 1, &v_src, NULL);
		glCompileShader(v_shader);

		GLint compiled;
		glGetShaderiv(v_shader, GL_COMPILE_STATUS, &compiled);

		if (!compiled) 
		{
			GLsizei len;
			glGetShaderiv( v_shader, GL_INFO_LOG_LENGTH, &len );

			GLchar* log = new GLchar[len+1];
			glGetShaderInfoLog( v_shader, len, &len, log );
			std::cerr << "Shader compilation failed: " << log << std::endl;
			delete [] log;
			return 0;
		}
		glAttachShader(program, v_shader);
	}

	// fragment shader
	GLuint f_shader = glCreateShader(GL_FRAGMENT_SHADER);
	{
		const GLchar *f_src = f_shader_src.c_str();
		glShaderSource(f_shader, 1, &f_src, NULL);
		glCompileShader(f_shader);

		GLint compiled;
		glGetShaderiv(f_shader, GL_COMPILE_STATUS, &compiled);

		if (!compiled) 
		{
			GLsizei len;
			glGetShaderiv( f_shader, GL_INFO_LOG_LENGTH, &len );

			GLchar* log = new GLchar[len+1];
			glGetShaderInfoLog( f_shader, len, &len, log );
			std::cerr << "Shader compilation failed: " << log << std::endl;
			delete [] log;
			return 0;
		}
		glAttachShader(program, f_shader);
	}


	glLinkProgram(program);

	GLint linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (!linked) 
	{
		GLsizei len;
		glGetProgramiv( program, GL_INFO_LOG_LENGTH, &len );

		GLchar* log = new GLchar[len+1];
		glGetProgramInfoLog( program, len, &len, log );
		std::cerr << "Shader linking failed: " << log << std::endl;
		delete [] log;

		glDeleteShader(v_shader);
		glDeleteShader(f_shader);
		glDeleteProgram(program);

		return 0;
	}

	return program;
}

void
GL_ShaderProgram::CreateDefaultProgram()
{
	std::string v_shader;
	// v_shader =	R"vV(
	v_shader = "	\
	#version 330\n																			   \
																							   \
	layout (location=0) in vec4 vs_position;												   \
	layout (location=1) in vec4 vs_color;													   \
	layout (location=2) in vec3 vs_normal;													   \
	layout (location=3) in vec2 vs_uv;														   \
	layout (location=4) in uvec4 vs_bone_id;												   \
	layout (location=5) in vec4 vs_bone_weight;												   \
																							   \
	uniform mat4 projection_matrix;															   \
	uniform mat4 modelview_matrix;															   \
																							   \
	uniform bool flag_skinning;																   \
	uniform mat4 bone_matrices[100];														   \
																							   \
	out vec4 fs_color;																		   \
	out vec2 fs_uv;																			   \
	out vec3 fs_modelviewed_normal;															   \
	out vec3 fs_modelviewed_pos;															   \
	out vec3 fs_eye_dir;																	   \
																							   \
	void main(void)																			   \
	{																						   \
		fs_color = vs_color;																   \
		fs_uv = vs_uv;																		   \
																							   \
		mat4 modelviewbone_matrix;															   \
		if ( !flag_skinning ) 																   \
		{																					   \
			modelviewbone_matrix = modelview_matrix;										   \
		}																					   \
		else																				   \
		{																					   \
			modelviewbone_matrix = modelview_matrix *										   \
						(																	   \
						  vs_bone_weight[0] * bone_matrices[vs_bone_id[0]] 					   \
						+ vs_bone_weight[1] * bone_matrices[vs_bone_id[1]] 					   \
						+ vs_bone_weight[2] * bone_matrices[vs_bone_id[2]]					   \
						+ vs_bone_weight[3] * bone_matrices[vs_bone_id[3]] 					   \
						);																	   \
		}																					   \
																							   \
		gl_Position = projection_matrix * (modelviewbone_matrix * vs_position);				   \
		fs_modelviewed_normal = normalize(modelviewbone_matrix*vec4(vs_normal, 0.0)).xyz;	   \
																							   \
		fs_modelviewed_pos = (modelviewbone_matrix * vs_position).xyz;						   \
		fs_eye_dir = (modelview_matrix * vs_position).xyz;									   \
	}";
	//)vV";

	std::string f_shader;

	//f_shader = R"fF( 
	f_shader = "	\
	#version 330\n																								\
																												\
	in vec4 fs_color;																							\
	in vec3 fs_normal;																							\
	in vec2 fs_uv;																								\
	in vec3 fs_modelviewed_pos;																					\
	in vec3 fs_modelviewed_normal;																				\
	in vec3 fs_eye_dir;																							\
																												\
	layout (location = 0) out vec4 color;																		\
																												\
	struct Light																								\
	{																											\
		// 0: Directionl Light\n																					\
		// 1: Point Light\n																						\
		// 2: Spot Light\n																						\
		int type;																								\
																												\
																												\
		vec3 dir;																								\
		vec3 position;																							\
		vec4 intensity; // I_l\n																					\
		float cos_cutoff;																						\
	};																											\
																												\
	uniform int num_lights = 10;																				\
	uniform Light lights[20];																					\
	uniform bool flag_lights[20];																				\
																												\
	uniform float I_a = 0.05f;				 										\
	uniform vec4 K_a =  vec4(0.5f, 0.5f, 0.5f, 1.f);															\
	uniform vec4 K_d =  vec4(0.5f, 0.5f, 0.5f, 1.f);															\
	uniform vec4 K_s =  vec4(0.5f, 0.5f, 0.5f, 1.f);															\
																												\
	uniform float shininess_n = 100.f;																			\
	uniform int shading_mode = 0;	// 1: Phong Shading, 2: Flat Shading, 0: No Shading\n							\
																												\
	uniform bool flag_use_vertex_color = true;																	\
	uniform bool flag_texture = false;																			\
	uniform sampler2D tex0;																						\
																												\
	void main()																									\
	{																											\
		vec3 K_a_, K_d_, K_s_;																					\
		float a_ = (K_a.a+K_d.a+K_s.a)/3.f;																		\
																												\
		if ( flag_texture && flag_use_vertex_color )															\
		{																										\
			K_d_ = min(texture(tex0, fs_uv).rgb * fs_color.rgb, 1.0);											\
			K_a_ = K_d_;																						\
			a_ = a_ * fs_color.a;																				\
		}																										\
		else if ( flag_texture )																				\
		{																										\
			K_d_ = min(texture(tex0, fs_uv).rgb, 1.0);															\
			K_a_ = K_d_;																						\
		}																										\
		else if ( flag_use_vertex_color )																		\
		{																										\
			K_d_ = fs_color.rgb;																				\
			K_a_ = K_d_;																						\
		}																										\
		else																									\
		{																										\
			K_a_ = K_a.rgb;																						\
			K_d_ = K_d.rgb;																						\
		}																										\
																												\
																												\
		if ( shading_mode == 0 )																				\
		{																										\
			color = fs_color;																					\
		}																										\
		else if ( shading_mode == 1 || shading_mode == 2 )														\
		{																										\
			vec3 L, H, V, N;																					\
																												\
			vec3 ambient = I_a * K_a_;																			\
			vec3 diffuse = vec3(0.f);																			\
			vec3 specular = vec3(0.f);																			\
																												\
																												\
			V = normalize(-fs_eye_dir);																			\
																												\
			if ( shading_mode == 1 ) 																			\
			{																									\
				N = normalize(fs_modelviewed_normal);															\
			}																									\
			else																								\
			{																									\
				vec3 fdx = vec3(dFdx(fs_modelviewed_pos));														\
				vec3 fdy = vec3(dFdy(fs_modelviewed_pos)); 														\
				N = normalize(cross(fdx,fdy));																	\
			}																									\
																												\
			bool no_light = true;																				\
			for ( int i=0; i<num_lights; i++ )																	\
			{																									\
				if ( !flag_lights[i] ) continue;																\
				no_light = false;																				\
																												\
				if ( lights[i].type == 0 )																		\
				{																								\
					L = normalize(-lights[i].dir);																\
					H = (L+V)/length(L+V);																		\
																												\
					vec4 I_l = lights[i].intensity;																\
																												\
					diffuse  += I_l.a * I_l.rgb * K_d_ * max(0.f, dot(L, N));									\
					specular += I_l.a * I_l.rgb * K_s_ * pow(max(0.f, dot(N, H)), shininess_n);					\
				}																								\
																												\
				else if ( lights[i].type == 1 )																	\
				{																								\
					L = normalize(lights[i].position-fs_eye_dir);												\
					H = (L+V)/length(L+V);																		\
					float d =  length(lights[i].position-fs_eye_dir);											\
																												\
					vec4 I_l = lights[i].intensity;																\
					if ( d > 0.001f )																			\
						I_l = lights[i].intensity * min(1.0f/(0.001*d*d), 1.0f);								\
																												\
					diffuse  += I_l.a * I_l.rgb * K_d_ * max(0.f, dot(L, N));									\
					specular += I_l.a * I_l.rgb * K_s_ * pow(max(0.f, dot(N, H)), shininess_n);					\
				}																								\
																												\
				else if ( lights[i].type == 2 )																	\
				{																								\
					L = normalize(lights[i].position-fs_eye_dir);												\
					H = (L+V)/length(L+V);																		\
					vec3 Sd = normalize(-lights[i].dir);														\
																												\
					vec4 I_l = lights[i].intensity;																\
																												\
					if ( dot(Sd,L) >= lights[i].cos_cutoff )													\
					{																							\
						diffuse  += I_l.a * I_l.rgb * K_d_ * max(0.f, dot(L, N));								\
						specular += I_l.a * I_l.rgb * K_s_ * pow(max(0.f, dot(N, H)), shininess_n);				\
					}																							\
				}																								\
																												\
																												\
			}																									\
																												\
			if ( no_light )																						\
			{																									\
				color = K_d;																					\
			}																									\
			else																								\
			{																									\
				color.rgb = min(ambient + diffuse + specular, 1.0);												\
				color.a = a_;																					\
			}																									\
		}																										\
																												\
																												\
																												\
																												\
	}";
	//)fF";

	program_handle_ = CreateFromSource(v_shader, f_shader);
}

////////////////////////////////////////////////////////////
// Set Default Vertex Attributes

void 
GL_ShaderProgram::SetDefaultVertexColor(double r, double g, double b, double a)
{
	if ( !IsUsing() ) UseProgram();
	glVertexAttrib4d(V_COLOR_LOC, r, g, b, a);
}






////////////////////////////////////////////////////////////
// Set Unifrom Values

void
GL_ShaderProgram::SetShadingMode(int m)
{
	if (!IsUsing()) UseProgram();
	GLint shading_mode_loc = glGetUniformLocation(program_handle_, "shading_mode");
	if (shading_mode_loc < 0) return;

	glUniform1i(shading_mode_loc, m);
}


////////////////////////////////////////////////////////////
// Set Unifrom Values : Matrices

void
GL_ShaderProgram::SetProjectionMatrix(const float *m44)
{
	if (!IsUsing()) UseProgram();
	glUniformMatrix4fv(projection_matrix_loc_, 1, GL_FALSE, m44);
}

void
GL_ShaderProgram::SetModelviewMatrix(const float *m44)
{
	if (!IsUsing()) UseProgram();
	glUniformMatrix4fv(modelview_matrix_loc_, 1, GL_FALSE, m44);
}


////////////////////////////////////////////////////////////
// Set Unifrom Values : Materials

void
GL_ShaderProgram::SetMaterial(const GL_Material &m)
{
	if (!IsUsing()) UseProgram();

	SetPhongMaterial(m.ambient_color_f(), m.diffuse_color_f(), m.specular_color_f(), m.shininess_f());

	EnableVertexColor(m.flag_use_vertex_color());
	EnableTexture(m.flag_texture());

	if ( !m.flag_shading() )
	{
		SetShadingMode(NO_SHADING);
	}
	else if ( m.flag_flat_shading() )
	{
		SetShadingMode(FLAT_SHADING);
	}
	else
	{
		SetShadingMode(PHONG_SHADING);
	}

	if ( m.flag_texture() )
	{
		for ( int i=0; i<m.CountTextures(); i++ )
		{
			SetTexture( *(m.texture(i)) );
		}
	}
}

void
GL_ShaderProgram::SetPhongMaterial(const cml::vector4f &k_a, const cml::vector4f &k_d, const cml::vector4f &k_s, const float shinness)
{
	if (!IsUsing()) UseProgram();

	glUniform4fv(glGetUniformLocation(program_handle_, "K_a"), 1, k_a.data());
	glUniform4fv(glGetUniformLocation(program_handle_, "K_d"), 1, k_d.data());
	glUniform4fv(glGetUniformLocation(program_handle_, "K_s"), 1, k_s.data());
	glUniform1f(glGetUniformLocation(program_handle_, "shininess_n"), shinness);
}

void
GL_ShaderProgram::SetTexture(const GL_Texture &t)
{
	t.Bind();
}

void
GL_ShaderProgram::EnableVertexColor(bool f)
{
	if (!IsUsing()) UseProgram();

	glUniform1i(glGetUniformLocation(program_handle_, "flag_use_vertex_color"), (int)f);
}

void
GL_ShaderProgram::EnableTexture(bool f)
{
	if (!IsUsing()) UseProgram();
	glUniform1i(glGetUniformLocation(program_handle_, "flag_texture"), (int)f);
}



////////////////////////////////////////////////////////////
// Set Unifrom Values : Lighting

void 
GL_ShaderProgram::SetAmbientLightIntensity(float l)
{
	if (!IsUsing()) UseProgram();

	glUniform1f(glGetUniformLocation(program_handle_, "I_a"), (int)l);
}

void
GL_ShaderProgram::EnableLight(int light_i, bool f)
{
	if (!IsUsing()) UseProgram();

	//std::string fs_val_name = "flag_lights[" + std::to_string(light_i) + "]";
	//glUniform1i(glGetUniformLocation(program_handle_, fs_val_name.c_str()), (int)f);
	
	char tmp_str[256];
	sprintf(tmp_str, "flag_lights[%d]", light_i);
	glUniform1i(glGetUniformLocation(program_handle_, tmp_str), (int)f);

}

void
GL_ShaderProgram::SetLight(int light_id, const GL_Light &l)
{
	SetLight(light_id, l.type, l.dir, l.position, l.intensity, l.cos_cutoff);
}

void
GL_ShaderProgram::SetLight(int light_id, int type, cml::vector3f dir, cml::vector3f position, cml::vector4f intensity, float cos_cutoff)
{
	if (!IsUsing()) UseProgram();

	/*glUniform1i(glGetUniformLocation(program_handle_, ("lights[" + std::to_string(light_id) + "].type").c_str()), type);
	glUniform3fv(glGetUniformLocation(program_handle_, ("lights[" + std::to_string(light_id) + "].dir").c_str()), 1, dir.data());
	glUniform3fv(glGetUniformLocation(program_handle_, ("lights[" + std::to_string(light_id) + "].position").c_str()), 1, position.data());
	glUniform4fv(glGetUniformLocation(program_handle_, ("lights[" + std::to_string(light_id) + "].intensity").c_str()), 1, intensity.data());
	glUniform1f(glGetUniformLocation(program_handle_, ("lights[" + std::to_string(light_id) + "].cos_cutoff").c_str()), cos_cutoff);*/

	char tmp_str[256];
	sprintf(tmp_str, "lights[%d].type", light_id);
	glUniform1i(glGetUniformLocation(program_handle_, tmp_str), type);
	sprintf(tmp_str, "lights[%d].dir", light_id);
	glUniform3fv(glGetUniformLocation(program_handle_, tmp_str), 1, dir.data());
	sprintf(tmp_str, "lights[%d].position", light_id);
	glUniform3fv(glGetUniformLocation(program_handle_, tmp_str), 1, position.data());
	sprintf(tmp_str, "lights[%d].intensity", light_id);
	glUniform4fv(glGetUniformLocation(program_handle_, tmp_str), 1, intensity.data());
	sprintf(tmp_str, "lights[%d].cos_cutoff", light_id);
	glUniform1f(glGetUniformLocation(program_handle_, tmp_str), cos_cutoff);
}

void
GL_ShaderProgram::DisableAllLight()
{
	for (int i = 0; i<num_lights_; i++)
	{
		EnableLight(i, false);
	}
}




////////////////////////////////////////////////////////////
// Set Unifrom Values : Skinning


void
GL_ShaderProgram::EnableSkinning(bool f)
{
	if ( !IsUsing() ) UseProgram();

	glUniform1i( glGetUniformLocation(program_handle_, "flag_skinning"), (int)f );
}





bool
GL_ShaderProgram::IsSkinningOn()
{
	if (!IsUsing()) UseProgram();

	int result;
	glGetUniformiv(program_handle_, glGetUniformLocation(program_handle_, "flag_skinning"), &result);

	return (bool)result;
}

void
GL_ShaderProgram::UseProgram()
{
	if (program_handle_ != 0)
	{
		glUseProgram(program_handle_);


		// Get Unifrom Locations
		projection_matrix_loc_ = glGetUniformLocation(program_handle_, "projection_matrix");
		modelview_matrix_loc_ = glGetUniformLocation(program_handle_, "modelview_matrix");
		bone_matrix_loc_ = glGetUniformLocation(program_handle_, "bone_matrices");



		// Set Default Values : Vertex Atributes
		glVertexAttrib4d(V_POSITION_LOC, 0.0, 0.0, 0.0, 1.0);
		glVertexAttrib3d(V_NORMAL_LOC, 0.0, 0.0, 0.0);
		glVertexAttrib2d(V_TEX_COORD_LOC, 0.0, 0.0);
		glVertexAttrib4d(V_COLOR_LOC, 1.0, 1.0, 1.0, 1.0);
		glVertexAttribI4ui(V_BONE_ID_LOC, 0, 0, 0, 0);
		glVertexAttrib4d(V_BONE_WEIGHT_LOC, 0.0, 0.0, 0.0, 0.0);

		glUniform1i(glGetUniformLocation(program_handle_, "tex0"), 0);


		// Set Default Values : Skinning
		{
			EnableSkinning(false);

			float* bone_matrices = new float[100 * 4 * 4];
			for (int i = 0; i < 100; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					for (int k = 0; k < 4; k++)
					{
						if (j == k)
							bone_matrices[i * 4 * 4 + j * 4 + k] = 1.0f;
						else
							bone_matrices[i * 4 * 4 + j * 4 + k] = 0.0f;
					}
				}
			}

			glUniformMatrix4fv(bone_matrix_loc_, 100, GL_FALSE, bone_matrices);
			delete[] bone_matrices;
		}

		// Set Default Values : Lighting
		{
			num_lights_ = glGetUniformLocation(program_handle_, "num_lights");;
			DisableAllLight();

			for ( int i=0; i<num_lights_; i++ )
			{
				SetLight(i, 0, cml::vector3f(0.f, 0.f, -1.f));
			}

			EnableLight(0);
		}
	}
}

GLint
GL_ShaderProgram::GetModelviewMatrixLoc()
{
	return glGetUniformLocation(program_handle_, "modelview_matrix");
}

GLint
GL_ShaderProgram::GetProjectionMatrixLoc()
{
	return glGetUniformLocation(program_handle_, "projection_matrix");
}

GLint 
GL_ShaderProgram::GetBoneMatrixLoc(int bone_id) 
{
	if ( !IsUsing() ) UseProgram();
	return bone_matrix_loc_+bone_id;

	std::stringstream sstr;
	sstr << "bone_matrices[" << bone_id << "]";
	std::string name;
	name = sstr.str();
	return glGetUniformLocation(program_handle_, name.c_str());
}

void 
GL_ShaderProgram::SetBoneMatrix(int bone_id, float *m44)
{
	if ( !IsUsing() ) UseProgram();
	glUniformMatrix4fv(GetBoneMatrixLoc(bone_id), 1, GL_FALSE, m44);
}

bool
GL_ShaderProgram::IsUsing() const
{
	GLint cur_program_handle_;
	glGetIntegerv(GL_CURRENT_PROGRAM, &cur_program_handle_);

	if ( cur_program_handle_ == (GLint)program_handle_ ) return true;

	return false;
}


void
GL_ShaderProgram::Delete()
{
	if ( program_handle_ != 0 )
	{
		if ( IsUsing() )
			glUseProgram(0);
	
		glDeleteShader(program_handle_);
	}
}


};



