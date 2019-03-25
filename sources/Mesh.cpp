/*
 * File:   Mesh.cpp
 * Author: b4n92uid
 *
 * Created on 28 avril 2010, 18:36
 */

#include "Mesh.h"

#include <functional>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/optional/optional.hpp>

#include "ParallelScene.h"
#include "SceneManager.h"
#include "MeshParallelScene.h"
#include "Tools.h"
#include "ShadowMap.h"
#include "AbstractParser.h"

using namespace tbe;
using namespace tbe::scene;
using namespace std;

static class SharedlObjMeshManager : public map<Mesh*, string>
{
public:

    Mesh* shared(string source)
    {
        for(iterator it = begin(); it != end(); it++)
            if(it->second == source)
                return it->first;

        return NULL;
    }

    bool used(HardwareBuffer* hb)
    {
        for(iterator it = begin(); it != end(); it++)
            if(it->first->getHardwareBuffer() == hb)
                return true;

        return false;
    }

} _BufferManager;

Mesh::Mesh(MeshParallelScene* scene)
{
    m_triangulate = true;
    m_withNormal = false;
    m_withTexCoord = false;
    m_visible = true;
    m_billBoard = false;
    m_priorityRender = 0;
    m_receiveShadow = true;
    m_castShadow = true;
    m_computeNormals = false;
    m_computeTangent = false;
    m_computeAocc = false;

    Node::m_parallelScene = m_parallelScene = scene;

    m_sceneManager = m_parallelScene->getSceneManager();

    m_parallelScene->registerNode(this);

    m_hardwareBuffer = NULL;
}

Mesh::Mesh(const Mesh& copy) : Node(copy)
{
    this->copy(copy);

    m_parallelScene->registerNode(this);
}

Mesh::~Mesh()
{
    clear();

    m_parallelScene->unregisterNode(this);
}

Mesh& Mesh::operator=(const Mesh& copy)
{
    Node::operator=(copy);

    return this->copy(copy);
}

void Mesh::clear()
{
    BOOST_FOREACH(SubMesh* s, m_subMeshs)
            delete s;

    m_subMeshs.clear();

    Mesh::unregisterBuffer(this);

    if(!Mesh::isUsedBuffer(m_hardwareBuffer))
        delete m_hardwareBuffer, m_hardwareBuffer = NULL;

    m_aabb.clear();
}

void Mesh::fetchMaterials(const Mesh& copy)
{
    m_subMeshs.reserve(copy.m_subMeshs.size());

    for(unsigned i = 0; i < copy.m_subMeshs.size(); i++)
    {
        SubMesh* sm = new SubMesh(copy.m_subMeshs[i]->m_name,
                                  this,
                                  copy.m_subMeshs[i]->m_material,
                                  copy.m_subMeshs[i]->m_offset,
                                  copy.m_subMeshs[i]->m_size);

        m_subMeshs.push_back(sm);
    }
}

void Mesh::fetchVertexes(const Mesh& copy)
{
    m_triangulate = copy.m_triangulate;
    m_withNormal = copy.m_withNormal;
    m_withTexCoord = copy.m_withTexCoord;
    m_computeNormals = copy.m_computeNormals;
    m_computeTangent = copy.m_computeTangent;
    m_computeAocc = copy.m_computeAocc;

    m_hardwareBuffer = new HardwareBuffer(*copy.m_hardwareBuffer);

    fetchMaterials(copy);

    computeAabb();
}

void Mesh::shareVertexes(const Mesh& copy)
{
    m_triangulate = copy.m_triangulate;
    m_withNormal = copy.m_withNormal;
    m_withTexCoord = copy.m_withTexCoord;
    m_visible = copy.m_visible;
    m_billBoard = copy.m_billBoard;
    m_priorityRender = copy.m_priorityRender;
    m_receiveShadow = copy.m_receiveShadow;
    m_castShadow = copy.m_castShadow;
    m_aabb = copy.m_aabb;
    m_computeNormals = copy.m_computeNormals;
    m_computeTangent = copy.m_computeTangent;
    m_computeAocc = copy.m_computeAocc;

    m_hardwareBuffer = copy.m_hardwareBuffer;

    fetchMaterials(copy);
}

Mesh& Mesh::copy(const Mesh& copy)
{
    Node::m_parallelScene = m_parallelScene = copy.m_parallelScene;

    m_sceneManager = m_parallelScene->getSceneManager();

    m_triangulate = copy.m_triangulate;
    m_withNormal = copy.m_withNormal;
    m_withTexCoord = copy.m_withTexCoord;
    m_visible = copy.m_visible;
    m_billBoard = copy.m_billBoard;
    m_priorityRender = copy.m_priorityRender;
    m_receiveShadow = copy.m_receiveShadow;
    m_castShadow = copy.m_castShadow;
    m_aabb = copy.m_aabb;
    m_computeNormals = copy.m_computeNormals;
    m_computeTangent = copy.m_computeTangent;
    m_computeAocc = copy.m_computeAocc;

    if(copy.m_hardwareBuffer)
        m_hardwareBuffer = new HardwareBuffer(*copy.m_hardwareBuffer);

    fetchMaterials(copy);

    return *this;
}

