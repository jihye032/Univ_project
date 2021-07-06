
#pragma once
#include "BaseLib/CmlExt/CmlExt.h"
#include "BaseLib/Geometry/Mesh.h"

#include "GL/glew.h"
#include <set>
#include <map>

class PmHuman;

namespace mg
{

class GL_RenderableObj;
class GL_Material;

class GL_VBO
{
public:
	GL_VBO();
	GL_VBO(GLenum data_type, int vec_dim, int num_of_vectors, void *data, GLenum target=GL_ARRAY_BUFFER, GLenum usage=GL_STATIC_DRAW);	
	~GL_VBO();
	
	// vec_dim is 1, 2,3 or 4.
	// total buffer size (*data) must be sizeof(data_type)*vec_dim*num_of_vectors bytes.
	void GenAndBind(GLenum data_type, int vec_dim, int num_of_vectors, void *data, GLenum target=GL_ARRAY_BUFFER, GLenum usage=GL_STATIC_DRAW);	
	void Update(void *data);	
	void Bind();
	void Delete();

	GLuint vbo_id() const { return vbo_id_; }
	GLenum data_type() const { return data_type_; }
	int vec_dim() const { return vec_dim_; }
	int num_of_vectors() const { return num_of_vectors_; }
	GLenum target() const { return target_; }
	GLenum usage() const { return usage_; }

protected:
	GLuint vbo_id_;
	GLenum data_type_;
	int vec_dim_;
	int num_of_vectors_;
	GLenum target_;
	GLenum usage_;
};



class GL_VBOGroup
{
	friend class GL_ResourceManager;
private:
	GL_VBOGroup();
	GL_VBOGroup(int num_vertices);
public:

	enum { VERTEX_VBO=0, NORMAL_VBO, TEX_COORD_VBO, COLOR_VBO, BONE_ID_VBO, BONE_WEIGHT_VBO };

	virtual ~GL_VBOGroup();


	void Init(int num_vertices);
	virtual void Delete();

	///////////////////
	// VBO
	/*
	content_type: VERTEX_VBO, NORMAL_VBO, UV_VBO, or COLOR_VBO 
	data_type: GL_FLOAT, GL_INT ...
	vec_dim: 1, 2, 3 or 4
	data: It' byte size is "sizeof(data_type)*vec_dim*num_vertices()".
	*/
	virtual void SetVBO(int content_type, GLenum data_type, int vec_dim, void *data);
	virtual GL_VBO* GetVBO(int content_type) const { return vbos_[content_type]; }
	virtual void UpdateVBO(int content_type, GLenum data_type, int vec_dim, void *data);


	/*
	It create and set a GL_ELEMENT_ARRAY_BUFFER.
	*/
	virtual void SetElementIndices(int num_indices, unsigned int *data);


	virtual void SetByMesh(const mg::Mesh &mesh);
	/*
	virtual void SetByAssimp(const std::string filename);
	virtual void SetByAssimp(const std::string filename, std::vector< std::pair<std::string, cml::matrix44d_c> > &out_bone_name_and_mat);
	virtual void SetByAssimp(const std::string filename, const std::map<std::string, int> &in_bonename_to_pmjoint);
	virtual void SetByAssimp(const std::string filename, const std::map<std::string, int> &in_bonename_to_pmjoint, std::vector< std::pair<std::string, cml::matrix44d_c> > &out_bone_name_and_mat);
	*/
	// Old style
	void BeginVertex();
	void glNormal(cml::vector3f n);
	void glUV(cml::vector2f uv);
	void glColor(cml::vector4f c);
	void glVertex(cml::vector3f v);
	void EndVertex();

	void BeginUpdate();
	void glUpdateNormal(cml::vector3f n);
	void glUpdateUV(cml::vector2f uv);
	void glUpdateColor(cml::vector4f c);
	void glUpdateVertex(cml::vector3f v);
	void EndUpdate();



	/*
	drawing_mode is GL_POINTS, GL_LINES, GL_TRIANGLES, GL_TRIANGLE_STRIP...
	*/
	inline void drawing_mode(GLenum drawing_mode) { drawing_mode_ = drawing_mode; } 

	inline GLuint ibo_id() const { return ibo_id_; }
	inline int num_vertices() const { return num_vertices_; }
	inline int num_indices() const { return num_indices_; }
	inline GLenum drawing_mode() const { return drawing_mode_; }

	virtual void BindVBO();


protected:
	GLuint ibo_id_;	// GL_ELEMENT_ARRAY_BUFFER (Triangle indices)
	
	std::vector<GL_VBO*> vbos_;

	int num_vertices_;
	int num_indices_;	

	GLenum drawing_mode_;

};	



class GL_VAO
{
	friend class GL_ResourceManager;
private:
	GL_VAO(GL_VBOGroup* vbo_group);
public:
	virtual ~GL_VAO();

	void Bind();
	void Delete();

	GLuint vao_id() const { return vao_id_; }

	virtual void Draw() const;

	GL_VBOGroup* vbo_group() const { return vbo_group_; }

	
protected:
	virtual void GenAndBind();


protected:
	GLuint vao_id_;
	GL_VBOGroup *vbo_group_;

	std::set<int> using_bone_matrix_ids_;
	std::map<int, cml::matrix44d_c> bone_offset_matrices_;
	std::map<int, cml::matrix44d_c> bone_matrices_;

};

GL_RenderableObj* CreateRenderableObjByUsingAssimp(const std::string filename,
	const std::string renderable_obj_name,
	const std::map<std::string, int> &in_bonename_to_pmjoint,
	std::vector< std::pair<std::string, cml::matrix44d_c> > &out_bone_name_and_mat);

GL_VBOGroup* CreatePmHumanVBO(PmHuman *p, double size = 2.0);
//GL_RenderableObj* CreatePmHumanRObj(PmHuman *p, double size=2.0);

};