
#include "BaseLib/GL4U/GL_ResourceManager.h"

#include "assimp/scene.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/DefaultLogger.hpp"
#include "assimp/LogStream.hpp"
#include "IL/il.h"

namespace mg
{

void
GL_VBOGroup::SetByAssimp(const std::string filename)
{
	std::vector< std::pair<std::string, cml::matrix44d_c> > out_bone_name_and_mat;
	SetByAssimp(filename, out_bone_name_and_mat);
}

void
GL_VBOGroup::SetByAssimp(const std::string filename,  std::vector< std::pair<std::string, cml::matrix44d_c> > &out_bone_name_and_mat)
{
	std::map<std::string, int> empty_bonename_to_pmjoint;
	SetByAssimp(filename, empty_bonename_to_pmjoint, out_bone_name_and_mat);
}

void
GL_VBOGroup::SetByAssimp(const std::string filename,  const std::map<std::string, int> &in_bonename_to_pmjoint)
{
	std::vector< std::pair<std::string, cml::matrix44d_c> > out_bone_name_and_mat;
	SetByAssimp(filename, in_bonename_to_pmjoint, out_bone_name_and_mat);
}


void
GL_VBOGroup::SetByAssimp(const std::string filename, 
					const std::map<std::string, int> &in_bonename_to_pmjoint, 
					std::vector< std::pair<std::string, cml::matrix44d_c> > &out_bone_name_and_mat)
{
	Delete();

	//Assimp::Logger::LogSeverity severity = Assimp::Logger::NORMAL;
	Assimp::Logger::LogSeverity severity = Assimp::Logger::VERBOSE;

	// Create a logger instance for Console Output
	Assimp::DefaultLogger::create("",severity, aiDefaultLogStream_STDOUT);

	// Create a logger instance for File Output (found in project folder or near .exe)
	Assimp::DefaultLogger::create("assimp_log.txt",severity, aiDefaultLogStream_FILE);

	// Now I am ready for logging my stuff
	Assimp::DefaultLogger::get()->info("this is my info-call");


	Assimp::Importer ai_importer;

	const aiScene *ai_scene_ = ai_importer.ReadFile(filename, aiProcessPreset_TargetRealtime_Quality|aiProcess_Triangulate);


	// Kill it after the work is done
	Assimp::DefaultLogger::kill();

	//ai_scene_->mMaterials

	for ( unsigned int i=0; i<ai_scene_->mNumMeshes; i++ )
	{
		const aiMesh *mesh = ai_scene_->mMeshes[i];
		if ( mesh->HasBones() )
		{
			Init(mesh->mNumVertices);

			// vertices
			SetVBO(VERTEX_VBO, GL_FLOAT, 3, (void*)mesh->mVertices);
			
			// normals
			SetVBO(NORMAL_VBO, GL_FLOAT, 3, (void*)mesh->mNormals);
			

			// uvs
			{
				if ( mesh->GetNumUVChannels() >= 1 )
				{
					if ( mesh->mNumUVComponents[0] == 2 )
					{
						SetVBO(TEX_COORD_VBO, GL_FLOAT, 3, (void*)mesh->mTextureCoords[0]);
					}
				}
			}

			// color
			{
				if ( mesh->GetNumColorChannels() >= 1 )
				{
					SetVBO(COLOR_VBO, GL_FLOAT, 4, (void*)mesh->mColors[0]);
				}
			}

			

			// Triangle indices
			{
				unsigned int *tri_indices = new unsigned int[mesh->mNumFaces * 3];

				for ( unsigned int i=0; i<mesh->mNumFaces; i++ )
				{
					tri_indices[i*3+0] = mesh->mFaces[i].mIndices[0];
					tri_indices[i*3+1] = mesh->mFaces[i].mIndices[1];
					tri_indices[i*3+2] = mesh->mFaces[i].mIndices[2];
				}
				SetElementIndices(mesh->mNumFaces*3, tri_indices);
				drawing_mode(GL_TRIANGLES);

				delete[] tri_indices;
			}

			// Return bone names and offset_matrices
			if ( mesh->HasBones() )
			{
				for ( unsigned int i=0; i<mesh->mNumBones; i++ )
				{
					std::pair<std::string, cml::matrix44d_c> name_and_mat;
					
					name_and_mat.first = std::string( mesh->mBones[i]->mName.C_Str() );

					aiNode *node = ai_scene_->mRootNode->FindNode( mesh->mBones[i]->mName );

					aiMatrix4x4 m = mesh->mBones[i]->mOffsetMatrix;
					m.Inverse();
					

					for ( int j=0; j<4; j++ )
					for ( int k=0; k<4; k++ )
					{
						name_and_mat.second.set_basis_element(j, k
							, m[k][j]);
					}

					

					out_bone_name_and_mat.push_back(name_and_mat);
				}
			}

			
			// Weight
			if ( mesh->HasBones() )
			{
				// We assume that maximum 4 bones can be related for a vertex at most.

				int *nums_bones_per_vertex = new int[mesh->mNumVertices];
				int *bone_ids = new int[4*mesh->mNumVertices];
				double *bone_weights = new double[4*mesh->mNumVertices];

				// initialize
				for ( unsigned int i=0; i<mesh->mNumVertices; i++ )
				{
					nums_bones_per_vertex[i] = 0;
					bone_ids[i*4+0] = bone_ids[i*4+1] = bone_ids[i*4+2] = bone_ids[i*4+3] = -1;
					bone_weights[i*4+0] = bone_weights[i*4+1] = bone_weights[i*4+2] = bone_weights[i*4+3] = 0.0f;
				}


				for ( unsigned int i=0; i<mesh->mNumBones; i++ )
				{
					for ( unsigned int j=0; j<mesh->mBones[i]->mNumWeights; j++ )
					{
						int vertex_id = mesh->mBones[i]->mWeights[j].mVertexId;
						float weight = mesh->mBones[i]->mWeights[j].mWeight;

						// If there are more than four joints related at one vertex,
						// we just skip the fifth ane later one. 
						if ( nums_bones_per_vertex[vertex_id] == 4 )
						{
							continue;
						}
						

						int bone_id = i;
						if ( in_bonename_to_pmjoint.find(mesh->mBones[i]->mName.C_Str()) != in_bonename_to_pmjoint.end() )
						{
							bone_id = in_bonename_to_pmjoint.find(mesh->mBones[i]->mName.C_Str())->second;
						}
						bone_ids[ vertex_id*4 + nums_bones_per_vertex[vertex_id] ] = bone_id;
						bone_weights[ vertex_id*4 + nums_bones_per_vertex[vertex_id] ] = weight;

						nums_bones_per_vertex[vertex_id]++;
					}
				
					
				}

				SetVBO(BONE_ID_VBO, GL_INT, 4, bone_ids);
				SetVBO(BONE_WEIGHT_VBO, GL_DOUBLE, 4, bone_weights);


				delete[] nums_bones_per_vertex;
				delete[] bone_ids;
				delete[] bone_weights;
			}

			break;
		}
	}
}



// images / texture
// static std::string modelpath;
// std::map<std::string, GLuint*> textureIdMap;	// map image filenames to textureIds
//  GLuint*		textureIds;	

std::string getBasePath(const std::string& path)
{
	size_t pos = path.find_last_of("\\/");
	return (std::string::npos == pos) ? "" : path.substr(0, pos + 1);
}

static std::string g_base_path;

mg::GL_Texture* CreateGLTextures(std::string filename)
{
	ILboolean success;

	mg::GL_Texture *out_gl_tex = nullptr;

	/* Before calling ilInit() version should be checked. */
	if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION)
	{
		/// wrong DevIL version ///
		std::string err_msg = "Wrong DevIL version. Old devil.dll in system32/SysWow64?";
		char* cErr_msg = (char *)err_msg.c_str();
		return false;
	}