Mesh* Mesh::clone()
{
    return new Mesh(*this);
}

AABB Mesh::getAbsolutAabb()
{
    if(!m_hardwareBuffer)
        return 0;

    Matrix4 mat = getAbsoluteMatrix();

    const Vertex::Array& vertex = m_hardwareBuffer->getClientVertex();

    AABB aabb;

    for(unsigned i = 0; i < vertex.size(); i++)
        aabb.count(mat * vertex[i].pos);

    return aabb;
}

void Mesh::computeAabb()
{
    if(!m_hardwareBuffer)
        return;

    const Vertex::Array& vertex = m_hardwareBuffer->getClientVertex();

    m_aabb.clear();

    for(unsigned i = 0; i < vertex.size(); i++)
        m_aabb.count(vertex[i].pos);
}

void Mesh::computeNormal()
{
    if(!m_hardwareBuffer)
        return;

    Vertex* vertex = m_hardwareBuffer->bindBuffer().lock();

    if(!vertex)
        return;

    unsigned vertexCount = m_hardwareBuffer->getVertexCount();

    map<Vector3f, Vector3f> sharedNormals;

    for(unsigned i = 0; i < vertexCount; i += 3)
    {
        Vector3f base = vertex[i].pos;
        sharedNormals[base] += Vector3f::cross(vertex[i + 1].pos - base, vertex[i + 2].pos - base);

        base = vertex[i + 1].pos;
        sharedNormals[base] += Vector3f::cross(vertex[i + 2].pos - base, vertex[i].pos - base);

        base = vertex[i + 2].pos;
        sharedNormals[base] += Vector3f::cross(vertex[i].pos - base, vertex[i + 1].pos - base);
    }

    for(unsigned i = 0; i < vertexCount; i += 3)
    {
        vertex[i + 0].normal = sharedNormals[vertex[i + 0].pos].normalize();
        vertex[i + 1].normal = sharedNormals[vertex[i + 1].pos].normalize();
        vertex[i + 2].normal = sharedNormals[vertex[i + 2].pos].normalize();
    }

    m_hardwareBuffer->snapshot();

    m_hardwareBuffer->unlock().unbindBuffer();

    m_computeNormals = true;
}

void Mesh::computeTangent()
{
    if(!m_hardwareBuffer)
        return;

    // Buffer must be binded
    Vertex* vertex = m_hardwareBuffer->bindBuffer().lock();

    if(!vertex)
        return;

    unsigned indexCount = m_hardwareBuffer->getIndexCount();

    Vector3f::Array tan1;
    tan1.resize(indexCount);

    HardwareBuffer::IndexArray index = m_hardwareBuffer->getClientIndex();

    for(unsigned i = 0; i < indexCount; i += 3)
    {
        const Vector3f& v1 = vertex[index[i + 0]].pos;
        const Vector3f& v2 = vertex[index[i + 1]].pos;
        const Vector3f& v3 = vertex[index[i + 2]].pos;

        const Vector2f& w1 = vertex[index[i + 0]].texCoord;
        const Vector2f& w2 = vertex[index[i + 1]].texCoord;
        const Vector2f& w3 = vertex[index[i + 2]].texCoord;

        float x1 = v2.x - v1.x;
        float y1 = v2.y - v1.y;
        float z1 = v2.z - v1.z;

        float x2 = v3.x - v1.x;
        float y2 = v3.y - v1.y;
        float z2 = v3.z - v1.z;

        float s1 = w2.x - w1.x;
        float s2 = w3.x - w1.x;

        float t1 = w2.y - w1.y;
        float t2 = w3.y - w1.y;

        float r = 1.0F / (s1 * t2 - s2 * t1);

        Vector3f sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
                      (t2 * z1 - t1 * z2) * r);

        tan1[i + 0] += sdir;
        tan1[i + 1] += sdir;
        tan1[i + 2] += sdir;
    }

    for(unsigned i = 0; i < indexCount; i++)
    {
        const Vector3f& n = vertex[index[i]].normal;
        const Vector3f& t = tan1[i];

        // Gram-Schmidt orthogonalize
        vertex[index[i]].tangent = (t - n * Vector3f::dot(n, t));
        vertex[index[i]].tangent.normalize();
    }

    m_hardwareBuffer->snapshot();

    m_hardwareBuffer->unlock().unbindBuffer();

    m_computeTangent = true;
}

void Mesh::computeAocc()
{
    m_computeAocc = true;
}

bool Mesh::isComputeAocc() const
{
    return m_computeAocc;
}

bool Mesh::isComputeTangent() const
{
    return m_computeTangent;
}

