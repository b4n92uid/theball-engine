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
    Open(filepath);
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

void Ball3DMesh::ReadVertexs(std::ifstream& file)
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

        m_hardwareBuffer.AddVertex(v);
    }
}

void Ball3DMesh::ReadMaterial(std::ifstream& file, Material* mat)
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
            mat->SetAlphaThershold(tools::StrToNum<float>(value));

        else if(opcode == "ambient")
            mat->SetAmbient(tools::StrToVec4<float>(value));

        else if(opcode == "diffuse")
            mat->SetDiffuse(tools::StrToVec4<float>(value));

        else if(opcode == "specular")
            mat->SetSpecular(tools::StrToVec4<float>(value));

        else if(opcode == "shininess")
            mat->SetShininess(tools::StrToNum<float>(value));

        else if(opcode == "tex")
        {
            unsigned index = tools::StrToNum<float>(value);
            string filepath = tools::makeRelatifTo(m_filename, value.substr(value.find(' ') + 1));

            mat->SetTexture(Texture(filepath, true), index);
            mat->Enable(Material::TEXTURE);
        }
    }
}

void Ball3DMesh::Open(std::string filepath)
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
            curMaterial->Enable(Material::LIGHT);
            curMaterial->Enable(Material::COLOR);

            AddMaterial(value, curMaterial);
            ReadMaterial(file, curMaterial);
        }

        else if(opcode == "build")
        {
            unsigned offset = m_hardwareBuffer.GetVertexCount();
            ReadVertexs(file);
            unsigned size = m_hardwareBuffer.GetVertexCount() - offset;

            ApplyMaterial(value, offset, size);
        }
    }

    m_hardwareBuffer.Compile();

    ComputeAabb();

    file.close();
}

std::string Ball3DMesh::GetFilename()
{
    return m_filename;
}

Node::CtorMap Ball3DMesh::ConstructionMap()
{
    Node::CtorMap ctormap = Mesh::ConstructionMap();

    ctormap["class"] = "Ball3DMesh";

    ctormap["filename"] = m_filename;

    return ctormap;
}
