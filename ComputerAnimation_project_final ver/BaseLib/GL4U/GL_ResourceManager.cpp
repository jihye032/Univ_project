
#include "BaseLib/GL4U/GL_ResourceManager.h"
#include "BaseLib/Geometry/Mesh.h"

namespace mg
{

GL_ResourceManager* GL_ResourceManager::singleton_ = nullptr;

GL_ResourceManager::GL_ResourceManager()
{
}

GL_ResourceManager*
GL_ResourceManager::singleton()
{
	if ( singleton_ == nullptr )
	{
		singleton_ = new GL_ResourceManager;
		singleton_->CreateAndAddPrimitiveShapes();
		singleton_->CreateAndAddGroundObj();
	}

	return singleton_;
}

std::string
GL_ResourceManager::GenUniqueName(std::string prefix) const
{
	int i = 0;
	while (1)
	{
		char n[256];
		sprintf_s(n, "%d", i);
		std::string name = prefix + n;//std::to_string(i);
		
		if ( GetVAO(name) == nullptr &&
			GetVBOGroup(name) == nullptr &&
			GetMaterial(name) == nullptr &&
			GetTexture(name) == nullptr &&
			GetRenderableObj(name) == nullptr )
			return name;
		i++;
	}
}

void
GL_ResourceManager::CreateAndAddPrimitiveShapes()
{
	// Create VBO Groups
	{
		std::map< std::string, mg::Mesh > primi_meshes;
		primi_meshes["primi_sphere"].CreateSphere();
		primi_meshes["primi_capsule"].CreateCapsule();
		primi_meshes["primi_cylinder"].CreateCylinder();
		primi_meshes["primi_head"].CreateHeadModel();
		primi_meshes["primi_hemisphere"].CreateHemisphere();
		primi_meshes["primi_opened_cylinder"].CreateOpenedCylinder();
		primi_meshes["primi_box"].CreateBox();

		//for ( auto &mesh : primi_meshes )
		for ( std::map< std::string, mg::Mesh >::iterator mesh_iter = primi_meshes.begin();
			mesh_iter != primi_meshes.end();
			mesh_iter++ )
		{
			std::pair< std::string, mg::Mesh > mesh = *(mesh_iter);

			std::string name = mesh.first;
			mesh.second.UpdateNormalVectorsBasedOnVertex();

			// GL_VBOGroup
			GL_VBOGroup *vbo_group = CreateVBOGroup("vbo_" + name);
			vbo_group->SetByMesh(mesh.second);
		

			// GL_VAO
			GL_VAO *vao = CreateVAO("vao_" + name, vbo_group);
		

			// GL_RenderableObj
			GL_RenderableObj* obj = CreateRendarableObj(name, vao, nullptr);
		}
	}

}


void
GL_ResourceManager::CreateAndAddGroundObj()
{
	// Ground
	{
		int i;
		int lineNum = 51;
		float blockWidth = 20.0f;
		float min = -1 * (50 / 2) * blockWidth;
		float max = (50 / 2) * blockWidth;


		GL_VBOGroup *ground_vbo_ = mg::GL_ResourceManager::singleton()->CreateVBOGroup("vbo_ground");
		{
			ground_vbo_->drawing_mode(GL_LINES);
			ground_vbo_->BeginVertex();

			for (i = 0; i < lineNum; i += 1)
			{
				if (i == 25)
					ground_vbo_->glColor(cml::vector4f( 0.2f, 0.2f, 0.2f, 1.0f));
				else
					ground_vbo_->glColor(cml::vector4f( 0.9f, 0.9f, 0.9f, 1.0f ));
				ground_vbo_->glNormal(cml::vector3f( 0.0f, 1.0f, 0.0f ));
				ground_vbo_->glVertex(cml::vector3f(min + i*blockWidth, 0.f, min));
				ground_vbo_->glVertex(cml::vector3f(min + i*blockWidth, 0.f, max));
				ground_vbo_->glVertex(cml::vector3f(min, 0.f, min + i*blockWidth));
				ground_vbo_->glVertex(cml::vector3f(max, 0.f, min + i*blockWidth));
			}
			ground_vbo_->EndVertex();
		}

		GL_Material *ground_mat = mg::GL_ResourceManager::singleton()->CreateMaterial("mat_ground");
		ground_mat->flag_use_vertex_color(true);
		ground_mat->flag_shading(false);


		// GL_RenderableObj
		GL_ResourceManager::singleton()->CreateRendarableObj("ground", ground_vbo_, ground_mat);

	}

}



GL_VAO*
GL_ResourceManager::CreateVAO(std::string name, GL_VBOGroup *vbo_group)
{
	if (name.length() == 0) name = GenUniqueName("VAO");
	
	std::unique_ptr<GL_VAO> vao(new GL_VAO(vbo_group));
	vao_set_[name] = std::move(vao);

	return vao_set_[name].get();
}

GL_VBOGroup*
GL_ResourceManager::CreateVBOGroup(std::string name)
{
	if (name.length() == 0) name = GenUniqueName("VBOGroup");

	std::unique_ptr<GL_VBOGroup> vbo(new GL_VBOGroup);
	vbo_group_set_[name] = std::move(vbo);

	return vbo_group_set_[name].get();
}

GL_Material*
GL_ResourceManager::CreateMaterial(std::string name)
{
	if (name.length() == 0) name = GenUniqueName("MAT");

	std::unique_ptr<GL_Material> mat(new GL_Material);
	material_set_[name] = std::move(mat);

	return material_set_[name].get();
}

GL_Texture*
GL_ResourceManager::CreateTexture(std::string name)
{
	if (name.length() == 0) name = GenUniqueName("MAT");

	std::unique_ptr<GL_Texture> t(new GL_Texture);
	texture_set_[name] = std::move(t);

	return texture_set_[name].get();
}

GL_RenderableObj*
GL_ResourceManager::CreateRendarableObj(std::string name, GL_VAO *vao, GL_Material *material)
{
	if (name.length() == 0) name = GenUniqueName("OBJ");

	std::unique_ptr<GL_RenderableObj> t(new GL_RenderableObj);
	t->Initialize(vao, material);
	renderable_obj_set_[name] = std::move(t);

	return renderable_obj_set_[name].get();
}

GL_RenderableObj*
GL_ResourceManager::CreateRendarableObj(std::string name, GL_VBOGroup *vbo_group, GL_Material *material)
{
	GL_VAO* vao = CreateVAO("", vbo_group);

	return CreateRendarableObj(name, vao, material);
}

GL_VAO* 
GL_ResourceManager::GetVAO(std::string name) const
{
	const auto &d = vao_set_.find(name);
	if (d == vao_set_.end()) return nullptr;
	return d->second.get();
}

GL_VBOGroup* 
GL_ResourceManager::GetVBOGroup(std::string name) const
{
	const auto &d = vbo_group_set_.find(name);
	if (d == vbo_group_set_.end()) return nullptr;
	return d->second.get();
}

GL_Material* 
GL_ResourceManager::GetMaterial(std::string name) const
{
	const auto &d = material_set_.find(name);
	if (d == material_set_.end()) return nullptr;
	return d->second.get();
}

GL_Texture* 
GL_ResourceManager::GetTexture(std::string name) const
{
	const auto &d = texture_set_.find(name);
	if (d == texture_set_.end()) return nullptr;
	return d->second.get();
}

GL_RenderableObj* 
GL_ResourceManager::GetRenderableObj(std::string name) const
{
	const auto &d = renderable_obj_set_.find(name);
	if (d == renderable_obj_set_.end()) return nullptr;
	return d->second.get();
}

};