bool Mesh::isComputeNormals() const
{
    return m_computeNormals;
}

struct TriangleFace
{
    Vertex v1;
    Vertex v2;
    Vertex v3;

    typedef vector<TriangleFace> Array;
};

struct DepthSortVertexFunc
{

    bool operator()(const TriangleFace& face1, const TriangleFace & face2)
    {
        return (meshPos + (face1.v1.pos + face1.v2.pos + face1.v3.pos) / 3.0f - camPos) >
                (meshPos + (face2.v1.pos + face2.v2.pos + face2.v3.pos) / 3.0f - camPos);
    }

    Vector3f camPos;
    Vector3f meshPos;
};

class ShaderBind
{
public:
    MeshParallelScene* scene;
    Material* material;
    Shader& shader;
    Mesh* mesh;

    ShaderBind(MeshParallelScene* scene, Mesh* mesh, Material* material, Shader& shader_) : shader(shader_)
    {
        this->scene = scene;
        this->material = material;
        this->mesh = mesh;
    }

    void bindLighted(std::string location)
    {
        shader.uniform(location, material->isEnable(Material::LIGHTED));
    }

    void bindFoged(std::string location)
    {
        bool s = material->isEnable(Material::FOGED) & glIsEnabled(GL_FOG);
        shader.uniform(location, s);
    }

    void bindTextured(std::string location)
    {
        shader.uniform(location, material->isEnable(Material::TEXTURED));
    }

    void bindColored(std::string location)
    {
        shader.uniform(location, material->isEnable(Material::COLORED));
    }

    void bindTangent(std::string location)
    {
        GLuint index = glGetAttribLocation(shader, location.c_str());
        mesh->getHardwareBuffer()->bindTangent(true, index);
    }

    void bindAOCC(std::string location)
    {
        GLuint index = glGetAttribLocation(shader, location.c_str());
        mesh->getHardwareBuffer()->bindAocc(true, index);
    }

    void bindTimestamp(std::string location)
    {
        ticks::Clock cl = material->getClock();

        shader.uniform(location, (int) cl.getEsplanedTime(false));
    }

    void bindNodeMatrix(std::string location)
    {
        shader.uniform(location, mesh->getMatrix());
    }

    void bindLightProjMatrix(std::string location)
    {
        Light* light = scene->light(0);
        if(light)
        {
            ShadowMap* smap = light->getShadowMap();
            if(smap)
                shader.uniform(location, smap->getProjectionMatrix());
        }
    }

    void bindLightViewMatrix(std::string location)
    {
        Light* light = scene->light(0);
        if(light)
        {
            Light* light = scene->light(0);
            ShadowMap* smap = light->getShadowMap();
            if(smap)
                shader.uniform(location, smap->getViewMatrix());
        }
    }

    void bindShadowIntensity(std::string location)
    {
        Light* light = scene->light(0);
        if(light)
        {
            Light* light = scene->light(0);
            ShadowMap* smap = light->getShadowMap();
            if(smap)
                shader.uniform(location, smap->getIntensity());
        }
    }

    void bindShadowBlur(std::string location)
    {
        Light* light = scene->light(0);
        if(light)
        {
            Light* light = scene->light(0);
            ShadowMap* smap = light->getShadowMap();
            if(smap)
                shader.uniform(location, smap->getBlurPass());
        }
    }

    void bindShadowCast(std::string location)
    {
        shader.uniform(location, mesh->isCastShadow());
    }

    void bindShadowReceive(std::string location)
    {
        shader.uniform(location, mesh->isReceiveShadow());
    }

    void assignExp(string location, string exp)
    {
        using namespace boost;

        if(isdigit(exp[0]) && iends_with(exp, "i"))
            shader.uniform(location, lexical_cast<int>(exp.data(), exp.size() - 1));
        if(isdigit(exp[0]) && iends_with(exp, "f"))
            shader.uniform(location, lexical_cast<float>(exp.data(), exp.size() - 1));
        else
        {
            map< string, std::function<void(string)> > callmap;

            callmap["lighted"] = boost::bind(&ShaderBind::bindLighted, this, _1);
            callmap["foged"] = boost::bind(&ShaderBind::bindFoged, this, _1);
            callmap["textured"] = boost::bind(&ShaderBind::bindTextured, this, _1);
            callmap["colored"] = boost::bind(&ShaderBind::bindColored, this, _1);
            callmap["tangent"] = boost::bind(&ShaderBind::bindTangent, this, _1);
            callmap["aocc"] = boost::bind(&ShaderBind::bindAOCC, this, _1);
            callmap["timestamp"] = boost::bind(&ShaderBind::bindTimestamp, this, _1);
            callmap["light_projection_matrix"] = boost::bind(&ShaderBind::bindLightProjMatrix, this, _1);
            callmap["light_view_matrix"] = boost::bind(&ShaderBind::bindLightViewMatrix, this, _1);
            callmap["node_matrix"] = boost::bind(&ShaderBind::bindNodeMatrix, this, _1);
            callmap["shadow_intensity"] = boost::bind(&ShaderBind::bindShadowIntensity, this, _1);
            callmap["shadow_blur"] = boost::bind(&ShaderBind::bindShadowBlur, this, _1);
            callmap["shadow_cast"] = boost::bind(&ShaderBind::bindShadowCast, this, _1);
            callmap["shadow_receive"] = boost::bind(&ShaderBind::bindShadowReceive, this, _1);

            if(callmap.count(exp))
                callmap[exp](location);
        }

    }
};

