


#include "BaseLib/GL4U/GL_Renderer.h"
#include "BaseLib/GL4U/GL_ShaderProgram.h"
#include "BaseLib/GL4U/GL_VBOVAO.h"
#include "BaseLib/GL4U/GL_RenderableObj.h"
#include "BaseLib/GL4U/GL_ResourceManager.h"
#include "BaseLib/Geometry/Mesh.h"
#include "BaseLib/Geometry/PrimitiveShape.h"
#include <assert.h>

namespace mg
{

GL_Renderer::GL_Renderer()
{
	// Default Shader
	GL_ShaderProgram *shader = new GL_ShaderProgram;
	shader->CreateDefaultProgram();
	shader->UseProgram();
	shader_ = default_shader_ = shader;

	default_material_ = nullptr;
	CreateDefaultMateiral();
}

GL_Renderer::GL_Renderer(GL_ShaderProgram* shader)
{
	default_shader_ = 0;
	shader_ = shader;
	shader->UseProgram();
	
	default_material_ = nullptr;
	CreateDefaultMateiral();
}



GL_Renderer::~GL_Renderer()
{
	Clear();
}

void
GL_Renderer::Clear()
{
	if ( shader_ != default_shader_ )
	{
		shader_ = 0;
	}

	if ( default_shader_ != 0 )
	{
		if ( default_shader_->IsUsing() )
		{
			glUseProgram(0);
		}
		delete default_shader_;
	}

}




void 
GL_Renderer::Draw(const std::string &renderable_obj_name) 
{
	Draw( GL_ResourceManager::singleton()->GetRenderableObj(renderable_obj_name) );
}

void
GL_Renderer::Draw(const GL_RenderableObj *obj) 
{
	assert(obj!=nullptr);


	// Skinning
	shader_->EnableSkinning(obj->flag_skinning());
	
	if ( obj->flag_skinning() )
	{
		
		//for ( const auto &bone_id : obj->using_bone_matrix_id_set() )
		for ( std::set<int>::const_iterator bone_id_iter=obj->using_bone_matrix_id_set().cbegin();
				bone_id_iter != obj->using_bone_matrix_id_set().cend();
				bone_id_iter++
			)
		{
			int bone_id = *bone_id_iter;
			cml::matrix44f_c m_and_offset = obj->GetBoneMatrix(bone_id) * cml::inverse(obj->GetBoneOffsetMatrix(bone_id));
			shader_->SetBoneMatrix(bone_id, m_and_offset.data());
		}
	}

	// Material
	if ( obj->flag_material() && obj->material() )
	{
		ApplyMaterial(obj->material());
	}
	else
	{
		ApplyMaterial(default_material_);
	}

	// Draw
	DrawVAO(obj->vao());
}

void
GL_Renderer::CreateDefaultMateiral()
{
	if (default_material_ == nullptr)
	{
		std::string name = GL_ResourceManager::singleton()->GenUniqueName("__default_material__");
		default_material_ = GL_ResourceManager::singleton()->CreateMaterial(name);
	}
}

void 
GL_Renderer::ApplyMaterial(const GL_Material *mat)
{
	shader_->SetMaterial(*mat);

	if ( mat->flag_texture() )
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}

	if ( mat->flag_transparency() )
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else
	{
		glDisable(GL_BLEND);
	}
}

void 
GL_Renderer::DrawVAO(const GL_VAO *vao)
{
	vao->Draw();
}

void 
GL_Renderer::DrawSphere()
{
	Draw("primi_sphere"); 
}

void 
GL_Renderer::DrawSphere(double radius, cml::vector3d &center_p) 
{
	PushModelviewMatrix();
	MultiModelviewMatrix(cml::MatrixTranslation(center_p));
	MultiModelviewMatrix(cml::MatrixUniformScaling(radius));
	Draw("primi_sphere"); 
	PopModelviewMatrix();
}

void GL_Renderer::DrawCapsule() 
{ 
	Draw("primi_capsule"); 
}

void GL_Renderer::DrawCapsule(double cylinder_len, double radius) 
{ 
	DrawHemisphere(radius, cml::vector3d(0.0, cylinder_len/2.0, 0.0));
	DrawCylinder(cylinder_len, radius);
	PushModelviewMatrix();
	MultiModelviewMatrix(cml::MatrixRotationEuler(M_PI, 0.0, 0.0));
	DrawHemisphere(radius, cml::vector3d(0.0, cylinder_len/2.0, 0.0));
	PopModelviewMatrix();
}


