#include "ObjMesh.h"
#include "Tools.h"

#include <fstream>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace tbe;
using namespace tbe::scene;

OBJMesh::OBJMesh(MeshParallelScene* scene) : Mesh(scene), m_mtlfile(this) { }

OBJMesh::OBJMesh(MeshParallelScene* scene, const std::string& path) : Mesh(scene), m_mtlfile(this)
{
    open(path);
}

OBJMesh::OBJMesh(const OBJMesh& copy) : Mesh(copy), m_mtlfile(this)
{
    this->copy(copy);
}

OBJMesh& OBJMesh::operator=(const OBJMesh& org)
{
    Mesh::operator=(org);

    return this->copy(org);
}

OBJMesh& OBJMesh::copy(const OBJMesh& org)
{
    m_filename = org.m_filename;
    m_mtlfile = org.m_mtlfile;

    return *this;
}

OBJMesh* OBJMesh::clone()
{
    return new OBJMesh(*this);
}

OBJMesh::~OBJMesh() { }

void OBJMesh::open(const std::string& path)
{
    Mesh* shared = Mesh::isSharedBuffer(path);

    if(shared)
    {
        fetchVertexes(*shared);

        computeAabb();

        m_filename = path;
        m_name = tools::basename(path, false);

        Mesh::registerBuffer(this, path);

        return;
    }

    cout << "[OBJ Mesh] " << path << endl;

    ifstream file(path.c_str(), ios::in);

    if(!file)
        throw tbe::Exception("OBJMesh::open; Open OBJ File Error; (%1%)") % path;

    if(!m_hardwareBuffer)
        m_hardwareBuffer = new HardwareBuffer;
    else
        m_hardwareBuffer->clear();

    m_filename = path;
    m_name = tools::basename(path, false);

    Mesh::registerBuffer(this, path);

    Material* curMaterial = NULL;

    Vector3f::Array vPos;
    Vector3f::Array vNormal;
    Vector2f::Array vTexCoord;

    unsigned applySize = 0;
    unsigned applyOffset = 0;

    string buffer;
    for(unsigned lineCount = 1; tools::getline(file, buffer); lineCount++)
    {
        for(unsigned i = 0; i < buffer.size(); i++)
            if(buffer[i] <= 0)
                throw tbe::Exception("OBJMesh::open; File is binary; (%1%)") % path;

        if(buffer[0] == '#' || buffer.empty())
            continue;

        string opcode = buffer.substr(0, buffer.find_first_of(' '));
        string value = buffer.substr(buffer.find_first_of(' ') + 1);

        if(opcode == "mtllib")
        {
            string mtlFilename = tools::resolvePath(value, m_filename);
            m_mtlfile.open(mtlFilename);
        }

            // object define
        else if(opcode == "o");

            // groupe define
        else if(opcode == "g");

            // smooth enable
        else if(opcode == "s");

        else if(opcode == "v")
            vPos.push_back(Vector3f().fromStr(value, false));

        else if(opcode == "vn")
            vNormal.push_back(Vector3f().fromStr(value, false));

        else if(opcode == "vt")
            vTexCoord.push_back(Vector2f().fromStr(value, false));

        else if(opcode == "usemtl")
        {
            if(curMaterial)
            {
                if(!vTexCoord.empty())
                    curMaterial->enable(Material::TEXTURED);

                if(!vNormal.empty())
                    curMaterial->enable(Material::LIGHTED);

                curMaterial->enable(Material::COLORED);

                if(applySize % 3 != 0)
                    cout << "/!\\ WARNING: Mesh may not be triangulated (" << curMaterial->getName() << ")" << endl;

                applyMaterial(curMaterial, applyOffset, applySize);
            }

            applyOffset += applySize;
            applySize = 0;

            try
            {
                curMaterial = getMaterial(value);
            }

            catch(...)
            {
                curMaterial = new Material;
                addMaterial("", curMaterial);
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
                if(!vTexCoord.empty())
                {
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

                }
                // Normal

                if(!vNormal.empty())
                {
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
                }

                newFace.push_back(vert);
            }

            m_hardwareBuffer->addFace(newFace);

            applySize += newFace.size();
        }

        else
            cout << "OBJMesh::Open; Warning : line (" << lineCount << ") ignored" << endl;
    }

    if(m_hardwareBuffer->isEmpty())
        throw tbe::Exception("OBJMesh::open; File is invalid; Buffer is empty; (%1%)") % path;

    if(curMaterial)
    {
        if(!vTexCoord.empty())
            curMaterial->enable(Material::TEXTURED);

        if(!vNormal.empty())
            curMaterial->enable(Material::LIGHTED);

        curMaterial->enable(Material::COLORED);

        if(applySize % 3 != 0)
            cout << "/!\\ WARNING: Mesh may not be triangulated (" << curMaterial->getName() << ")" << endl;

        applyMaterial(curMaterial, applyOffset, applySize);
    }

    m_hardwareBuffer->compile();

    file.close();

    computeAabb();
}