void Mesh::process()
{
    if(!m_enable)
        return;

    for_each(m_childs.begin(), m_childs.end(), mem_fun(&Node::process));
}

bool RayCastTriangle(Vector3f p, Vector3f d, float& i, Vector3f v0, Vector3f v1, Vector3f v2)
{
    Vector3f e1, e2, h, s, q;

    float a, f, u, v;

    e1 = v1 - v0;
    e2 = v2 - v0;

    h = Vector3f::cross(d, e2);

    a = Vector3f::dot(e1, h);

    if(a > -0.00001 && a < 0.00001)
        return false;

    f = 1.0f / a;
    s = p - v0;
    u = f * (Vector3f::dot(s, h));

    if(u < 0.0 || u > 1.0)
        return false;

    q = Vector3f::cross(s, e1);

    v = f * Vector3f::dot(d, q);

    if(v < 0.0 || u + v > 1.0)
        return false;

    // at this stage we can compute t to find out where
    // the intersection point is on the line
    float t = f * Vector3f::dot(e2, q);

    if(t > 0.00001) // ray intersection
    {
        i = t;
        return true;
    }

        // this means that there is a line intersection
        // but not a ray intersection
    else
        return false;
}

bool Mesh::rayCastVertexBuffer(Vector3f rayStart, Vector3f rayDir, float& intersect, bool global)
{
    const Vertex::Array& vertex = m_hardwareBuffer->getClientVertex();

    // Cannot raycast no triangulated mesh !
    if(vertex.size() % 3 != 0)
        return false;

    Matrix4 absmat = getAbsoluteMatrix();

    vector<float> hits;

    for(unsigned i = 0; i < vertex.size(); i += 3)
    {
        Vector3f pos0 = vertex[i + 0].pos,
                pos1 = vertex[i + 1].pos,
                pos2 = vertex[i + 2].pos;

        if(global)
        {
            pos0 = absmat * pos0;
            pos1 = absmat * pos1;
            pos2 = absmat * pos2;
        }

        float intr;

        if(RayCastTriangle(rayStart, rayDir, intr, pos0, pos1, pos2))
            hits.push_back(intr);
    }

    if(!hits.empty())
    {
        intersect = *std::min_element(hits.begin(), hits.end());
        return true;
    }

    else
        return false;
}

bool Mesh::rayCastIndexBuffer(Vector3f rayStart, Vector3f rayDir, float& intersect, bool global)
{
    const HardwareBuffer::IndexArray& index = m_hardwareBuffer->getClientIndex();
    const Vertex::Array& vertex = m_hardwareBuffer->getClientVertex();

    // Cannot raycast no triangulated mesh !
    if(index.size() % 3 != 0)
        return false;

    Matrix4 absmat = getAbsoluteMatrix();

    vector<float> hits;

    for(unsigned i = 0; i < index.size(); i += 3)
    {
        Vector3f pos0 = vertex[index[i + 0]].pos,
                pos1 = vertex[index[i + 1]].pos,
                pos2 = vertex[index[i + 2]].pos;

        if(global)
        {
            pos0 = absmat * pos0;
            pos1 = absmat * pos1;
            pos2 = absmat * pos2;
        }

        float intr;

        if(RayCastTriangle(rayStart, rayDir, intr, pos0, pos1, pos2))
            hits.push_back(intr);
    }

    if(!hits.empty())
    {
        intersect = *std::min_element(hits.begin(), hits.end());
        return true;
    }

    else
        return false;
}

bool Mesh::rayCast(Vector3f rayStart, Vector3f rayDir, float& intersect, bool global)
{
    if(!m_hardwareBuffer)
        return false;

    if(m_hardwareBuffer->isIndexMode())
        return rayCastIndexBuffer(rayStart, rayDir, intersect, global);
    else
        return rayCastVertexBuffer(rayStart, rayDir, intersect, global);
}

bool Mesh::findFloor(float getx, float& sety, float getz, bool global)
{
    if(global)
    {
        float gety = (getAbsoluteMatrix() * m_aabb.max).y + 1;

        float intersect;

        if(rayCast(Vector3f(getx, gety, getz), Vector3f(0, -1, 0), intersect, global))
        {
            sety = gety + -1 * intersect;
            return true;
        }
        else
            return false;
    }

    else
    {
        float intersect;

        if(rayCast(Vector3f(getx, m_aabb.max.y, getz), Vector3f(0, -1, 0), intersect, global))
        {
            sety = m_aabb.max.y + -1 * intersect;
            return true;
        }
        else
            return false;
    }
}

