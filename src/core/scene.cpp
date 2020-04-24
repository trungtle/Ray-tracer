#include "core/scene.h"

#include <iostream>

#include "pbrtParser/Scene.h"

static std::string FileExtension(const std::string &filename) {
    if (filename.find_last_of(".") != std::string::npos)
        return filename.substr(filename.find_last_of(".") + 1);
    return "";
}

void Scene::LoadFromFile(const std::string& filename)
{
    std::string ext = FileExtension(filename);
    if (ext.compare("pbrt") == 0 || ext.compare("pbf") == 0)
    {
        pbrt::Scene::SP scene = pbrt::Scene::loadFrom(filename);
        
        std::cout << "Loading file " << filename << endl;
        std::cout << "Scene: " << endl;

        
        for (pbrt::Shape::SP shape : scene->world->shapes)
        {
            std::cout << "\tShape: " << shape->getNumPrims() << std::endl;
            pbrt::TriangleMesh::SP triangleMesh =
            dynamic_pointer_cast<pbrt::TriangleMesh>(shape);
            
            for (auto v : triangleMesh->vertex)
            {
                glm::vec3 pos(v.x, v.y, v.z);
//                m_positions.push_back(pos);
            }
            
            for (auto n : triangleMesh->normal)
            {
                glm::vec3 nor(n.x, n.y, n.z);
//                m_positions.push_back(nor);
            }
            
            for (int tri = 0; tri < triangleMesh->index.size(); tri++)
            {
                // Triangle comes in groups of 3
                pbrt::vec3i i3 = triangleMesh->index[tri];
//                m_indices.push_back(i3.x);
//                m_indices.push_back(i3.y);
//                m_indices.push_back(i3.z);
            }
            cout << endl;
        }
        
        pbrt::box3f bbox = scene->getBounds();
//        m_aabb._min = glm::vec3(
//                                bbox.lower.x,
//                                bbox.lower.y,
//                                bbox.lower.z);
//        m_aabb._max =  glm::vec3(
//                                 bbox.upper.x,
//                                 bbox.upper.y,
//                                 bbox.upper.z);
        std::cout << "Finished loading scene." << std::endl;
        
    }
}