	ilInit(); /* Initialization of DevIL */

	
	/* array with DevIL image IDs */
	ILuint imageId;

	/* generate DevIL Image IDs */
	ilGenImages(1, &imageId); /* Generation of numTextures image names */

	ilBindImage(imageId); /* Binding of DevIL image name */
	
	success = ilLoadImage(filename.c_str());

	if (success) /* If no error occured: */
	{
		success = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE); /* Convert every colour component into
		unsigned byte. If your image contains alpha channel you can replace IL_RGB with IL_RGBA */
		if (!success)
		{
			/* Error occured */
			// MessageBox(NULL, ("Couldn't convert image" + fileloc).c_str(), "ERROR", MB_OK | MB_ICONEXCLAMATION);
			return nullptr;
		}
		else
		{

			////glGenTextures(numTextures, &textureIds[i]); /* Texture name generation */
			//glBindTexture(GL_TEXTURE_2D, textureIds[i]); /* Binding of texture name */
			////redefine standard texture values
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); /* We will use linear
			//interpolation for magnification filter */
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); /* We will use linear
			//interpolation for minifying filter */
			//glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH),
			//	ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE,
			//	ilGetData()); /* Texture specification */

			out_gl_tex = mg::GL_ResourceManager::singleton()->CreateTexture();// new mg::GL_Texture;

			out_gl_tex->Gen2DAndBind(ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT));
			out_gl_tex->SetData(ilGetData());

		}
	}
	else
	{
		/* Error occured */
		// MessageBox(NULL, ("Couldn't load Image: " + fileloc).c_str(), "ERROR", MB_OK | MB_ICONEXCLAMATION);
	}


	ilDeleteImages(1, &imageId); /* Because we have already copied image data into texture data
	we can release memory used by image. */

	//Cleanup
	
	//return success;
	return out_gl_tex;
}