bool Mesh::isTransparent()
{
    BOOST_FOREACH(SubMesh* smesh, m_subMeshs)
    if(smesh->m_material->isTransparent())
        return true;

    return false;
}

unsigned Mesh::getSubMeshCount()
{
    return m_subMeshs.size();
}

SubMesh* Mesh::addSubMesh(std::string name, Material* material, unsigned offset, unsigned size)
{
    SubMesh* smesh = new SubMesh(name, this, material, offset, size);
    m_subMeshs.push_back(smesh);

    return smesh;
}

SubMesh* Mesh::getSubMesh(int index)
{
    return m_subMeshs.at(index);
}

SubMesh* Mesh::getSubMesh(std::string matname)
{
    BOOST_FOREACH(SubMesh* smesh, m_subMeshs)
    if(smesh->m_name == matname)
        return smesh;

    return NULL;
}

const SubMesh::Array& Mesh::getAllSubMesh()
{
    return m_subMeshs;
}

HardwareBuffer* Mesh::getHardwareBuffer() const
{
    return m_hardwareBuffer;
}

void Mesh::setVisible(bool visible)
{
    this->m_visible = visible;
}

bool Mesh::isVisible() const
{
    return m_visible;
}

rtree Mesh::serialize(std::string root)
{
    using namespace boost;

    rtree scheme = Node::serialize(root);

    optional<string> path = scheme.get_optional<string>("class.path");

    if(path && tools::isAbsoloutPath(*path))
    {
        *path = tools::relativizePath(*path, root);
        scheme.put("class.path", *path);
    }

    scheme.put("class", "Mesh");
    scheme.put("class.billBoarding", m_billBoard.toStr());
    scheme.put("class.castShadow", m_castShadow);
    scheme.put("class.receiveShadow", m_receiveShadow);
    scheme.put("class.computeNormal", m_computeNormals);
    scheme.put("class.computeTangent", m_computeTangent);
    scheme.put("class.computeAocc", m_computeAocc);

    // Relativize absolute path according to the root
    optional<rtree&> materials = scheme.get_child_optional("material");

    if(materials)
    {

        BOOST_FOREACH(rtree::value_type& v, *materials)
        {
            string path = v.second.get_value<string>();

            if(tools::isAbsoloutPath(path))
                path = tools::relativizePath(path, root);

            v.second.put_value(path);
        }

    }

    return scheme;
}

void Mesh::setBillBoard(Vector2b billBoard)
{
    this->m_billBoard = billBoard;
}

Vector2b Mesh::getBillBoard() const
{
    return m_billBoard;
}

void Mesh::registerBuffer(Mesh* mesh, const std::string& source)
{
    _BufferManager[mesh] = source;
}

void Mesh::unregisterBuffer(Mesh* mesh)
{
    _BufferManager.erase(mesh);
}

Mesh* Mesh::isSharedBuffer(const std::string& source)
{
    return _BufferManager.shared(source);
}

bool Mesh::isUsedBuffer(HardwareBuffer* hb)
{
    return _BufferManager.used(hb);
}

MeshParallelScene* Mesh::getParallelScene() const
{
    return m_parallelScene;
}

std::vector<std::string> Mesh::getUsedRessources()
{
    vector<string> ressPath;

    BOOST_FOREACH(SubMesh* smesh, m_subMeshs)
    {
        unsigned txcount = smesh->m_material->getTexturesCount();

        for(unsigned i = 0; i < txcount; i++)
            ressPath.push_back(smesh->m_material->getTexture(i).getFilename());
    }

    ressPath.push_back(_BufferManager[this]);

    return ressPath;
}

void Mesh::setPriorityRender(int priorityRender)
{
    this->m_priorityRender = priorityRender;
}

int Mesh::getPriorityRender() const
{
    return m_priorityRender;
}

void Mesh::setCastShadow(bool castShadow)
{
    this->m_castShadow = castShadow;
}

bool Mesh::isCastShadow() const
{
    return m_castShadow;
}

void Mesh::setReceiveShadow(bool receiveShadow)
{
    this->m_receiveShadow = receiveShadow;
}

bool Mesh::isReceiveShadow() const
{
    return m_receiveShadow;
}

SubMesh::SubMesh(std::string name, Mesh* mh, Material* mt, unsigned f, unsigned c)
{
    m_name = name;
    m_owner = mh;
    m_material = mt;
    m_offset = f;
    m_size = c;

    m_hardbuf = m_owner->getHardwareBuffer();
    m_scenemng = m_owner->getSceneManager();
    m_parallel = m_owner->getParallelScene();
}

SubMesh::~SubMesh() { }

void SubMesh::setMaterial(Material* material)
{
    this->m_material = material;
}

