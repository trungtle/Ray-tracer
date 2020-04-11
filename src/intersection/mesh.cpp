#include "mesh.h"

#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include "pbrtParser/Scene.h"
#include "intersection/Triangle.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
// #define TINYGLTF_NOEXCEPTION // optional. disable exception handling.

#include "tiny_gltf.h"

using namespace tinygltf;
using namespace glm;
using namespace std;

static std::unordered_map<int, int> GLTF_COMPONENT_LENGTH_LOOKUP = {
	{ TINYGLTF_TYPE_SCALAR, 1 },
	{ TINYGLTF_TYPE_VEC2, 2 },
	{ TINYGLTF_TYPE_VEC3, 3 },
	{ TINYGLTF_TYPE_VEC4, 4 },
	{ TINYGLTF_TYPE_MAT2, 4 },
	{ TINYGLTF_TYPE_MAT3, 9 },
	{ TINYGLTF_TYPE_MAT4, 16 }
};

static std::unordered_map<int, int> GLTF_COMPONENT_BYTE_SIZE_LOOKUP = {
	{ TINYGLTF_COMPONENT_TYPE_BYTE , 1 },
	{ TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE, 1 },
	{ TINYGLTF_COMPONENT_TYPE_SHORT, 2 },
	{ TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT, 2 },
	{ TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT, 4 },
	{ TINYGLTF_COMPONENT_TYPE_FLOAT, 4 }
};

std::string FileExtension(const std::string &FileName) {
 	if (FileName.find_last_of(".") != std::string::npos)
   		return FileName.substr(FileName.find_last_of(".") + 1);
 	return "";
}

bool mi::Mesh::Hit(const Ray& ray, float tmin, float tmax, Intersection& intersect) const
{
	bool isHit = false;
	float minT = 1000000;
	for (size_t i = 0; i < m_indices.size(); i+=3)
	{
		if (i / 3 > 10950) break;
		
		const vec3& p0 = m_positions[i * 3];
		const vec3& p1 = m_positions[i * 3 + 1];
		const vec3& p2 = m_positions[i * 3 + 2];

		Intersection triIsect;
		if (Triangle::RayTriangleIntersect(
			ray, 
			p0, 
			p1,
			p2,
			tmin, 
			tmax, 
			triIsect)
			)
		{
			isHit = true;
			if (triIsect.t < minT)
			{
				minT = triIsect.t;
				intersect = triIsect;
				intersect.hit = this;
			}
		}
		// cout << "hit " << i / 3 << "/" << m_indices.size() / 3 << endl;
	}

	return isHit;
}	

void mi::Mesh::LoadFromFile(const std::string& file)
{
	tinygltf::Model model;
	tinygltf::TinyGLTF gltf_ctx;
	std::string err;
	std::string warn;
	std::string ext = FileExtension(file);

	bool ret = false;
	if (ext.compare("pbrt") == 0 || ext.compare("pbf") == 0)
	{
		pbrt::Scene::SP scene = 
		pbrt::Scene::loadFrom(file);

		// DEBUG
		cout << "Loaded file " << file << endl;
		cout << "Scene: " << endl;

		for (pbrt::Shape::SP shape : scene->world->shapes)
		{
			cout << "\tShape: " << shape->getNumPrims() << endl;
			pbrt::TriangleMesh::SP triangleMesh = 
				dynamic_pointer_cast<pbrt::TriangleMesh>(shape);

			for (auto v : triangleMesh->vertex)
			{
				glm::vec3 pos(v.x, v.y, v.z);
				m_positions.push_back(pos);
			}

			for (auto n : triangleMesh->normal)
			{
				glm::vec3 nor(n.x, n.y, n.z);
				m_positions.push_back(nor);
			}

			for (int tri = 0; tri < triangleMesh->index.size(); tri++)
			{
				// Triangle comes in groups of 3
				pbrt::vec3i i3 = triangleMesh->index[tri];
				m_indices.push_back(i3.x);
				m_indices.push_back(i3.y);
				m_indices.push_back(i3.z);
			}			
			cout << endl;
		}

		pbrt::box3f bbox = scene->getBounds();
		m_aabb._min = glm::vec3(
			bbox.lower.x,
			bbox.lower.y,
			bbox.lower.z);
		m_aabb._max =  glm::vec3(
			bbox.upper.x,
			bbox.upper.y,
			bbox.upper.z);
		cout << "Finished loading scene." << endl;
		ret = true;
	}
	else if (ext.compare("glb") == 0) {
		// assume binary glTF.
		ret = gltf_ctx.LoadBinaryFromFile(&model, &err, &warn,
	                                  file.c_str());
	} else {
		// assume ascii glTF.
		ret = gltf_ctx.LoadASCIIFromFile(&model, &err, &warn, file.c_str());
	}

	if (!ret)
	{
		throw runtime_error("Failed to parse gltf\n");
	}

	ParseModel(model);

	// Compute bbox
	// for (auto& pos : m_positions)
	// {
	// 	m_aabb._min = glm::min(pos, m_aabb._min);
	// 	m_aabb._max = glm::max(pos, m_aabb._max);
	// }
}