void GL_Renderer::DrawHemisphere(double radius, cml::vector3d &center_p) 
{ 
	PushModelviewMatrix();
	MultiModelviewMatrix(cml::MatrixTranslation(center_p));
	MultiModelviewMatrix(cml::MatrixUniformScaling(radius));
	Draw("primi_hemisphere"); 
	PopModelviewMatrix();
}

void GL_Renderer::DrawHemisphere() 
{ 
	Draw("primi_hemisphere"); 
}

void GL_Renderer::DrawHead() 
{ 
	Draw("primi_head"); 
}

void GL_Renderer::DrawCylinder() 
{ 
	Draw("primi_cylinder"); 
}

void GL_Renderer::DrawCylinder(double cylinder_len, double radius) 
{
	PushModelviewMatrix();
	MultiModelviewMatrix(cml::MatrixScaling(cml::vector3d(radius, cylinder_len, radius)));
	Draw("primi_cylinder"); 
	PopModelviewMatrix();
}

void GL_Renderer::DrawCylinder(cml::vector3d &bottom_p, cml::vector3d &top_p, double radius) 
{
	cml::vector3d align_vec = top_p-bottom_p;
	cml::matrix44d_c align_rot;
	cml::matrix_rotation_vec_to_vec(align_rot, cml::y_axis_3D(), align_vec);

	PushModelviewMatrix();
	MultiModelviewMatrix(cml::MatrixTranslation(bottom_p));
	MultiModelviewMatrix(align_rot);
	MultiModelviewMatrix(cml::MatrixScaling(cml::vector3d(radius, align_vec.length(), radius)));
	MultiModelviewMatrix(cml::MatrixTranslation(0.0, 0.5, 0.0));
	Draw("primi_cylinder"); 
	PopModelviewMatrix();
}

void GL_Renderer::DrawOpenedCylinder() 
{ 
	Draw("primi_opened_cylinder"); 
}

void GL_Renderer::DrawBox(double width, double height, double depth) 
{ 
	PushModelviewMatrix();
	MultiModelviewMatrix(cml::MatrixScaling(width, height, depth));
	Draw("primi_box"); 
	PopModelviewMatrix();
}

void
GL_Renderer::Draw(const mg::PrimitiveShape *p) 
{
	PushModelviewMatrix();
	MultiModelviewMatrix(cml::MatrixTranslation(p->global_translation()));
	MultiModelviewMatrix(cml::MatrixRotationQuaternion(p->global_rotation()));
	switch (p->type())
	{
	case mg::PrimitiveShape::BOX:
	{
		mg::PrimitiveBox* box = (mg::PrimitiveBox*)p;
		DrawBox(box->width(), box->height(), box->depth());
		break;
	}

	case mg::PrimitiveShape::SPHERE:
	{
		mg::PrimitiveSphere* sphere = (mg::PrimitiveSphere*)p;
		DrawSphere(sphere->radius());
		break;
	}

	case mg::PrimitiveShape::CYLINDER:
	{
		mg::PrimitiveCylinder* cyl = (mg::PrimitiveCylinder*)p;
		DrawCylinder(cyl->height(), cyl->radius());
		break;
	}

	case mg::PrimitiveShape::CAPSULE:
	{
		mg::PrimitiveCapsule* cap = (mg::PrimitiveCapsule*)p;
		DrawCapsule(cap->cylinder_height(), cap->radius());
		break;
	}

	default:
		break;
	}
	PopModelviewMatrix();
}

void
GL_Renderer::Draw(const mg::PrimitiveComposition *p)
{
	PushModelviewMatrix();
	MultiModelviewMatrix(cml::MatrixTranslation((p->global_translation())));
	MultiModelviewMatrix(cml::MatrixRotationQuaternion((p->global_rotation())));

	for (int i = p->CountPrimitives() - 1; i >= 0; i--)
	{
		Draw(p->primitive(i));
	}
	PopModelviewMatrix();

}




void 
GL_Renderer::SetProjectionMatrix(const cml::matrix44d_c &m)
{
	shader_->SetProjectionMatrix(((cml::matrix44f_c)m).data());
}

void 
GL_Renderer::SetModelviewMatrix(const cml::matrix44d_c &m)
{
	modelview_mat_ = m;
	shader_->SetModelviewMatrix(((cml::matrix44f_c)m).data());
}

void
GL_Renderer::SetViewMatrix(const cml::matrix44d_c &m)
{
	view_mat_ = m;
	SetModelviewMatrix(view_mat_);
}