Material* SubMesh::getMaterial() const
{
    return m_material;
}

void SubMesh::setSize(unsigned size)
{
    this->m_size = size;
}

unsigned SubMesh::getSize() const
{
    return m_size;
}

void SubMesh::setOffset(unsigned offset)
{
    this->m_offset = offset;
}

unsigned SubMesh::getOffset() const
{
    return m_offset;
}

bool SubMesh::operator==(const SubMesh& other)
{
    return m_owner->getHardwareBuffer() == other.m_owner->getHardwareBuffer()
            && m_material == other.m_material
            && m_offset == other.m_offset
            && m_size == other.m_size;
}

void SubMesh::setName(std::string name)
{
    this->m_name = name;
}

std::string SubMesh::getName() const
{
    return m_name;
}

void SubMesh::setOwner(Mesh* owner)
{
    this->m_owner = owner;
}

Mesh* SubMesh::getOwner() const
{
    return m_owner;
}

void SubMesh::beginProperty()
{
    if(m_material->m_depthTest)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);

    glDepthMask(m_material->m_depthWrite);

    if(m_material->m_renderFlags & Material::BACKFACE_CULL)
    {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }

    else if(m_material->m_renderFlags & Material::FRONTFACE_CULL)
    {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
    }

    if(glIsEnabled(GL_FOG) && !(m_material->m_renderFlags & Material::FOGED))
    {
        glDisable(GL_FOG);
    }

    glPolygonMode(GL_FRONT_AND_BACK, m_material->m_polygoneMode);

    if(m_material->m_lineWidth)
        glLineWidth(m_material->m_lineWidth);

    if(m_material->m_renderFlags & Material::VERTEX_SORT_CULL_TRICK)
    {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
    }

    if(m_material->m_renderFlags & Material::ADDITIVE)
    {
        glEnable(GL_BLEND);
        // glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glBlendFunc(GL_ONE, GL_ONE);
    }

    else if(m_material->m_renderFlags & Material::MULTIPLY)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_DST_COLOR, GL_SRC_COLOR);

    }

    else if(m_material->m_renderFlags & Material::MODULATE)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    else
        glDisable(GL_BLEND);

    if(m_material->m_renderFlags & Material::ALPHA)
    {
        glEnable(GL_ALPHA_TEST);
        glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
        glAlphaFunc(GL_GREATER, m_material->m_alphaThershold);
    }
}

void SubMesh::bindBuffers()
{
    using namespace boost;

    glPushAttrib(GL_ENABLE_BIT);

    m_hardbuf->bindBuffer(true);

    Shader& usedshader = m_material->m_shader;

    // Request uniform for general purpose
    if(usedshader.isEnable())
    {
        ShaderBind callbind(m_parallel, m_owner, m_material, usedshader);

        Shader::bind(usedshader);

        const Shader::UniformMap& umap = usedshader.getRequestedUniform();

        BOOST_FOREACH(Shader::UniformMap::value_type u, umap)
        {
            callbind.assignExp(u.first, u.second);
        }

        Shader::unbind();
    }

    if(m_material->m_renderFlags & Material::TEXTURED)
    {

        BOOST_FOREACH(Texture::Map::value_type itt, m_material->m_textures)
        {
            if(!itt.second)
                continue;

            Texture texture = m_material->m_textures[itt.first];
            TextureApply settings = m_material->m_texApply[itt.first];

            bindTexture(itt.first, texture, settings);
        }

        // Request uniform for texture layer
        if(usedshader.isEnable())
        {
            Shader::bind(usedshader);

            const Shader::UniformMap& umap = usedshader.getRequestedUniform();

            BOOST_FOREACH(Shader::UniformMap::value_type u, umap)
            {
                if(u.second == "shadow_map")
                {
                    unsigned layer = m_material->m_textures.size();
                    usedshader.uniform(u.first, (int) layer);

                    Light* light = m_parallel->light(0);
                    if(light)
                    {
                        ShadowMap* smap = light->getShadowMap();
                        if(smap)
                        {
                            Texture shadowMap = smap->getDepthMap();
                            bindTexture(layer, shadowMap, TextureApply());
                        }
                    }
                    break;
                }
            }

            Shader::unbind();
        }
    }
    else
        glDisable(GL_TEXTURE_2D);

    if(m_material->m_renderFlags & Material::LIGHTED)
    {
        m_hardbuf->bindNormal();

        glEnable(GL_LIGHTING);

        glMaterialfv(GL_FRONT, GL_AMBIENT, m_material->m_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, m_material->m_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, m_material->m_specular);
        glMaterialf(GL_FRONT, GL_SHININESS, m_material->m_shininess);

        /* Normal Scaling ------------------------------------------------------
         *
         * Ici on divise les normale des vertex par le Scale de la matrice du noeud
         * pour les rendre unitaire (normaliser), afin d'éviter un calcule
         * incorrect de la lumière lors d'une mise a l'échelle sur la matrice
         * TODO renable scalling
         */

        // TODO Avoid normal rescaling
        if(!math::isEqual(m_owner->getScale(), 1))
            glEnable(GL_RESCALE_NORMAL);
        else
            glDisable(GL_RESCALE_NORMAL);
    }

    if(m_material->m_renderFlags & Material::COLORED)
    {
        m_hardbuf->bindColor();

        // TODO Avoid Material Coloring on every frame
        if(!usedshader.isEnable())
        {
            glEnable(GL_COLOR_MATERIAL);

            #if 0
            Vertex* vertex = m_hardbuf->lock(GL_READ_WRITE);
            unsigned vertexCount = m_hardbuf->getVertexCount();

            for(unsigned i = m_offset; i < m_offset + m_size && i < vertexCount; i++)
                vertex[i].color *= m_material->m_color;

            m_hardbuf->unlock();
            #endif
        }
    }
}

