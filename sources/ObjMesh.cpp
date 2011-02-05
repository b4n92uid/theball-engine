#include "ObjMesh.h"
#include "Tools.h"

#include <fstream>

using namespace std;
using namespace tbe;
using namespace tbe::scene;

static class SharedlObjMeshManager : public map<OBJMesh*, string>
{
public:

    OBJMesh* IsExist(string path)
    {
        for(iterator itt = begin(); itt != end(); itt++)
            if(itt->second == path)
                return itt->first;

        return NULL;
    }
} manager;

OBJMesh::OBJMesh(MeshParallelScene* scene) : Mesh(scene), m_mtlfile(this)
{
}

OBJMesh::OBJMesh(MeshParallelScene* scene, const std::string& path) : Mesh(scene), m_mtlfile(this)
{
    Open(path);
}

OBJMesh::OBJMesh(const OBJMesh& copy) : Mesh(copy), m_mtlfile(this)
{
    *this = copy;
}

OBJMesh& OBJMesh::operator=(const OBJMesh& copy)
{
    Mesh::operator=(copy);

    m_filename = copy.m_filename;
    m_mtlfile = copy.m_mtlfile;

    return *this;
}

OBJMesh::~OBJMesh()
{
}

void OBJMesh::Open(const std::string& path)
{
    cout << "Load obj mesh file : " << path << endl;

    ifstream file(path.c_str());

    if(!file)
        throw tbe::Exception("OBJMesh::Open; Open OBJ File Error; (%s)", path.c_str());

    manager[this] = m_filename = path;

    m_name = tools::basename(path, false);

    Material* curMaterial = NULL;

    Vector3f::Array vPos;
    Vector3f::Array vNormal;
    Vector2f::Array vTexCoord;

    unsigned applySize = 0;
    unsigned applyOffset = 0;

    string buffer;
    for(unsigned lineCount = 1; tools::getline(file, buffer); lineCount++)
    {
        if(buffer[0] == '#' || buffer.empty())
            continue;

        string opcode = buffer.substr(0, buffer.find_first_of(' '));
        string value = buffer.substr(buffer.find_first_of(' ') + 1);

        if(opcode == "mtllib")
        {
            string mtlFilename = tools::makeRelatifTo(m_filename, value);
            m_mtlfile.Open(mtlFilename);
        }

            // object define
        else if(opcode == "o");

            // groupe define
        else if(opcode == "g");

            // smooth enable
        else if(opcode == "s");

        else if(opcode == "v")
            vPos.push_back(tools::StrToVec3<float>(value));

        else if(opcode == "vn")
            vNormal.push_back(tools::StrToVec3<float>(value));

        else if(opcode == "vt")
            vTexCoord.push_back(tools::StrToVec2<float>(value));

        else if(opcode == "usemtl")
        {
            if(curMaterial)
            {
                if(!vTexCoord.empty())
                    curMaterial->Enable(Material::TEXTURE);

                if(!vNormal.empty())
                    curMaterial->Enable(Material::LIGHT);

                curMaterial->Enable(Material::COLOR);

                ApplyMaterial(curMaterial, applyOffset, applySize);
            }

            applyOffset += applySize;
            applySize = 0;

            try
            {
                curMaterial = GetMaterial(value);
            }

            catch(...)
            {
                curMaterial = new Material;
                AddMaterial("", curMaterial);
            }
        }

        else if(opcode == "f")
        {
            Face newFace;

            istringstream exp(value);

            while(!exp.eof())
            {
                Vertex vert;

                vert.color = 1;

                unsigned index = 0;
                char sep = 0;

                exp >> index;
                vert.pos = vPos[index - 1];

                // TexCoord

                exp >> sep;
                exp >> index;

                if(exp.fail())
                {
                    m_withTexCoord = false;
                    exp.clear();
                }

                else
                {
                    m_withTexCoord = true;
                    vert.texCoord = vTexCoord[index - 1];

                }

                // Normal

                exp >> sep;
                exp >> index;

                if(exp.fail())
                {
                    m_withNormal = false;
                    exp.clear();
                }

                else
                {
                    vert.normal = vNormal[index - 1];
                    m_withNormal = true;
                }

                newFace.push_back(vert);
            }

            m_hardwareBuffer.AddFace(newFace);

            applySize += newFace.size();
        }

        else
            cout << "OBJMesh::Open; Warning : line (" << lineCount << ") ignored" << endl;
    }

    if(curMaterial)
    {
        if(!vTexCoord.empty())
            curMaterial->Enable(Material::TEXTURE);

        if(!vNormal.empty())
            curMaterial->Enable(Material::LIGHT);

        curMaterial->Enable(Material::COLOR);

        ApplyMaterial(curMaterial, applyOffset, applySize);
    }

    m_hardwareBuffer.Compile();

    file.close();

    ComputeAabb();
}

