/*
 * File:   Mesh.cpp
 * Author: b4n92uid
 *
 * Created on 28 avril 2010, 18:36
 */

#include "Mesh.h"
#include "ParallelScene.h"
#include "SceneManager.h"
#include "MeshParallelScene.h"
#include "Tools.h"
#include "ShadowMap.h"
#include "AbstractParser.h"

#include <boost/function.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/optional/optional.hpp>

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
    m_requestVertexRestore = false;
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
    m_requestVertexRestore = false;

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
    m_renderProess.clear();


    if(!m_materialsBackup.empty())
    {
        /*
         * m_materials are cleared by internal manager
         * since they are loaded from a file
         */

        BOOST_FOREACH(Material::Map::value_type& v, m_materialsBackup)
                delete v.second;

        m_materialsBackup.clear();
        m_materials.clear();
    }
    else
    {
        for(Material::Map::iterator it = m_materials.begin(); it != m_materials.end(); ++it)
            delete it->second;

        m_materials.clear();
    }

    Mesh::unregisterBuffer(this);

    if(!Mesh::isUsedBuffer(m_hardwareBuffer))
        delete m_hardwareBuffer, m_hardwareBuffer = NULL;

    m_aabb.clear();
}

void Mesh::fetchMaterials(const Mesh& copy)
{
    for(Material::Map::const_iterator it = m_materials.begin(); it != m_materials.end(); ++it)
        delete it->second;

    m_materials.clear();

    for(Material::Map::const_iterator it = copy.m_materials.begin(); it != copy.m_materials.end(); ++it)
        m_materials[it->first] = new Material(*it->second);

    m_renderProess = copy.m_renderProess;

    for(unsigned i = 0; i < m_renderProess.size(); i++)
        m_renderProess[i].parent = this;
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

    const Vertex::Array& vertex = m_hardwareBuffer->getInitialVertex();

    AABB aabb;

    for(unsigned i = 0; i < vertex.size(); i++)
        aabb.count(mat * vertex[i].pos);

    return aabb;
}