void SubMesh::draw(const Matrix4& mat)
{
    if(m_material->m_clockCycle > 0)
        if(m_material->m_clock.isEsplanedTime(m_material->m_clockCycle, false))
            m_material->m_clock.snapShoot();

    Shader& usedshader = m_material->m_shader;
    Shader::bind(usedshader);

    const Shader::UniformMap& umap = usedshader.getRequestedUniform();

    // Ambient Pass ------------------------------------------------------------

    if(!(m_material->m_renderFlags & Material::LIGHTED))
    {
        glDisable(GL_LIGHTING);
        transform(mat);
        m_hardbuf->render(m_material->m_faceType, m_offset, m_size, m_material->m_drawPass);
        glPopMatrix();
    }

    else
    {
        glEnable(GL_LIGHTING);
        glDisable(GL_LIGHT0);

        BOOST_FOREACH(Shader::UniformMap::value_type u, umap)
        {
            if(u.second == "ambient_pass")
            {
                usedshader.uniform(u.first, true);
                break;
            }
        }

        transform(mat);
        m_hardbuf->render(m_material->m_faceType, m_offset, m_size, m_material->m_drawPass);
        glPopMatrix();
    }

    // Forward Rendering -------------------------------------------------------

    if(m_material->m_renderFlags & Material::LIGHTED)
    {
        glPushAttrib(GL_ENABLE_BIT);

        BOOST_FOREACH(Shader::UniformMap::value_type u, umap)
        {
            if(u.second == "ambient_pass")
            {
                usedshader.uniform(u.first, false);
                break;
            }
        }

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        glDepthFunc(GL_LEQUAL);

        Vector4f globalAmbient = m_scenemng->getAmbientLight();

        m_scenemng->setAmbientLight(0);

        int lightCount = m_parallel->beginPrePassLighting(m_owner);

        for(int i = 0; i < lightCount; i++)
        {
            m_parallel->prePassLighting(i);

            transform(mat);
            m_hardbuf->render(m_material->m_faceType, m_offset, m_size, m_material->m_drawPass);
            glPopMatrix();
        }

        m_parallel->endPrePassLighting();

        m_scenemng->setAmbientLight(globalAmbient);

        glPopAttrib();
    }

    Shader::unbind();
}

void SubMesh::beginShadowPass()
{
    glPushAttrib(GL_ENABLE_BIT);

    m_hardbuf->bindBuffer();

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.5);

    glDisable(GL_BLEND);
    glDisable(GL_LIGHTING);
    glDisable(GL_FOG);

    glColor4f(0, 0, 0, 1);

    // If we have a back face then cull the front
    if(m_material->m_renderFlags & Material::BACKFACE_CULL)
    {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
    }

    // Only first texture unit to handle alpha
    if(!m_material->m_textures.empty())
    {
        Texture::Map::iterator itt = m_material->m_textures.begin();

        Texture texture = m_material->m_textures[itt->first];
        TextureApply settings = m_material->m_texApply[itt->first];

        bindTexture(itt->first, texture, settings);
    }

}

void SubMesh::drawShadow(const Matrix4& mat)
{
    transform(mat);

    m_hardbuf->render(Material::TRIANGLES, m_offset, m_size);

    glPopMatrix();
}