void
GL_Renderer::SetModelMatrix(const cml::matrix44d_c &m)
{
	SetModelviewMatrix(view_mat_*m);
}

void 
GL_Renderer::PushModelviewMatrix()
{
	modelview_mat_stack_.push(modelview_mat_);
}


void
GL_Renderer::MultiModelviewMatrix(const cml::matrix44d_c &m)
{
	modelview_mat_ = modelview_mat_*m;
	shader_->SetModelviewMatrix(((cml::matrix44f_c)modelview_mat_).data());
}

void 
GL_Renderer::PopModelviewMatrix()
{
	SetModelviewMatrix(modelview_mat_stack_.top());
	modelview_mat_stack_.pop();
}






/////////////////////////////
// Indexed Color

static int g_last_used_color_index;
static const int g_indexed_color_num = 42;
static const float g_indexed_color_set[g_indexed_color_num][3] = { 
			{0.7f, 1.0f, 1.0f},
			{1.0f, 0.7f, 0.7f},
			{0.7f, 1.0f, 0.7f},
			{0.7f, 0.7f, 1.0f},
			{0/255.0f,	204/255.0f,	255/255.0f},	
			{204/255.0f,	255/255.0f,	204/255.0f},
			{255/255.0f,	255/255.0f,	153/255.0f},
			{153/255.0f,	204/255.0f,	255/255.0f},
			{255/255.0f,	153/255.0f,	204/255.0f},
			{204/255.0f,	153/255.0f,	255/255.0f},	// 10
			{255/255.0f,	204/255.0f,	153/255.0f},
			{51/255.0f,	102/255.0f,	255/255.0f},
			{51/255.0f,	204/255.0f,	204/255.0f},
			{153/255.0f,	204/255.0f,	0/255.0f},
			{255/255.0f,	204/255.0f,	0/255.0f},
			{255/255.0f,	153/255.0f,	0/255.0f},
			{255/255.0f,	102/255.0f,	0/255.0f},
			{102/255.0f,	102/255.0f,	153/255.0f},
			{150/255.0f,	150/255.0f,	150/255.0f},
			{0/255.0f,	51/255.0f,	102/255.0f},	// 20
			{51/255.0f,	153/255.0f,	102/255.0f},
			{0/255.0f,	51/255.0f,	0/255.0f},
			{51/255.0f,	51/255.0f,	0/255.0f},
			{153/255.0f,	51/255.0f,	0/255.0f},
			{51/255.0f,	51/255.0f,	153/255.0f},
			{51/255.0f,	51/255.0f,	51/255.0f},
			{128/255.0f,	0/255.0f,	0/255.0f},
			{0/255.0f,	128/255.0f,	0/255.0f},
			{0/255.0f,	0/255.0f,	128/255.0f},
			{128/255.0f,	128/255.0f,	0/255.0f},	// 30
			{128/255.0f,	0/255.0f,	128/255.0f},
			{0/255.0f,	128/255.0f,	128/255.0f},
			{192/255.0f,	192/255.0f,	192/255.0f},
			{128/255.0f,	128/255.0f,	128/255.0f},
			{153/255.0f,	153/255.0f,	255/255.0f},
			{153/255.0f,	51/255.0f,	102/255.0f},
			{255/255.0f,	255/255.0f,	204/255.0f},
			{204/255.0f,	255/255.0f,	255/255.0f},
			{102/255.0f,	0/255.0f,	102/255.0f},
			{255/255.0f,	128/255.0f,	128/255.0f},	// 40
			{0/255.0f,	102/255.0f,	204/255.0f},
			{204/255.0f,	204/255.0f,	255/255.0f}
			};
void 
GL_Renderer::SetColor(int i, float a)
{
	int r = i % g_indexed_color_num;
	SetColor(g_indexed_color_set[r][0], g_indexed_color_set[r][1], g_indexed_color_set[r][2], a);
	g_last_used_color_index = r;
}

void
GL_Renderer::SetColor(float r, float g, float b, float a)
{
	//shader_->SetColor(r, g, b, a);
	default_material_->diffuse_color(r, g, b, a);
	default_material_->ambient_color(r, g, b, a);
}

void
GL_Renderer::SetAlpha(float a)
{
	default_material_->ChangeAllAlphas(a);
}

void
GL_Renderer::EnableTransparency(bool f)
{
	default_material_->flag_transparency(f);
}

int
GL_Renderer::GetRecentlyUsedColorIndex()
{
	return g_last_used_color_index;
}

};