// Can't send color down as a pointer to aiColor4D because AI colors are ABGR.
static void Color4f(const aiColor4D *color)
{
	glColor4f(color->r, color->g, color->b, color->a);
}

static void set_float4(float f[4], float a, float b, float c, float d)
{
	f[0] = a;
	f[1] = b;
	f[2] = c;
	f[3] = d;
}

static void color4_to_float4(const aiColor4D *c, float f[4])
{
	f[0] = c->r;
	f[1] = c->g;
	f[2] = c->b;
	f[3] = c->a;
}


static mg::GL_Material* CreateGlMaterial(const aiMaterial *mtl)
{
	//mg::GL_Material *out_gl_material = new mg::GL_Material;
	mg::GL_Material *out_gl_material = mg::GL_ResourceManager::singleton()->CreateMaterial();

	float c[4];

	// GLenum fill_mode;
	int ret1, ret2;
	aiColor4D diffuse;
	aiColor4D specular;
	aiColor4D ambient;
	aiColor4D emission;
	float shininess, strength;
	// int two_sided;
	// int wireframe;
	unsigned int max;	// changed: to unsigned

	int texIndex = 0;
	aiString texPath;	//contains filename of texture

	if(AI_SUCCESS == mtl->GetTexture(aiTextureType_DIFFUSE, texIndex, &texPath))
	{
		//bind texture
		// unsigned int texId = *textureIdMap[texPath.data];
		// glBindTexture(GL_TEXTURE_2D, texId);

		GL_Texture *tex = CreateGLTextures(g_base_path + texPath.C_Str());
		if ( tex != nullptr ) 
		{
			out_gl_material->AddTexture(tex);
			out_gl_material->flag_texture(true);
		}
	}

	set_float4(c, 0.8f, 0.8f, 0.8f, 1.0f);
	if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &diffuse))
		color4_to_float4(&diffuse, c);
	out_gl_material->diffuse_color(c[0], c[1], c[2], c[3]);

	set_float4(c, 0.0f, 0.0f, 0.0f, 1.0f);
	if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &specular))
		color4_to_float4(&specular, c);
	out_gl_material->specular_color(c[0], c[1], c[2], c[3]);

	set_float4(c, 0.2f, 0.2f, 0.2f, 1.0f);
	if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT, &ambient))
		color4_to_float4(&ambient, c);
	out_gl_material->ambient_color(c[0], c[1], c[2], c[3]);

	set_float4(c, 0.0f, 0.0f, 0.0f, 1.0f);
	if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_EMISSIVE, &emission))
		color4_to_float4(&emission, c);
	out_gl_material->emission_color(c[0], c[1], c[2], c[3]);

	max = 1;
	ret1 = aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS, &shininess, &max);
	if( ret1 == AI_SUCCESS )
		out_gl_material->shininess(shininess);

	max = 1;
	ret2 = aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS_STRENGTH, &strength, &max);
	if( ret2 == AI_SUCCESS )
		out_gl_material->shininess_strength(strength);

	return out_gl_material;
	
