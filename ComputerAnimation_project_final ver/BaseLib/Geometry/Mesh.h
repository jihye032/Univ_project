
#pragma once

#include "BaseLib/CmlExt/CmlExt.h"
#include "BaseLib/Geometry/AABox.h"
#include <stdarg.h>
#include <string>

///////////////////////////////////////////
// Class: Mesh
// This class represents general geometry object as a mesh model.

namespace mg
{

class Mesh
{
public:
	Mesh();
	Mesh(const Mesh &in_mesh);
	~Mesh();
	void Clear();

	// Create Premitives.
	void CreateQuad(double width=1, double depth=1);
	void CreateBox(double width=1, double height=1, double depth=1);
	void CreateCylinder(cml::vector3d bottom_center, cml::vector3d top_center, double radius=1, int slices=32, int stacks=1);
	void CreateCylinder(double height=1, double radius=1, int slices=32, int stacks=1);
	void CreateOpenedCylinder(double height=1, double radius=1, int slices=32, int stacks=1);
	void CreateSphere(double radius=1);
	void CreateHemisphere(double radius=1);
	void CreateCapsule(cml::vector3d bottom_center, cml::vector3d top_center, double radius=1, int slices=32, int stacks=1);
	void CreateCapsule(double cylinder_height=1, double radius=1);
	void CreateHeadModel(double radius=1);

	// Transform.
	void TranslateVertices(cml::vector3d t);
	void RotateVertices(cml::quaterniond q, cml::vector3d center=cml::vector3d(0, 0, 0));
	void ScaleUniformlyVertices(double s, cml::vector3d center=cml::vector3d(0, 0, 0));
	void ScaleVertices(cml::vector3d s, cml::vector3d center=cml::vector3d(0, 0, 0));

	// Assign, Merge
	void Assign(const Mesh& mesh);
	void Merge(const Mesh& mesh);

	// Calculate Normal Vector.
	cml::vector3d CalculFaceNormal(int face_id) const;
	cml::vector3d CalculVertexNormal(int vertex_id) const;
	void UpdateNormalVectorsBasedOnFace();
	void UpdateNormalVectorsBasedOnVertex();

	//
	void Trangulate();

	// Axis Aligned Bounding Box
	AABox CalculAABB() const;

	// Get Properties. 
	inline int num_vertices() const { return num_vertices_; }
	inline int num_uvs() const { return num_uvs_; }
	inline int num_normals() const { return num_normals_; }
	inline int num_faces() const { return num_faces_; }
	
	inline double*  vertices() const { return vertices_; }
	inline double*  uvs() const { return uvs_; }
	inline double*  normals() const { return normals_; }
	inline int*     face_sizes() const { return face_sizes_; }
	inline int**    vertices_of_faces() const { return face_vertex_ids_; }
	inline int**    uvs_of_faces() const { return face_uv_ids_; }
	inline int**    normals_of_faces() const { return face_normal_ids_; }

	inline std::pair<double, double> uv(int id) const { return std::make_pair(uvs_[id*2+0], uvs_[id*2+1]); }
	inline cml::vector3d vertex(int id)              const { return cml::vector3d(vertices_[id*3+0], vertices_[id*3+1], vertices_[id*3+2]); }
	inline cml::vector3d normal(int id)              const { return cml::vector3d(normals_[id*3+0], normals_[id*3+1], normals_[id*3+2]); }

	inline int        face_size(int face_id)          const { return face_sizes_[face_id]; }
	inline const int* face_vertex_ids(int face_id) const { return face_vertex_ids_[face_id]; }
	inline const int* face_uv_ids(int face_id)     const { return face_uv_ids_[face_id]; }
	inline const int* face_normal_ids(int face_id) const { return face_normal_ids_[face_id]; }

	inline bool          has_bone() const { return flag_has_bone_; }
	inline int           max_bone_num_per_vertex() const { return max_bone_num_per_vertex_; }
	inline const int*    bone_ids(int vertex_id) const { return bone_ids_[vertex_id]; }
	inline const double* bone_weights(int vertex_id) const { return bone_weights_[vertex_id]; }

	// Set Properties. 
	inline void vertex(int id, const cml::vector3d &v) { vertices_[id*3+0]=v[0]; vertices_[id*3+1]=v[1]; vertices_[id*3+2]=v[2]; }
	inline void normal(int id, const cml::vector3d &n) { normals_[id*3+0]=n[0]; normals_[id*3+1]=n[1]; normals_[id*3+2]=n[2]; }
	inline void uv(int id, double u, double v) { uvs_[id*2+0]=u; uvs_[id*2+1]=v; }
	void SetAllNormals(cml::vector3d unified_normal);
	void SetAllUVs(double u, double v);
	
	void UseBoneWeight(bool f=true);
	void UnuseBoneWeight();
	
	// Set 'i'th bone_id and weight for 'vertex_id'th vertex.
	inline void SetBoneIdAndWeight(int vertex_id, int i, int bone_id, double w) { bone_ids_[vertex_id][i] = bone_id; bone_weights_[vertex_id][i] = w; }
	
	// Set 'i'th bone_id for all vertices
	void SetBoneIdAndWeightForAllVertices(int i, int bone_id, double w);


	// File IO
	void WriteObjFormatStream(std::ostream &out, std::string group_name="", int v_offset=0, int vt_offset=0, int vn_offset=0) const;
	void WriteObjFormatStreamV(std::ostream &out) const;	// Vertices.
	void WriteObjFormatStreamVT(std::ostream &out) const;	// UVs.
	void WriteObjFormatStreamVN(std::ostream &out) const;	// Normals.
	void WriteObjFormatStreamG(std::ostream &out, std::string group_name) const;	// Group name.
	void WriteObjFormatStreamF(std::ostream &out, int v_offset=0, int vt_offset=0, int vn_offset=0) const;		// Faces.
	void ReadObjFormatStream(std::istream &in);
	void ReadObjFormatFile(std::string file);

private:
	void SetZero();

	// Parameter: face_id, 1'st vertex id, 1'st normal id, ..., n'th vertex id, n'th normal id
	//            n is the number of points in the face (face_sizes_[face_id]).
	void SetFaceVertexAndNormalIds(int face_id, ...);

	// Set and Allocate
	void SetVertexUvNormalFaceSizes(int num_vertices, int num_uvs, int num_normals, int num_faces);

protected:
	int num_vertices_;
	int num_uvs_;
	int num_normals_;
	int num_faces_;

	double *vertices_;
	double *uvs_;
	double *normals_;
	int *face_sizes_;
	int **face_vertex_ids_;
	int **face_uv_ids_;
	int **face_normal_ids_;

	// For skinning. 
	// A vertex can be connected up to 'max_bone_num_per_vertex_' boneds.
	// If flag_has_bone_ is true, bone_ids_'s size (and bone_weight's size) must be num_vertices_*max_bone_num_per_vertex_
	bool flag_has_bone_;
	static const int max_bone_num_per_vertex_ = 4;
	int** bone_ids_;
	double** bone_weights_;
};


};