std::string OBJMesh::getFilename() const
{
    return m_filename;
}

rtree OBJMesh::serialize(std::string root)
{
    using boost::filesystem::absolute;
    rtree scheme = Mesh::serialize(root);

    scheme.put("class.format", "obj");
    scheme.put("class.path", absolute(m_filename, root));

    return scheme;
}

std::vector<std::string> OBJMesh::getUsedRessources()
{
    using namespace std;

    vector<string> ressPath = Mesh::getUsedRessources();

    ressPath.push_back(m_filename);
    ressPath.push_back(m_mtlfile.getFilename());

    return ressPath;
}

// MTLFile ---------------------------------------------------------------------

MTLFile::MTLFile(OBJMesh * parent)
{
    m_parent = parent;
}

MTLFile::MTLFile(OBJMesh* parent, const std::string & path)
{
    m_parent = parent;

    open(path);
}

MTLFile::~MTLFile() { }

MTLFile::MTLFile(const MTLFile& copy)
{
    *this = copy;
}

MTLFile& MTLFile::operator=(const MTLFile& copy)
{
    m_filename = copy.m_filename;

    return *this;
}

void MTLFile::open(const std::string& path)
{
    ifstream file(path.c_str());

    if(!file)
        throw tbe::Exception("MTLFile::Open; Open MTL File Error; (%1%)") % path;

    cout << "[MTL File] " << path << endl;

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

            m_parent->addMaterial(arg, material);
        }

        else if(opcode == "Ns")
        {
            // Evite de lire la valeur Ns pour la non-standarisation du format
            // le shininess doit etre spécifier manuellement
        }

        else if(opcode == "Ka")
        {
            // Vector4f ka;
            // sscanf(arg.c_str(), "%f %f %f", &ka.x, &ka.y, &ka.z);
            // material->setAmbient(ka);
        }

        else if(opcode == "Kd")
        {
            // Vector4f kd;
            // sscanf(arg.c_str(), "%f %f %f", &kd.x, &kd.y, &kd.z);
            // material->setDiffuse(kd);
        }

        else if(opcode == "Ks")
        {
            // Vector4f ks;
            // sscanf(arg.c_str(), "%f %f %f", &ks.x, &ks.y, &ks.z);
            // material->setSpecular(ks);
        }

        else if(opcode == "Ni")
        {
            // Refraction index
        }

        else if(opcode == "d")
        {
            // Valeur alpha du materiaux
            try
            {
                float alpha = boost::lexical_cast<float>(arg);

                if(math::isZero(alpha))
                {
                    material->enable(Material::ALPHA);
                    material->setAlphaThershold(0.5);
                }
            }

            catch(tbe::Exception& e)
            {
                cout << e.what() << endl;
            }
        }

        else if(opcode == "illum")
        {
            // Mode d'éclairage :
            // 0 : pas d'éclairage
            // 1 : éclairage ambiant et diffuse
            // 2 : éclairage ambiant, diffuse et spéculaire
        }

        else if(opcode == "map_Kd")
        {
            string texturepath = tools::resolvePath(arg, path);

            try
            {
                Texture tex(texturepath, true, false);

                material->setTexture(tex);
            }

            catch(tbe::Exception& e)
            {
                cout << e.what() << endl;
            }
        }

        else
        {
            cout << "/!\\ WARNING: MTLFile::Open; line (" << line << ") ignored" << endl;
        }

    }

    file.close();
}

std::string MTLFile::getFilename() const
{
    return m_filename;
}
