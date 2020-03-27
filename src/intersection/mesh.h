#ifndef MESH_H
#define MESH_H

#include <iostream>
#include <string>
#include <vector>
#include "intersection/Intersection.h"

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
	Mesh(const string& filePath, int mId)
	{
		LoadFromFile(filePath);

		// Compute bbox
		for (auto& pos : m_positions)
		{
			m_aabb._min = glm::min(pos, m_aabb._min);
			m_aabb._max = glm::max(pos, m_aabb._max);
		}

		materialId = mId;
	}

	virtual bool Hit(const Ray& ray, float tmin, float tmax, Intersection& intersect) const override;

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