void Mesh::computeAabb()
{
    if(!m_hardwareBuffer)
        return;

    const Vertex::Array& vertex = m_hardwareBuffer->getInitialVertex();

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

    unsigned vertexCount = m_hardwareBuffer->getVertexCount();

    Vector3f::Array tan1(vertexCount);
    Vector3f::Array tan2(vertexCount);

    for(unsigned i = 0; i < vertexCount; i += 3)
    {
        const Vector3f& v1 = vertex[i + 0].pos;
        const Vector3f& v2 = vertex[i + 1].pos;
        const Vector3f& v3 = vertex[i + 2].pos;

        const Vector2f& w1 = vertex[i + 0].texCoord;
        const Vector2f& w2 = vertex[i + 1].texCoord;
        const Vector2f& w3 = vertex[i + 2].texCoord;

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
        Vector3f tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
                      (s1 * z2 - s2 * z1) * r);

        tan1[i + 0] += sdir;
        tan1[i + 1] += sdir;
        tan1[i + 2] += sdir;

        tan2[i + 0] += tdir;
        tan2[i + 1] += tdir;
        tan2[i + 2] += tdir;

        if(i + 3 >= vertexCount)
            break;
    }

    for(unsigned i = 0; i < vertexCount; i++)
    {
        const Vector3f& n = vertex[i].normal;
        const Vector3f& t = tan1[i];

        // Gram-Schmidt orthogonalize
        vertex[i].tangent = (t - n * Vector3f::dot(n, t));
        vertex[i].tangent.normalize();
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

Shader Mesh::getUsedShader(Material* material)
{
    Shader usedshader;

    if(!material->isEnable(Material::PIPELINE))
    {
        if(material->m_shader.isEnable())
            usedshader = material->m_shader;

        else if(m_parallelScene->getRenderingShader().isEnable())
            usedshader = m_parallelScene->getRenderingShader();
    }

    return usedshader;
}

void Mesh::animateTexture(unsigned layer, Texture texture, TextureApply settings)
{
    unsigned vertexCount = m_hardwareBuffer->getVertexCount();

    requestVertexRestore();

    const Vector2f& size = texture.getSize();

    const Vertex::Array& initvert = m_hardwareBuffer->getInitialVertex();

    if(layer)
    {
        Vector2f* uvs = m_hardwareBuffer->lockMultiTexCoord(layer, GL_WRITE_ONLY);

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
        Vertex* vs = m_hardwareBuffer->lock(GL_WRITE_ONLY);

        for(unsigned i = 0; i < vertexCount; i++)
        {
            Vector2f frame(settings.frameSize.x / size.x, settings.frameSize.y / size.y);

            Vector2f scaled(initvert[i].texCoord.x * frame.x, initvert[i].texCoord.y * frame.y);

            vs[i].texCoord.x = scaled.x + frame.x * settings.part.x;
            vs[i].texCoord.y = scaled.y + frame.y * settings.part.y;
        }
    }

    m_hardwareBuffer->unlock();

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

void Mesh::bindTexture(unsigned layer, Texture texture, TextureApply settings)
{
    unsigned id = GL_TEXTURE0 + layer;

    // Animation de la texture par modification des coordonés UV
    if(settings.clipped)
        animateTexture(layer, texture, settings);

    glActiveTexture(id);
    glEnable(GL_TEXTURE_2D);
    texture.use(true);

    glClientActiveTexture(id);
    m_hardwareBuffer->bindTexture(true, layer);

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

struct ShaderBind
{
    Mesh* mesh;
    Material* material;
    Shader& shader;

    ShaderBind(Mesh* mesh, Material* material, Shader& shader_) : shader(shader_)
    {
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

    void bindProjMatrix(std::string location)
    {
        ShadowMap* smap = mesh->getSceneManager()->getShadowMap();
        shader.uniform(location, smap->getProjectionMatrix());
    }

    void bindViewMatrix(std::string location)
    {
        ShadowMap* smap = mesh->getSceneManager()->getShadowMap();
        shader.uniform(location, smap->getViewMatrix());
    }

    void bindNodeMatrix(std::string location)
    {
        shader.uniform(location, mesh->getMatrix());
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
            map<string, function<void (string) > > callmap;
            callmap["lighted"] = boost::bind(&ShaderBind::bindLighted, this, _1);
            callmap["foged"] = boost::bind(&ShaderBind::bindFoged, this, _1);
            callmap["textured"] = boost::bind(&ShaderBind::bindTextured, this, _1);
            callmap["colored"] = boost::bind(&ShaderBind::bindColored, this, _1);
            callmap["tangent"] = boost::bind(&ShaderBind::bindTangent, this, _1);
            callmap["aocc"] = boost::bind(&ShaderBind::bindAOCC, this, _1);
            callmap["timestamp"] = boost::bind(&ShaderBind::bindTimestamp, this, _1);
            callmap["light_projection_matrix"] = boost::bind(&ShaderBind::bindProjMatrix, this, _1);
            callmap["light_view_matrix"] = boost::bind(&ShaderBind::bindViewMatrix, this, _1);
            callmap["node_matrix"] = boost::bind(&ShaderBind::bindNodeMatrix, this, _1);

            if(callmap.count(exp))
                callmap[exp](location);
        }

    }
};

void Mesh::beginRenderingBuffer(Material* material, unsigned offset, unsigned count)
{
    using namespace boost;

    glPushAttrib(GL_ENABLE_BIT);

    Shader usedshader = getUsedShader(material);

    // Request uniform for general purpose
    if(usedshader.isEnable())
    {
        ShaderBind callbind(this, material, usedshader);

        Shader::bind(usedshader);

        const Shader::UniformMap& umap = usedshader.getRequestedUniform();

        BOOST_FOREACH(Shader::UniformMap::value_type u, umap)
        {
            callbind.assignExp(u.first, u.second);
        }

        Shader::unbind();
    }

    if(material->m_renderFlags & Material::TEXTURED)
    {

        BOOST_FOREACH(Texture::Map::value_type itt, material->m_textures)
        {
            if(!itt.second)
                continue;

            Texture texture = material->m_textures[itt.first];
            TextureApply settings = material->m_texApply[itt.first];

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
                    unsigned layer = material->m_textures.size();
                    usedshader.uniform(u.first, (int) layer);

                    Texture shadowMap = m_sceneManager->getShadowMap()->getDepthMap();
                    bindTexture(layer, shadowMap, TextureApply());
                }
            }

            Shader::unbind();
        }
    }
    else
        glDisable(GL_TEXTURE_2D);

    if(material->m_renderFlags & Material::LIGHTED)
    {
        m_hardwareBuffer->bindNormal();

        glEnable(GL_LIGHTING);

        glMaterialfv(GL_FRONT, GL_AMBIENT, material->m_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, material->m_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, material->m_specular);
        glMaterialf(GL_FRONT, GL_SHININESS, material->m_shininess);

        /* Normal Scaling ------------------------------------------------------
         *
         * Ici on divise les normale des vertex par le Scale de la matrice du noeud
         * pour les rendre unitaire (normaliser), afin d'éviter un calcule
         * incorrect de la lumière lors d'une mise a l'échelle sur la matrice
         */

        tbe::Vector3f position, scale;
        tbe::Quaternion rotation;

        m_matrix.decompose(position, rotation, scale);

        if(!math::isEqual(scale, 1))
            glEnable(GL_RESCALE_NORMAL);
        else
            glDisable(GL_RESCALE_NORMAL);
    }

    if(material->m_renderFlags & Material::COLORED)
    {
        unsigned vertexCount = m_hardwareBuffer->getVertexCount();

        m_hardwareBuffer->bindColor();

        if(!usedshader.isEnable())
        {
            glEnable(GL_COLOR_MATERIAL);

            // if(!math::isEqual(material->m_color, 1))
            // requestVertexRestore();

            Vertex* vertex = m_hardwareBuffer->lock(GL_READ_WRITE);

            for(unsigned i = offset; i < offset + count && i < vertexCount; i++)
                vertex[i].color = material->m_color;

            m_hardwareBuffer->unlock();
        }
    }
}

void Mesh::beginRenderingProperty(Material* material, unsigned offset, unsigned count)
{
    if(material->m_depthTest)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);

    glDepthMask(material->m_depthWrite);

    if(material->m_renderFlags & Material::BACKFACE_CULL)
    {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }

    else if(material->m_renderFlags & Material::FRONTFACE_CULL)
    {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
    }

    if(glIsEnabled(GL_FOG) && !(material->m_renderFlags & Material::FOGED))
    {
        glDisable(GL_FOG);
    }

    if(material->m_lineWidth)
        glLineWidth(material->m_lineWidth);

    if(material->m_renderFlags & Material::VERTEX_SORT)
    {
        if(material->m_frameSortWait <= 0)
        {
            static DepthSortVertexFunc cmp;
            cmp.camPos = m_parallelScene->getSceneManager()->getCurCamera()->getPos();
            cmp.meshPos = m_matrix.getPos();

            TriangleFace* vertexes = reinterpret_cast<TriangleFace*> (m_hardwareBuffer->lock(GL_READ_WRITE));
            TriangleFace* start = vertexes + offset / 3;
            TriangleFace* end = start + count / 3;

            std::sort(start, end, cmp);

            m_hardwareBuffer->unlock();

            material->m_frameSortWait = 16;
        }

        material->m_frameSortWait--;
    }

    else if(material->m_renderFlags & Material::VERTEX_SORT_CULL_TRICK)
    {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
    }

    if(material->m_renderFlags & Material::ADDITIVE)
    {
        glEnable(GL_BLEND);
        // glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glBlendFunc(GL_ONE, GL_ONE);
    }

    else if(material->m_renderFlags & Material::MULTIPLY)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_DST_COLOR, GL_SRC_COLOR);

    }

    else if(material->m_renderFlags & Material::MODULATE)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    else
        glDisable(GL_BLEND);

    if(material->m_renderFlags & Material::ALPHA)
    {
        glEnable(GL_ALPHA_TEST);
        glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
        glEnable(GL_MULTISAMPLE);
        glAlphaFunc(GL_GREATER, material->m_alphaThershold);
    }

}