void SubMesh::endShadowPass()
{
    if(!m_material->m_textures.empty())
    {
        Texture::Map::iterator itt = m_material->m_textures.begin();

        glClientActiveTexture(GL_TEXTURE0);
        m_hardbuf->bindTexture(false);

        glActiveTexture(GL_TEXTURE0);
        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    m_hardbuf->bindBuffer(false);

    glColor4f(1, 1, 1, 1);

    glPopAttrib();
}

void SubMesh::unbindBuffers()
{
    if(m_material->m_renderFlags & Material::SHADER)
    {
        m_hardbuf->bindTangent(false);
        m_hardbuf->bindAocc(false);
    }

    Shader& usedshader = m_material->m_shader;

    if(m_material->m_renderFlags & Material::TEXTURED)
    {

        BOOST_FOREACH(Texture::Map::value_type itt, m_material->m_textures)
        {
            if(!itt.second)
                continue;

            unsigned layer = GL_TEXTURE0 + itt.first;

            glClientActiveTexture(layer);
            m_hardbuf->bindTexture(false);

            glActiveTexture(layer);
            glDisable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        if(usedshader.isRequested("shadow_map"))
        {
            unsigned layer = GL_TEXTURE0 + m_material->m_textures.size();

            glClientActiveTexture(layer);
            m_hardbuf->bindTexture(false);

            glActiveTexture(layer);
            glDisable(GL_TEXTURE_2D);
        }

        // Don't forger to reactivate texture unit 0 for future use !!!
        glClientActiveTexture(GL_TEXTURE0);
        glActiveTexture(GL_TEXTURE0);
    }

    if(m_material->m_renderFlags & Material::LIGHTED)
    {
        m_hardbuf->bindNormal(false);
    }

    if(m_material->m_renderFlags & Material::COLORED)
    {
        m_hardbuf->bindColor(false);
    }

    m_hardbuf->unbindBuffer();

    glPopAttrib();

    glColor4f(1, 1, 1, 1);
}

void SubMesh::endProperty()
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if(m_material->m_renderFlags & Material::VERTEX_SORT_CULL_TRICK)
    {
        glCullFace(GL_BACK);
        m_owner->getHardwareBuffer()->render(m_material->m_faceType, m_offset, m_size, m_material->m_drawPass);
    }
}

void SubMesh::transform(const Matrix4& mat)
{
    glPushMatrix();

    // Billboarding ------------------------------------------------------------

    // TODO fix billboard
    Vector2b billboard = m_owner->getBillBoard();

    if(!!billboard)
    {
        Vector3f position, scale;
        Quaternion rotation;
        mat.decompose(position, rotation, scale);

        Quaternion new_rotation = m_scenemng
                ->computeBillboard(m_owner->getAbsoluteMatrix().getPos(), 0, billboard);

        Matrix4 newmat;
        rotation.w = -rotation.w;
        newmat.rotate(rotation.conjugate());
        newmat.rotate(new_rotation);
        newmat.scale(scale);
        newmat.translate(position);

        glMultMatrixf(newmat);
    }
    else
        glMultMatrixf(mat);
}

void SubMesh::animateTexture(unsigned layer, Texture texture, TextureApply settings)
{
    unsigned vertexCount = m_hardbuf->getVertexCount();

    const Vector2f& size = texture.getSize();

    const Vertex::Array& initvert = m_hardbuf->getClientVertex();

    if(layer)
    {
        Vector2f* uvs = m_hardbuf->lockMultiTexCoord(layer, GL_WRITE_ONLY);

        for(unsigned i = 0; i < vertexCount; i++)
        {
            Vector2f frame(settings.frameSize.x / size.x, settings.frameSize.y / size.y);

            Vector2f scaled(initvert[i].texCoord.x * frame.x, initvert[i].texCoord.y * frame.y);

            uvs[i].x = scaled.x + frame.x * settings.part.x;
            uvs[i].y = scaled.y + frame.y * settings.part.y;
        }
    }
    else
    {
        Vertex* vs = m_hardbuf->lock(GL_WRITE_ONLY);

        for(unsigned i = 0; i < vertexCount; i++)
        {
            Vector2f frame(settings.frameSize.x / size.x, settings.frameSize.y / size.y);

            Vector2f scaled(initvert[i].texCoord.x * frame.x, initvert[i].texCoord.y * frame.y);

            vs[i].texCoord.x = scaled.x + frame.x * settings.part.x;
            vs[i].texCoord.y = scaled.y + frame.y * settings.part.y;
        }
    }

    m_hardbuf->unlock();

    if(settings.animation > 0)
        if(settings.clock.isEsplanedTime(settings.animation))
        {
            settings.part.x++;

            if(settings.part.x >= size.x / settings.frameSize.x)
            {
                settings.part.x = 0;
                settings.part.y++;

                if(settings.part.y >= size.y / settings.frameSize.y)
                    settings.part.y = 0;
            }
        }

}

void SubMesh::bindTexture(unsigned layer, Texture texture, TextureApply settings)
{
    unsigned id = GL_TEXTURE0 + layer;

    // Animation de la texture par modification des coordonés UV
    if(settings.clipped)
        animateTexture(layer, texture, settings);

    glActiveTexture(id);
    glEnable(GL_TEXTURE_2D);
    texture.use(true);

    glClientActiveTexture(id);
    m_hardbuf->bindTexture(true, layer);

    unsigned multexb = settings.blend;

    if(multexb == Material::REPLACE)
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    else if(multexb == Material::ADDITIVE)
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);

    else if(multexb == Material::MODULATE)
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    else if(multexb == Material::ALPHA)
    {
        // Use a shader instead
    }
}