/*

	max = 1;
	if(AI_SUCCESS == aiGetMaterialIntegerArray(mtl, AI_MATKEY_ENABLE_WIREFRAME, &wireframe, &max))
		fill_mode = wireframe ? GL_LINE : GL_FILL;
	else
		fill_mode = GL_FILL;
	glPolygonMode(GL_FRONT_AND_BACK, fill_mode);

	max = 1;
	if((AI_SUCCESS == aiGetMaterialIntegerArray(mtl, AI_MATKEY_TWOSIDED, &two_sided, &max)) && two_sided)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);*/
}



GL_RenderableObj* CreateRenderableObjByUsingAssimp(const std::string filename,
													const std::string renderable_obj_name,
													const std::map<std::string, int> &in_bonename_to_pmjoint,
												std::vector< std::pair<std::string, cml::matrix44d_c> > &out_bone_name_and_mat)
{

	g_base_path = getBasePath(filename);
	mg::GL_VBOGroup* vbo_group = mg::GL_ResourceManager::singleton()->CreateVBOGroup();
	mg::GL_Material* gl_material = nullptr;
	bool flag_has_bone = false;

	//Assimp::Logger::LogSeverity severity = Assimp::Logger::NORMAL;
	Assimp::Logger::LogSeverity severity = Assimp::Logger::VERBOSE;

	// Create a logger instance for Console Output
	Assimp::DefaultLogger::create("", severity, aiDefaultLogStream_STDOUT);

	// Create a logger instance for File Output (found in project folder or near .exe)
	Assimp::DefaultLogger::create("assimp_log.txt", severity, aiDefaultLogStream_FILE);

	// Now I am ready for logging my stuff
	Assimp::DefaultLogger::get()->info("this is my info-call");


	Assimp::Importer ai_importer;

	const aiScene *ai_scene_ = ai_importer.ReadFile(filename, aiProcessPreset_TargetRealtime_Quality | aiProcess_Triangulate);


	// Kill it after the work is done
	Assimp::DefaultLogger::kill();

	//ai_scene_->mMaterials

	for (unsigned int i = 0; i<ai_scene_->mNumMeshes; i++)
	{
		const aiMesh *mesh = ai_scene_->mMeshes[i];
		if (mesh->HasBones())
		{
			vbo_group->Init(mesh->mNumVertices);

			// vertices
			vbo_group->SetVBO(GL_VBOGroup::VERTEX_VBO, GL_FLOAT, 3, (void*)mesh->mVertices);

			// normals
			vbo_group->SetVBO(GL_VBOGroup::NORMAL_VBO, GL_FLOAT, 3, (void*)mesh->mNormals);


			// tex_coords
			{
				if (mesh->GetNumUVChannels() >= 1)
				{
					if (mesh->mNumUVComponents[0] == 2)
					{
						// transform 
						for (unsigned int t_i = 0; t_i<mesh->mNumVertices; t_i++)
						{
							mesh->mTextureCoords[0][t_i][1] = 1 - mesh->mTextureCoords[0][t_i][1];
						}
						vbo_group->SetVBO(GL_VBOGroup::TEX_COORD_VBO, GL_FLOAT, 3, (void*)mesh->mTextureCoords[0]);
					}
				}
			}

			// color
			{
				if (mesh->GetNumColorChannels() >= 1)
				{
					vbo_group->SetVBO(GL_VBOGroup::COLOR_VBO, GL_FLOAT, 4, (void*)mesh->mColors[0]);
				}
			}



			// Triangle indices
			{
				unsigned int *tri_indices = new unsigned int[mesh->mNumFaces * 3];

				for (unsigned int i = 0; i<mesh->mNumFaces; i++)
				{
					tri_indices[i * 3 + 0] = mesh->mFaces[i].mIndices[0];
					tri_indices[i * 3 + 1] = mesh->mFaces[i].mIndices[1];
					tri_indices[i * 3 + 2] = mesh->mFaces[i].mIndices[2];
				}
				vbo_group->SetElementIndices(mesh->mNumFaces * 3, tri_indices);
				vbo_group->drawing_mode(GL_TRIANGLES);

				delete[] tri_indices;
			}

			// Return bone names and offset_matrices
			if (mesh->HasBones())
			{
				flag_has_bone = true;
				for (unsigned int i = 0; i<mesh->mNumBones; i++)
				{
					std::pair<std::string, cml::matrix44d_c> name_and_mat;

					name_and_mat.first = std::string(mesh->mBones[i]->mName.C_Str());

					aiNode *node = ai_scene_->mRootNode->FindNode(mesh->mBones[i]->mName);

					aiMatrix4x4 m = mesh->mBones[i]->mOffsetMatrix;
					m.Inverse();


					for (int j = 0; j<4; j++)
						for (int k = 0; k<4; k++)
						{
							name_and_mat.second.set_basis_element(j, k
								, m[k][j]);
						}



					out_bone_name_and_mat.push_back(name_and_mat);
				}
			}


			// Weight
			if (mesh->HasBones())
			{
				// We assume that maximum 4 bones can be related for a vertex at most.

				int *nums_bones_per_vertex = new int[mesh->mNumVertices];
				int *bone_ids = new int[4 * mesh->mNumVertices];
				double *bone_weights = new double[4 * mesh->mNumVertices];

				// initialize
				for (unsigned int i = 0; i<mesh->mNumVertices; i++)
				{
					nums_bones_per_vertex[i] = 0;
					bone_ids[i * 4 + 0] = bone_ids[i * 4 + 1] = bone_ids[i * 4 + 2] = bone_ids[i * 4 + 3] = -1;
					bone_weights[i * 4 + 0] = bone_weights[i * 4 + 1] = bone_weights[i * 4 + 2] = bone_weights[i * 4 + 3] = 0.0f;
				}


				for (unsigned int i = 0; i<mesh->mNumBones; i++)
				{
					for (unsigned int j = 0; j<mesh->mBones[i]->mNumWeights; j++)
					{
						int vertex_id = mesh->mBones[i]->mWeights[j].mVertexId;
						float weight = mesh->mBones[i]->mWeights[j].mWeight;

						// If there are more than four joints related at one vertex,
						// we just skip the fifth ane later one. 
						if (nums_bones_per_vertex[vertex_id] == 4)
						{
							continue;
						}


						int bone_id = i;
						if (in_bonename_to_pmjoint.find(mesh->mBones[i]->mName.C_Str()) != in_bonename_to_pmjoint.end())
						{
							bone_id = in_bonename_to_pmjoint.find(mesh->mBones[i]->mName.C_Str())->second;
						}
						bone_ids[vertex_id * 4 + nums_bones_per_vertex[vertex_id]] = bone_id;
						bone_weights[vertex_id * 4 + nums_bones_per_vertex[vertex_id]] = weight;

						nums_bones_per_vertex[vertex_id]++;
					}


				}

				vbo_group->SetVBO(GL_VBOGroup::BONE_ID_VBO, GL_INT, 4, bone_ids);
				vbo_group->SetVBO(GL_VBOGroup::BONE_WEIGHT_VBO, GL_DOUBLE, 4, bone_weights);


				delete[] nums_bones_per_vertex;
				delete[] bone_ids;
				delete[] bone_weights;
			}

			// Material
			{
				if (mesh->mMaterialIndex >= 0 && mesh->mMaterialIndex < ai_scene_->mNumMaterials)
				{
					aiMaterial *mat = ai_scene_->mMaterials[mesh->mMaterialIndex];

					//*out_material = CreateGlMaterial(mat);
					gl_material = CreateGlMaterial(mat);
				}
			}

			break;
		}
	}

	GL_RenderableObj *r_obj = GL_ResourceManager::singleton()->CreateRendarableObj(renderable_obj_name, vbo_group, gl_material);
	r_obj->flag_material(true);
	if ( flag_has_bone ) r_obj->flag_skinning(true);

	return r_obj;
}

};