void Mesh::endRenderingProperty(Material* material, unsigned offset, unsigned count)
{
    if(material->m_renderFlags & Material::VERTEX_SORT_CULL_TRICK)
    {
        glCullFace(GL_BACK);
        m_hardwareBuffer->render(material->m_faceType, offset, count);
    }
}

void Mesh::endRenderingBuffer(Material* material, unsigned offset, unsigned count)
{
    if(material->m_renderFlags & Material::SHADER)
    {
        m_hardwareBuffer->bindTangent(false);
        m_hardwareBuffer->bindAocc(false);
    }

    Shader shade = getUsedShader(material);

    if(material->m_renderFlags & Material::TEXTURED)
    {

        BOOST_FOREACH(Texture::Map::value_type itt, material->m_textures)
        {
            if(!itt.second)
                continue;

            unsigned layer = GL_TEXTURE0 + itt.first;

            glClientActiveTexture(layer);
            m_hardwareBuffer->bindTexture(false);

            glActiveTexture(layer);
            glDisable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        if(shade.isRequested("shadow_map"))
        {
            unsigned layer = GL_TEXTURE0 + material->m_textures.size();

            glClientActiveTexture(layer);
            m_hardwareBuffer->bindTexture(false);

            glActiveTexture(layer);
            glDisable(GL_TEXTURE_2D);
        }

        // Don't forger to reactivate texture unit 0 for future use !!!
        glClientActiveTexture(GL_TEXTURE0);
        glActiveTexture(GL_TEXTURE0);
    }

    if(material->m_renderFlags & Material::LIGHTED)
    {
        m_hardwareBuffer->bindNormal(false);
    }

    if(material->m_renderFlags & Material::COLORED)
    {
        m_hardwareBuffer->bindColor(false);
    }

    glPopAttrib();

    glColor4f(1, 1, 1, 1);
}

void Mesh::beginRenderingMatrix()
{
    glPushMatrix();

    if(m_parent && !m_parent->isRoot())
        glMultMatrixf(m_parent->getAbsoluteMatrix());

    // Billboarding ------------------------------------------------------------

    if(!!m_billBoard)
    {
        Vector3f position, scale;
        Quaternion rotation;
        m_matrix.decompose(position, rotation, scale);

        Quaternion new_rotation = m_sceneManager
                ->computeBillboard(getAbsoluteMatrix().getPos(), 0, m_billBoard);

        Matrix4 newmat;
        rotation.w = -rotation.w;
        newmat.rotate(rotation.conjugate());
        newmat.rotate(new_rotation);
        newmat.scale(scale);
        newmat.translate(position);

        glMultMatrixf(newmat);
    }
    else
        glMultMatrixf(m_matrix);
}

void Mesh::endRenderingMatrix()
{
    glPopMatrix();
}

void Mesh::drawMaterial(Material* material, unsigned offset, unsigned count)
{
    if(material->m_clock.isEsplanedTime(material->m_clockCycle, false))
        material->m_clock.snapShoot();

    Shader usedshader = getUsedShader(material);

    beginRenderingBuffer(material, offset, count);

    beginRenderingMatrix();
    beginRenderingProperty(material, offset, count);

    const Shader::UniformMap& umap = usedshader.getRequestedUniform();

    Shader::bind(usedshader);

    if(!(material->m_renderFlags & Material::LIGHTED))
    {
        m_hardwareBuffer->render(material->m_faceType, offset, count);
    }

    else
    {
        // Disable or use default shader for only scene ambient light

        BOOST_FOREACH(Shader::UniformMap::value_type u, umap)
        {
            if(u.second == "ambient_pass")
                usedshader.uniform(u.first, true);
        }

        m_hardwareBuffer->render(material->m_faceType, offset, count);
    }

    endRenderingProperty(material, offset, count);
    endRenderingMatrix();

    // Forward Rendering -------------------------------------------------------

    if(material->m_renderFlags & Material::LIGHTED)
    {
        Shader::bind(usedshader);

        BOOST_FOREACH(Shader::UniformMap::value_type u, umap)
        {
            if(u.second == "ambient_pass")
                usedshader.uniform(u.first, false);
        }

        glEnable(GL_BLEND);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        glDepthFunc(GL_LEQUAL);

        Vector4f globalAmbient = m_sceneManager->getAmbientLight();

        m_sceneManager->setAmbientLight(0);

        int lightCount = m_parallelScene->beginPrePassLighting(this);

        for(int i = 0; i < lightCount; i++)
        {
            m_parallelScene->prePassLighting(i);

            beginRenderingMatrix();
            m_hardwareBuffer->render(material->m_faceType, offset, count);
            endRenderingMatrix();
        }

        m_parallelScene->endPrePassLighting();

        m_sceneManager->setAmbientLight(globalAmbient);

        Shader::unbind();
    }

    endRenderingBuffer(material, offset, count);
}

void Mesh::render()
{
    if(!m_hardwareBuffer || m_hardwareBuffer->isEmpty() || !m_enable || !m_visible)
        return;

    m_hardwareBuffer->bindBuffer();

    // Render ------------------------------------------------------------------

    if(m_renderProess.empty())
    {
        Material defaultMateral;
        drawMaterial(&defaultMateral, 0, m_hardwareBuffer->getVertexCount());
    }

    else
    {
        if(m_renderProess.size() > 1)
        {
            std::sort(m_renderProess.begin(), m_renderProess.end(), renderProcessSortFunc);

            for(unsigned i = 0; i < m_renderProess.size(); i++)
                drawMaterial(m_materials[m_renderProess[i].applyMaterial], m_renderProess[i].offset, m_renderProess[i].size);
        }
        else
        {
            drawMaterial(m_materials[m_renderProess.front().applyMaterial], m_renderProess.front().offset, m_renderProess.front().size);
        }

    }

    if(m_requestVertexRestore)
    {
        m_hardwareBuffer->restore();
        m_requestVertexRestore = false;
    }

    m_hardwareBuffer->bindBuffer(false);
}

void Mesh::renderShadow()
{
    if(!m_hardwareBuffer || m_hardwareBuffer->isEmpty() || !m_enable || !m_visible)
        return;

    beginRenderingMatrix();
    m_hardwareBuffer->bindBuffer();

    glEnable(GL_DEPTH_TEST);
    glDepthMask(true);

    m_hardwareBuffer->render(Material::TRIANGLES, 0, m_hardwareBuffer->getVertexCount());

    m_hardwareBuffer->bindBuffer(false);
    endRenderingMatrix();
}

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

bool Mesh::rayCast(Vector3f rayStart, Vector3f rayDir, float& intersect, bool global)
{
    if(!m_hardwareBuffer)
        return false;

    const Vertex::Array& vertex = m_hardwareBuffer->getInitialVertex();

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
    for(Material::Map::iterator itt = m_materials.begin(); itt != m_materials.end(); itt++)
        if(!itt->second->isTransparent())
            return false;

    return true;
}

void Mesh::addMaterial(std::string name, Material* material)
{
    m_materials[name] = material;

    material->setName(name);
}

void Mesh::deleteMaterial(std::string name)
{
    m_materials.erase(name);
}

unsigned Mesh::getMaterialCount()
{
    return m_materials.size();
}

Material::Array Mesh::getAllMaterial()
{
    std::vector<Material*> ret;
    ret.reserve(m_materials.size());

    for(Material::Map::iterator itt = m_materials.begin(); itt != m_materials.end(); itt++)
        ret.push_back(itt->second);

    return ret;
}

Vector2i::Array Mesh::getMaterialApply(std::string name)
{
    Vector2i::Array offset;

    for(unsigned i = 0; i < m_renderProess.size(); i++)
    {
        const RenderProcess& rp = m_renderProess[i];

        if(rp.applyMaterial == name)
            offset.push_back(Vector2i(rp.offset, rp.size));
    }

    return offset;
}

Material* Mesh::getMaterial(unsigned index)
{
    if(index == 0 && !m_materials.empty())
        return m_materials.begin()->second;

    if(index > m_materials.size() - 1)
        return NULL;

    unsigned i = 0;
    for(Material::Map::iterator it = m_materials.begin(); it != m_materials.end(); ++it, i++)
        if(i == index) return it->second;
}

Material* Mesh::getMaterial(std::string name)
{
    if(m_materials.find(name) == m_materials.end())
        return NULL;

    return m_materials[name];
}

Material* Mesh::releaseMaterial(std::string name)
{
    if(m_materials.find(name) == m_materials.end())
        return NULL;

    Material* toRet = m_materials[name];
    m_materials.erase(name);
    return toRet;
}

void Mesh::applyMaterial(std::string name, unsigned offset, unsigned size)
{
    RenderProcess rp = {this, name, offset, size};
    m_renderProess.push_back(rp);
}

void Mesh::applyMaterial(Material* material, unsigned offset, unsigned size)
{
    BOOST_FOREACH(Material::Map::value_type &it, m_materials)
    if(it.second == material)
    {
        RenderProcess rp = {this, it.first, offset, size};
        m_renderProess.push_back(rp);
        return;
    }

    throw tbe::Exception("Mesh::ApplyMaterial; [%1%] Material ptr not found") % m_name;
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

void Mesh::attachMaterialSet(const Material::Map& set)
{
    if(m_materialsBackup.empty())
        m_materialsBackup = m_materials;

    BOOST_FOREACH(const Material::Map::value_type& v, set)
    {
        if(!m_materials.count(v.first))
        {
            cout << "/!\\ WARNING: Mesh::attachMaterialFile; Unexpected material (" << v.first << ")" << endl;
            continue;
        }

        BOOST_FOREACH(Texture::Map::value_type& t, v.second->m_textures)
        {
            if(t.first != 0)
                m_hardwareBuffer->newMultiTexCoord(t.first);
        }

        m_materials[v.first] = v.second;
    }

    m_hardwareBuffer->compile();
}

void Mesh::attachMaterialFile(std::string path)
{
    Material::Map matset = AbstractParser::loadMaterialSet(path);

    m_attachMaterial = path;

    attachMaterialSet(matset);
}

void Mesh::releaseMaterialFile()
{

    BOOST_FOREACH(Material::Map::value_type &v, m_materialsBackup)
    {
        *m_materials[v.first] = *v.second;
        delete v.second, v.second = NULL;
    }

    m_attachMaterial.clear();
}

std::string Mesh::getMaterialFile()
{
    return m_attachMaterial;
}

rtree Mesh::serializeMaterial(std::string root)
{
    rtree scheme;

    for(Material::Map::iterator it = m_materials.begin(); it != m_materials.end(); it++)
    {
        rtree matscheme;

        matscheme.put_value(it->first);

        matscheme.put("ambient", it->second->getAmbient());
        matscheme.put("diffuse", it->second->getDiffuse());
        matscheme.put("specular", it->second->getSpecular());
        matscheme.put("shininess", it->second->getShininess());
        matscheme.put("clockCycle", it->second->m_clockCycle);

        matscheme.put("alpha", it->second->isEnable(Material::ALPHA));
        matscheme.put("alphaThershold", it->second->m_alphaThershold);

        if(it->second->isEnable(Material::MODULATE))
            matscheme.put("blend", "modulate");
        else if(it->second->isEnable(Material::ADDITIVE))
            matscheme.put("blend", "additive");
        else if(it->second->isEnable(Material::MULTIPLY))
            matscheme.put("blend", "multiplty");

        matscheme.put("color", it->second->m_color);
        matscheme.put("cullTrick", it->second->isEnable(Material::VERTEX_SORT_CULL_TRICK));

        if(it->second->isEnable(Material::FRONTFACE_CULL))
            matscheme.put("faceCull", "front");
        if(it->second->isEnable(Material::BACKFACE_CULL))
            matscheme.put("faceCull", "back");

        matscheme.put("lighted", it->second->isEnable(Material::LIGHTED));
        matscheme.put("textured", it->second->isEnable(Material::TEXTURED));
        matscheme.put("colored", it->second->isEnable(Material::COLORED));

        if(it->second->m_shader.isEnable())
        {
            Shader shade = it->second->m_shader;

            string shaderpath = shade.getShaderFile();

            if(!shaderpath.empty())
            {
                rtree shadertree = shade.serialize(shaderpath);
                boost::property_tree::write_info(shaderpath, shadertree);

                shaderpath = tools::relativizePath(shaderpath, root);

                matscheme.put("shader", shaderpath);
            }
            else
                cout << "/!\\ WARNING: Mesh::serializeMaterial; Shader has not been loaded from file (" << shaderpath << ")" << endl;
        }

        unsigned txcount = it->second->getTexturesCount();

        for(unsigned i = 0; i < txcount; i++)
        {
            rtree texscheme;

            Texture tex = it->second->getTexture(i);

            texscheme.put("path", tools::relativizePath(tex.getFilename(), root));
            texscheme.put("origin", tex.getOrigin());
            texscheme.put("mipmap", tex.isGenMipMap());

            unsigned blend = it->second->m_texApply[i].blend;

            if(blend == Material::MODULATE)
                texscheme.put("blend", "modulate");
            if(blend == Material::ADDITIVE)
                texscheme.put("blend", "additive");
            if(blend == Material::REPLACE)
                texscheme.put("blend", "replace");

            matscheme.add_child("textures.unit", texscheme);
        }

        scheme.add_child("pass", matscheme);
    }

    return scheme;
}

rtree Mesh::serialize(std::string root)
{
    using namespace boost;

    rtree scheme = Node::serialize(root);

    if(scheme.count("class") && scheme.get_child("class").empty())
    {
        // This node is loaded from external file
    }
    else
    {
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

        if(!m_attachMaterial.empty())
        {
            string matpath = m_attachMaterial;

            if(tools::isAbsoloutPath(matpath))
                matpath = tools::relativizePath(matpath, root);

            scheme.put("material", matpath);
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

void Mesh::requestVertexRestore(bool requestVertexRestore)
{
    this->m_requestVertexRestore = requestVertexRestore;
}

std::vector<std::string> Mesh::getUsedRessources()
{
    vector<string> ressPath;

    for(Material::Map::iterator it = m_materials.begin(); it != m_materials.end(); it++)
    {
        unsigned txcount = it->second->getTexturesCount();

        for(unsigned i = 0; i < txcount; i++)
            ressPath.push_back(it->second->getTexture(i).getFilename());
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
