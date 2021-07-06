
#pragma once
#include "BaseLib/CmlExt/CmlExt.h"
#include <map>
#include <set>


namespace mg
{

class GL_Material;
class GL_VAO;

class GL_RenderableObj
{
	friend class GL_ResourceManager;
public:
	GL_RenderableObj();
	~GL_RenderableObj();

	void Clear();

	void Initialize(GL_VAO *vao, GL_Material *material = 0);

	void SetBoneMatrix(int bone_id, cml::matrix44d_c m);
	void SetBoneOffsetMatrix(int bone_id, cml::matrix44d_c m);

	const cml::matrix44d_c& GetBoneMatrix(int bone_id) const;
	const cml::matrix44d_c& GetBoneOffsetMatrix(int bone_id) const;

	const std::set<int>& using_bone_matrix_id_set() const { return using_bone_matrix_ids_; }

	void flag_skinning(bool f) { flag_skinning_ = f; }
	void flag_material(bool f) { flag_material_ = f; }

	GL_VAO* vao() const { return vao_; }
	GL_Material* material() const { return material_; }
	
	bool flag_skinning() const { return flag_skinning_; }
	bool flag_material() const { return flag_material_; }

protected:
	GL_VAO *vao_;
	GL_Material *material_;

	bool flag_skinning_;
	bool flag_material_;

	std::set<int> using_bone_matrix_ids_;
	std::map<int, cml::matrix44d_c> bone_offset_matrices_;
	std::map<int, cml::matrix44d_c> bone_matrices_;
};



};