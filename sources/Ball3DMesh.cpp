/*
 * File:   Ball3DMesh.cpp
 * Author: b4n92uid
 *
 * Created on 25 novembre 2010, 17:45
 */

#include "Ball3DMesh.h"
#include "Tools.h"
#include "MeshParallelScene.h"

using namespace tbe;
using namespace scene;

Ball3DMesh::Ball3DMesh(MeshParallelScene* scene) : Mesh(scene)
{
}

Ball3DMesh::Ball3DMesh(MeshParallelScene* scene, const std::string& filepath) : Mesh(scene)
{
    open(filepath);
}

Ball3DMesh::Ball3DMesh(const Ball3DMesh& copy) : Mesh(copy)
{
    *this = copy;
}

Ball3DMesh::~Ball3DMesh()
{
}

Ball3DMesh & Ball3DMesh::operator=(const Ball3DMesh& copy)
{
    return *this;
}

Ball3DMesh* Ball3DMesh::clone()
{
    return new Ball3DMesh(*this);
}

void Ball3DMesh::readVertexs(std::ifstream& file)
{
    using namespace std;

    string buffer;
    while(getline(file, buffer))
    {
        if(buffer[0] == '#' || buffer.empty())
            break;

        Vertex v;

        stringstream parser(buffer);

        parser >> v.pos.x >> v.pos.y >> v.pos.z
                >> v.normal.x >> v.normal.y >> v.normal.z
                >> v.color.x >> v.color.y >> v.color.z >> v.color.w
                >> v.texCoord.x >> v.texCoord.y;

        m_hardwareBuffer->addVertex(v);
    }
}

void Ball3DMesh::readMaterial(std::ifstream& file, Material* mat)
{
    using namespace std;

    string buffer;
    while(getline(file, buffer))
    {
        if(buffer[0] == '#' || buffer.empty())
            break;

        unsigned assign = buffer.find(' ');

        string opcode(buffer, 0, assign), value(buffer, assign + 1);

        if(opcode == "alpha")
            mat->setAlphaThershold(tools::strToNum<float>(value));

        else if(opcode == "ambient")
            mat->setAmbient(Vector4f().fromStr(value, false));

        else if(opcode == "diffuse")
            mat->setDiffuse(Vector4f().fromStr(value, false));

        else if(opcode == "specular")
            mat->setSpecular(Vector4f().fromStr(value, false));

        else if(opcode == "shininess")
            mat->setShininess(tools::strToNum<float>(value));

        else if(opcode == "tex")
        {
            unsigned index = tools::strToNum<float>(value);
            string filepath = tools::pathScope(m_filename, value.substr(value.find(' ') + 1), true);

            mat->setTexture(Texture(filepath, true), index);
            mat->enable(Material::TEXTURED);
        }
    }
}

void Ball3DMesh::open(std::string filepath)
{
    using namespace std;

    ifstream file(filepath.c_str());

    if(!file)
        throw Exception("Ball3DMesh::Open; Open file error (%s)", filepath.c_str());

    string buffer;

    getline(file, buffer);

    if(buffer != ".ball3d.")
        throw Exception("Ball3DMesh::Open; Invalid file format (%s)", filepath.c_str());

    m_filename = filepath;
    m_name = tools::basename(filepath, false);

    Material* curMaterial;

    while(getline(file, buffer))
    {
        unsigned assign = buffer.find(' ');

        if(buffer[0] == '#' || buffer.empty())
            continue;

        string opcode(buffer, 0, assign), value(buffer, assign + 1);

        if(opcode == "material")
        {
            curMaterial = new Material;
            curMaterial->enable(Material::LIGHTED);
            curMaterial->enable(Material::COLORED);

            addMaterial(value, curMaterial);
            readMaterial(file, curMaterial);
        }

        else if(opcode == "build")
        {
            unsigned offset = m_hardwareBuffer->getVertexCount();
            readVertexs(file);
            unsigned size = m_hardwareBuffer->getVertexCount() - offset;

            applyMaterial(value, offset, size);
        }
    }

    m_hardwareBuffer->compile();

    computeAabb();

    file.close();
}

std::string Ball3DMesh::getFilename()
{
    return m_filename;
}

Node::CtorMap Ball3DMesh::constructionMap(std::string root)
{
    Node::CtorMap ctormap = Mesh::constructionMap(root);

    ctormap["class"] = "Ball3DMesh";

    ctormap["filename"] = tools::pathScope(root, m_filename, false);

    return ctormap;
}

std::vector<std::string> Ball3DMesh::getUsedRessources()
{
    using namespace std;

    vector<string> ressPath = Mesh::getUsedRessources();

    ressPath.push_back(m_filename);

    return ressPath;
}
