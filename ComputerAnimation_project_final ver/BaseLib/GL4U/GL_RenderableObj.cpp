
#include "BaseLib/GL4U/GL_RenderableObj.h"
#include "BaseLib/Geometry/Mesh.h"
#include "BaseLib/GL4U/GL_Material.h"
#include "BaseLib/GL4U/GL_VBOVAO.h"



namespace mg
{

GL_RenderableObj::GL_RenderableObj()
{
	material_ = 0;
	vao_ = 0;
	flag_skinning_ = false;
	flag_material_ = false;
}

GL_RenderableObj::~GL_RenderableObj()
{
	Clear();
}

void
GL_RenderableObj::Clear()
{
	using_bone_matrix_ids_.clear();
	bone_offset_matrices_.clear();
	bone_matrices_.clear();

	vao_ = 0;
	material_ = 0;
}

void
GL_RenderableObj::Initialize(GL_VAO *vao, GL_Material *material)
{
	vao_ = vao;
	material_ = material;
	if ( material ) flag_material_ = true;
}

void
GL_RenderableObj::SetBoneOffsetMatrix(int bone_id, cml::matrix44d_c m)
{
	using_bone_matrix_ids_.insert(bone_id);
	bone_offset_matrices_[bone_id] = m;
}

void
GL_RenderableObj::SetBoneMatrix(int bone_id, cml::matrix44d_c m)
{
	using_bone_matrix_ids_.insert(bone_id);
	bone_matrices_[bone_id] = m;
}


const cml::matrix44d_c&
GL_RenderableObj::GetBoneMatrix(int bone_id) const
{
	if ( bone_matrices_.find(bone_id) == bone_matrices_.end() )
	{
		cml::matrix44d_c m;
		return m.identity();
	}

	return bone_matrices_.find(bone_id)->second;
}

const cml::matrix44d_c&
GL_RenderableObj::GetBoneOffsetMatrix(int bone_id) const
{
	if ( bone_offset_matrices_.find(bone_id) == bone_offset_matrices_.end() )
	{
		cml::matrix44d_c m;
		return m.identity();
	}

	return bone_offset_matrices_.find(bone_id)->second;
}


};








