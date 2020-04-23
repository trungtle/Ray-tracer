#ifndef MESH_H
#define MESH_H

#include <iostream>
#include <string>
#include <vector>

#include "shapes/Intersection.h"

using namespace glm;
using namespace std;

namespace tinygltf
{
	class Model;
	class Node;
	struct Mesh;
	struct Primitive;
};

namespace mi {
class Mesh : public Hitable
{
public:
	// Args:
	// mId: Material ID
	Mesh(const string& filePath, int mId)
	{
		LoadFromFile(filePath);

		materialId = mId;
	}

	virtual ~Mesh();

	virtual bool Hit(const Ray& ray, float tmin, float tmax, Interaction& intersect) const override;

	virtual bool BoundingBox(AABB& aabb) const override
	{
		return false;
		aabb = m_aabb;
		return true;
	}

private:
	void LoadFromFile(const std::string& file);
	void ParseModel(const tinygltf::Model&);
	void ParseNode(const tinygltf::Model& model, const tinygltf::Node&);
	void ParseMesh(const tinygltf::Model& model, const tinygltf::Mesh&);
	void ParsePrimitive(const tinygltf::Model& model, const tinygltf::Primitive&);	

    vector<glm::vec3> m_positions;
    vector<glm::vec3> m_normals;
    vector<glm::vec4> m_colors;
    vector<unsigned int> m_indices;	
    AABB m_aabb;
};
};
#endif