mi::Mesh::~Mesh()
{

}

void mi::Mesh::ParseModel(const tinygltf::Model& model)
{
	for (const tinygltf::Scene& scene : model.scenes)
	{
		for (int nodeId : scene.nodes)
		{
			ParseNode(model, model.nodes[nodeId]);
		}
	}	
}

void mi::Mesh::ParseNode(const tinygltf::Model& model, const tinygltf::Node& node)
{
	if (node.mesh != -1)
	{
		ParseMesh(model, model.meshes[node.mesh]);
	}

	for (int childNodeId : node.children)
	{
		ParseNode(model, model.nodes[childNodeId]);
	}	
}

void mi::Mesh::ParseMesh(const tinygltf::Model& model, const tinygltf::Mesh& mesh)
{
	for (const tinygltf::Primitive& primitive : mesh.primitives)
	{
		ParsePrimitive(model, primitive);
	}
}

void mi::Mesh::ParsePrimitive(const tinygltf::Model& model, const tinygltf::Primitive& primitive)
{
	// Parse indices
	const auto& indicesAccessor = model.accessors[primitive.indices];
	const auto& indicesBufferView = model.bufferViews[indicesAccessor.bufferView];
	const auto& indicesBuffer = model.buffers[indicesBufferView.buffer];
	size_t count = indicesAccessor.count;
	int componentLen = GLTF_COMPONENT_LENGTH_LOOKUP[indicesAccessor.type];
	int componentByteSize = GLTF_COMPONENT_BYTE_SIZE_LOOKUP[indicesAccessor.componentType];
	size_t byteLength = indicesBufferView.byteLength;
	size_t byteOffset = indicesBufferView.byteOffset + indicesAccessor.byteOffset;
	
	m_indices.resize(count);
	memcpy(&m_indices[0], &indicesBuffer.data[byteOffset], byteLength);

	// Parse other attributes
	for (auto it = primitive.attributes.begin(); it != primitive.attributes.end(); it++)		
	{
		const string& attribName = it->first;
		int attribAccessorId = it->second;

		const auto& accessor = model.accessors[attribAccessorId];
		const auto& bufferView = model.bufferViews[accessor.bufferView];
		const auto& buffer = model.buffers[bufferView.buffer];
		componentLen = GLTF_COMPONENT_LENGTH_LOOKUP[accessor.type];
		componentByteSize = GLTF_COMPONENT_BYTE_SIZE_LOOKUP[accessor.componentType];

		byteLength = bufferView.byteLength;
		byteOffset = bufferView.byteOffset + accessor.byteOffset;
		count = accessor.count;

		if (attribName.compare("POSITION") == 0)
		{
			m_positions.resize(count);
			memcpy(&m_positions[0], &buffer.data[byteOffset], byteLength);
		}
		if (attribName.compare("NORMAL") == 0)
		{
			m_normals.resize(count);
			memcpy(&m_normals[0], &buffer.data[byteOffset], byteLength);
		}
		if (attribName.compare("TANGENT") == 0)
		{

		}
		if (attribName.compare("TEXCOORD_0") == 0)
		{

		}
	}
}