std::string OBJMesh::GetFilename() const
{
    return m_filename;
}

Node::CtorMap OBJMesh::ConstructionMap(std::string root)
{
    Node::CtorMap ctormap = Mesh::ConstructionMap(root);

    ctormap["class"] = "OBJMesh";

    ctormap["filename"] = tools::makeRelatifTo(root, m_filename);

    return ctormap;
}

// MTLFile ---------------------------------------------------------------------

MTLFile::MTLFile(OBJMesh * parent)
{
    m_parent = parent;
}

MTLFile::MTLFile(OBJMesh* parent, const std::string & path)
{
    m_parent = parent;

    Open(path);
}

MTLFile::~MTLFile()
{
}

MTLFile::MTLFile(const MTLFile& copy)
{
    *this = copy;
}

MTLFile& MTLFile::operator=(const MTLFile& copy)
{
    m_filename = copy.m_filename;

    return *this;
}

void MTLFile::Open(const std::string& path)
{
    ifstream file(path.c_str());

    if(!file)
        throw tbe::Exception("MTLFile::Open; Open MTL File Error; (%s)", path.c_str());

    cout << "Load mtl file : " << path << endl;

    m_filename = path;

    Material * material = NULL;

    string buffer;
    for(unsigned line = 1; tools::getline(file, buffer); line++)
    {
        if(buffer[0] == '#' || buffer.empty())
            continue;

        int pos = buffer.find_first_of(' ');

        string opcode(buffer, 0, pos),
                arg(buffer, pos + 1, string::npos);

        if(opcode == "newmtl")
        {
            material = new Material;

            m_parent->AddMaterial(arg, material);
        }

        else if(opcode == "Ns")
        {
            // Evite de lire la valeur Ns pour la non-standarisation du format
            // le shininess doit etre sp�cifier manuellement
        }

        else if(opcode == "Ka")
        {
            Vector4f ka;
            sscanf(arg.c_str(), "%f %f %f", &ka.x, &ka.y, &ka.z);
            material->SetAmbient(ka);
        }

        else if(opcode == "Kd")
        {
            Vector4f kd;
            sscanf(arg.c_str(), "%f %f %f", &kd.x, &kd.y, &kd.z);
            material->SetDiffuse(kd);
        }

        else if(opcode == "Ks")
        {
            Vector4f ks;
            sscanf(arg.c_str(), "%f %f %f", &ks.x, &ks.y, &ks.z);
            material->SetSpecular(ks);
        }

        else if(opcode == "Ni")
        {
            // Refraction index
        }

        else if(opcode == "d")
        {
            // Valeur alpha du materiaux
        }

        else if(opcode == "illum")
        {
            // Mode d'�clairage :
            // 0 : pas d'�clairage
            // 1 : �clairage ambiant et diffuse
            // 2 : �clairage ambiant, diffuse et sp�culaire
        }

        else if(opcode == "map_Kd")
        {
            string texturepath = tools::makeRelatifTo(path, arg);

            try
            {
                Texture tex(texturepath, true, false);

                material->SetTexture(tex);
            }

            catch(tbe::Exception& e)
            {
                cout << e.what() << endl;
            }
        }

        else
        {
            cout << "MTLFile::Open; Warning : line (" << line << ") ignored" << endl;
        }

    }

    file.close();
}

std::string MTLFile::GetFilename() const
{
    return m_filename;
}
