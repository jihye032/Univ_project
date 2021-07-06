
#pragma once
#include "GL/glew.h"
#include <map>
#include <string>
#include <stack>


#include "BaseLib/GL4U/GL_VBOVAO.h"
#include "BaseLib/GL4U/GL_Material.h"
#include "BaseLib/GL4U/GL_Texture.h"
#include "BaseLib/GL4U/GL_RenderableObj.h"


namespace mg
{


class GL_ResourceManager
{
private:
	GL_ResourceManager();
	static GL_ResourceManager* singleton_;

	void CreateAndAddPrimitiveShapes();
	void CreateAndAddGroundObj();

public:
	static GL_ResourceManager* singleton();

	GL_VAO*				CreateVAO(std::string name, GL_VBOGroup *vbo_group);
	GL_VBOGroup*		CreateVBOGroup(std::string name="");
	GL_Material*		CreateMaterial(std::string name="");
	GL_Texture*			CreateTexture(std::string name = "");
	GL_RenderableObj*	CreateRendarableObj(std::string name, GL_VAO *vao, GL_Material *material=nullptr);
	GL_RenderableObj*	CreateRendarableObj(std::string name, GL_VBOGroup *vbo_group, GL_Material *material=nullptr);



	GL_VAO* GetVAO(std::string name) const;
	GL_VBOGroup* GetVBOGroup(std::string name) const;
	GL_Material* GetMaterial(std::string name) const;
	GL_Texture* GetTexture(std::string name) const;
	GL_RenderableObj* GetRenderableObj(std::string name) const;

	// GL_Material* GetDefaultMaterial() const { return GetMaterial("mat_default"); }

	std::string GenUniqueName(std::string prefix="") const;

private:
	std::map< std::string, std::unique_ptr<GL_VAO> > vao_set_;
	std::map< std::string, std::unique_ptr<GL_VBOGroup> > vbo_group_set_;
	std::map< std::string, std::unique_ptr<GL_Material> > material_set_;
	std::map< std::string, std::unique_ptr<GL_Texture> > texture_set_;
	std::map< std::string, std::unique_ptr<GL_RenderableObj> > renderable_obj_set_;

};	





};

