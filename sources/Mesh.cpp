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

} manager;

void Mesh::registerBuffer(Mesh* mesh, const std::string& source)
{
    manager[mesh] = source;
}

void Mesh::unregisterBuffer(Mesh* mesh)
{
    manager.erase(mesh);
}

Mesh* Mesh::isSharedBuffer(const std::string& source)
{
    return manager.shared(source);
}

bool Mesh::isUsedBuffer(HardwareBuffer* hb)
{
    return manager.used(hb);
}

void Mesh::setMaterialFile(std::string materialFile)
{
    this->m_materialFile = materialFile;
}

std::string Mesh::getMaterialFile() const
{
    return m_materialFile;
}

Mesh::Mesh(MeshParallelScene* scene)
{
    m_triangulate = true;
    m_withNormal = false;
    m_withTexCoord = false;
    m_visible = true;

    Node::m_parallelScene = m_parallelScene = scene;

    m_sceneManager = m_parallelScene->getSceneManager();

    m_parallelScene->registerNode(this);

    m_hardwareBuffer = NULL;

    m_vertexScale = 1;
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
    m_renderProess.clear();

    for(Material::Map::iterator it = m_materials.begin(); it != m_materials.end(); ++it)
        delete it->second;

    m_materials.clear();

    Mesh::unregisterBuffer(this);

    if(!Mesh::isUsedBuffer(m_hardwareBuffer))
        delete m_hardwareBuffer, m_hardwareBuffer = NULL;

    m_aabb.clear();
}

void Mesh::fetch(const Mesh& copy)
{
    m_triangulate = copy.m_triangulate;
    m_withNormal = copy.m_withNormal;
    m_withTexCoord = copy.m_withTexCoord;

    m_hardwareBuffer = copy.m_hardwareBuffer;

    for(Material::Map::const_iterator it = m_materials.begin(); it != m_materials.end(); ++it)
        delete it->second;

    m_materials.clear();

    for(Material::Map::const_iterator it = copy.m_materials.begin(); it != copy.m_materials.end(); ++it)
        m_materials[it->first] = new Material(*it->second);

    m_renderProess = copy.m_renderProess;

    for(unsigned i = 0; i < m_renderProess.size(); i++)
        m_renderProess[i].parent = this;
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

    m_hardwareBuffer = new HardwareBuffer(*copy.m_hardwareBuffer);

    for(Material::Map::const_iterator it = m_materials.begin(); it != m_materials.end(); ++it)
        delete it->second;

    m_materials.clear();

    for(Material::Map::const_iterator it = copy.m_materials.begin(); it != copy.m_materials.end(); ++it)
        m_materials[it->first] = new Material(*it->second);

    m_renderProess = copy.m_renderProess;

    for(unsigned i = 0; i < m_renderProess.size(); i++)
        m_renderProess[i].parent = this;

    m_vertexScale = 1;

    setVertexScale(copy.m_vertexScale);

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
        aabb.count(mat * (vertex[i].pos * m_vertexScale));

    return aabb;
}

void Mesh::computeAabb()
{
    if(!m_hardwareBuffer)
        return;

    const Vertex::Array& vertex = m_hardwareBuffer->getInitialVertex();

    m_aabb.clear();

    for(unsigned i = 0; i < vertex.size(); i++)
        m_aabb.count(vertex[i].pos * m_vertexScale);
}

void Mesh::computeTangent()
{
    if(!m_hardwareBuffer)
        return;

    Vertex* vertex = m_hardwareBuffer->lock();

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

    m_hardwareBuffer->unlock();
    m_hardwareBuffer->snapshot();
}

void Mesh::computeAocc()
{

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
        return(meshPos + (face1.v1.pos + face1.v2.pos + face1.v3.pos) / 3.0f - camPos) >
                (meshPos + (face2.v1.pos + face2.v2.pos + face2.v3.pos) / 3.0f - camPos);
    }

    Vector3f camPos;
    Vector3f meshPos;
};

void Mesh::render(Material* material, unsigned offset, unsigned count)
{
    GLint tangentAttribIndex = -1, aoccAttribIndex = -1;

    unsigned vertexCount = m_hardwareBuffer->getVertexCount();

    m_hardwareBuffer->bindBuffer();

    glPushAttrib(GL_ENABLE_BIT);

    if(material->m_depthTest)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);

    glDepthMask(material->m_depthWrite);

    // Culling -----------------------------------------------------------------

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
    else
        // NOTE OpenGL 1.4
        glDisable(GL_CULL_FACE);

    // Sort polygones ----------------------------------------------------------

    if(material->m_renderFlags & Material::VERTEX_SORT)
    {
        if(material->m_frameSortWait <= 0)
        {
            static DepthSortVertexFunc cmp;
            cmp.camPos = m_parallelScene->getSceneManager()->getCurCamera()->getPos();
            cmp.meshPos = m_matrix.getPos();

            TriangleFace* vertexes = static_cast<TriangleFace*>(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE));
            TriangleFace* start = vertexes + offset / 3;
            TriangleFace* end = start + count / 3;

            std::sort(start, end, cmp);

            glUnmapBuffer(GL_ARRAY_BUFFER);

            material->m_frameSortWait = 16;
        }

        material->m_frameSortWait--;
    }

    else if(material->m_renderFlags & Material::VERTEX_SORT_CULL_TRICK)
    {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);

        glDepthMask(false);
    }

    // Shader ------------------------------------------------------------------

    if(material->m_renderFlags & Material::SHADER)
    {
        // Tangent

        if(material->m_renderFlags & Material::TANGENT)
        {
            tangentAttribIndex = glGetAttribLocation(material->m_shader, material->m_tangentLocation.c_str());

            if(tangentAttribIndex == -1)
                throw Exception("Mesh::render; Invalid tangent location (%s)", material->m_tangentLocation.c_str());

            m_hardwareBuffer->bindTangent(true, tangentAttribIndex);
        }

        // Amobient occlusion

        if(material->m_renderFlags & Material::AOCC)
        {
            aoccAttribIndex = glGetAttribLocation(material->m_shader, material->m_aoccLocation.c_str());

            if(aoccAttribIndex == -1)
                throw Exception("Mesh::render; Invalid tangent location (%s)", material->m_aoccLocation.c_str());

            m_hardwareBuffer->bindAocc(true, aoccAttribIndex);
        }

        material->m_shader.use(true);
    }

    // Texture -----------------------------------------------------------------

    if(material->m_renderFlags & Material::TEXTURED)
    {
        Texture::Map& textures = material->m_textures;
        Material::TexApplyMap& texApply = material->m_texApply;

        for(Texture::Map::iterator itt = textures.begin();
            itt != textures.end(); itt++)
        {
            if(!itt->second)
                continue;

            unsigned textureIndex = GL_TEXTURE0 + itt->first;

            glClientActiveTexture(textureIndex);

            /*
             * Animation de la texture par modification des coordonés UV
             */
            if(texApply[itt->first].clipped)
            {
                const Vector2f& texSize = itt->second.getSize();

                const Vertex::Array& initvert = m_hardwareBuffer->getInitialVertex();

                Vector2i& frame_offset = texApply[itt->first].part;
                Vector2i& frame_size = texApply[itt->first].frameSize;

                if(itt->first > 0)
                {
                    Vector2f* uvs = m_hardwareBuffer->lockMultiTexCoord(itt->first);

                    for(unsigned i = 0; i < vertexCount; i++)
                    {
                        Vector2f frame(frame_size.x / texSize.x, frame_size.y / texSize.y);

                        Vector2f scaled(initvert[i].texCoord.x * frame.x, initvert[i].texCoord.y * frame.y);

                        uvs[i].x = scaled.x + frame.x * frame_offset.x;
                        uvs[i].y = scaled.y + frame.y * frame_offset.y;
                    }
                }
                else
                {
                    Vertex* vs = m_hardwareBuffer->lock();

                    for(unsigned i = 0; i < vertexCount; i++)
                    {
                        Vector2f frame(frame_size.x / texSize.x, frame_size.y / texSize.y);

                        Vector2f scaled(initvert[i].texCoord.x * frame.x, initvert[i].texCoord.y * frame.y);

                        vs[i].texCoord.x = scaled.x + frame.x * frame_offset.x;
                        vs[i].texCoord.y = scaled.y + frame.y * frame_offset.y;
                    }
                }

                m_hardwareBuffer->unlock(false);

                if(texApply[itt->first].animation > 0)
                    if(texApply[itt->first].clock.isEsplanedTime(texApply[itt->first].animation))
                    {
                        frame_offset.x++;

                        if(frame_offset.x >= texSize.x / texApply[itt->first].frameSize.x)
                        {
                            frame_offset.x = 0;
                            frame_offset.y++;

                            if(frame_offset.y >= texSize.y / texApply[itt->first].frameSize.y)
                                frame_offset.y = 0;
                        }
                    }
            }


            m_hardwareBuffer->bindTexture(true, itt->first);

            glActiveTexture(textureIndex);
            glEnable(GL_TEXTURE_2D);

            itt->second.use(true);

            unsigned multexb = texApply[itt->first].blend;

            if(multexb == Material::REPLACE)
                glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

            else if(multexb == Material::ADDITIVE)
                glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);

            else if(multexb == Material::MODULATE)
                glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        }
    }

    // Lumiere -----------------------------------------------------------------

    if(material->m_renderFlags & Material::LIGHTED)
    {
        m_hardwareBuffer->bindNormal();

        glEnable(GL_LIGHTING);

        glMaterialfv(GL_FRONT, GL_AMBIENT, material->m_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, material->m_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, material->m_specular);
        glMaterialf(GL_FRONT, GL_SHININESS, material->m_shininess);
    }

    // Color -------------------------------------------------------------------

    if(material->m_renderFlags & Material::COLORED)
    {
        m_hardwareBuffer->bindColor();

        glEnable(GL_COLOR_MATERIAL);
    }

    // Blend -------------------------------------------------------------------

    if(material->m_renderFlags & Material::BLEND_ADD)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        glDepthMask(false);
    }

    else if(material->m_renderFlags & Material::BLEND_MUL)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_DST_COLOR, GL_SRC_COLOR);

    }

    else if(material->m_renderFlags & Material::BLEND_MOD)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    if(material->m_renderFlags & Material::ALPHA)
    {
        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, material->m_alphaThershold);
    }

    // Fog ---------------------------------------------------------------------

    if(glIsEnabled(GL_FOG) && !(material->m_renderFlags & Material::FOGED))
        glDisable(GL_FOG);

    // Line width --------------------------------------------------------------

    if(material->m_lineWidth)
        glLineWidth(material->m_lineWidth);

    // Coloring ----------------------------------------------------------------

    Vertex* vertex = m_hardwareBuffer->lock();

    for(unsigned i = offset; i < offset + count && i < vertexCount; i++)
    {
        vertex[i].color = material->m_color;
    }

    m_hardwareBuffer->unlock();

    // Rendue ------------------------------------------------------------------

    if(material->m_drawPass > 1)
    {
        int _count = count / material->m_drawPass;

        for(unsigned i = 0; i < material->m_drawPass - 1; i++)
        {
            int _offset = offset + (i * _count);

            m_hardwareBuffer->render(material->m_faceType, _offset, _count);
        }

        int rest = count % material->m_drawPass;

        if(rest > 0)
            m_hardwareBuffer->render(material->m_faceType, offset + count - rest, rest);
        else
            m_hardwareBuffer->render(material->m_faceType, offset + count - _count, _count);
    }
    else
    {
        m_hardwareBuffer->render(material->m_faceType, offset, count);
    }

    // Réstorations ------------------------------------------------------------

    if(material->m_renderFlags & Material::BLEND_ADD)
    {
        glDepthMask(true);
    }

    if(material->m_renderFlags & Material::VERTEX_SORT_CULL_TRICK)
    {
        glCullFace(GL_BACK);
        glDrawArrays(material->m_faceType, offset, count);

        glDepthMask(true);
    }

    if(material->m_renderFlags & Material::SHADER)
    {
        if(material->m_renderFlags & Material::TANGENT)
            m_hardwareBuffer->bindTangent(false, tangentAttribIndex);

        if(material->m_renderFlags & Material::AOCC)
            m_hardwareBuffer->bindAocc(false, aoccAttribIndex);


        material->m_shader.use(false);
    }

    if(material->m_renderFlags & Material::TEXTURED)
    {
        Texture::Map& textures = material->m_textures;

        for(Texture::Map::reverse_iterator itt = textures.rbegin();
            itt != textures.rend(); itt++)
        {
            if(!itt->second)
                continue;

            unsigned textureIndex = GL_TEXTURE0 + itt->first;

            glClientActiveTexture(textureIndex);
            m_hardwareBuffer->bindTexture(false);

            glActiveTexture(textureIndex);
            glDisable(GL_TEXTURE_2D);
        }
    }

    if(material->m_renderFlags & Material::LIGHTED)
    {
        m_hardwareBuffer->bindNormal(false);
    }

    if(material->m_renderFlags & Material::COLORED)
    {
        m_hardwareBuffer->bindColor(false);
    }

    m_hardwareBuffer->bindBuffer(false);

    // NOTE OpenGL 1.4
    glColor4f(1, 1, 1, 1);

    glPopAttrib();
}

void Mesh::render()
{
    if(!m_hardwareBuffer || m_hardwareBuffer->isEmpty() || !m_enable || !m_visible)
        return;

    m_hardwareBuffer->restore();

    glPushMatrix();

    if(m_parent)
        glMultMatrixf(m_parent->getAbsoluteMatrix());

    // Billboarding ------------------------------------------------------------

    if(!!m_billBoard)
    {
        Matrix4 setmat = m_matrix;

        Vector3f pos = getAbsoluteMatrix().getPos();

        Matrix4 rotation = m_sceneManager->computeBillboard(pos, Matrix4(), 0, m_billBoard);

        setmat.setRotate(rotation.getRotate());

        glMultMatrixf(setmat);
    }
    else
        glMultMatrixf(m_matrix);

    // Vertex Scaling ----------------------------------------------------------

    m_aabb.clear();

    Vertex* vertex = m_hardwareBuffer->lock();

    unsigned vertexCount = m_hardwareBuffer->getVertexCount();

    for(unsigned i = 0; i < vertexCount; i++)
    {
        vertex[i].pos *= m_vertexScale;

        m_aabb.count(vertex[i].pos);
    }

    m_hardwareBuffer->unlock();

    // Render ------------------------------------------------------------------

    if(m_renderProess.empty())
    {
        Material defaultMateral;
        render(&defaultMateral, 0, m_hardwareBuffer->getVertexCount());
    }

    else
    {
        std::sort(m_renderProess.begin(), m_renderProess.end(), renderProcessSortFunc);

        for(unsigned i = 0; i < m_renderProess.size(); i++)
            render(m_materials[m_renderProess[i].applyMaterial],
                   m_renderProess[i].offset, m_renderProess[i].size);

    }

    glPopMatrix();
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

bool Mesh::rayCast(Vector3f rayStart, Vector3f rayDiri, float& intersect, bool global)
{
    const Vertex::Array& vertex = m_hardwareBuffer->getInitialVertex();

    Matrix4 absmat = getAbsoluteMatrix();

    vector<float> hits;

    for(unsigned i = 0; i < vertex.size(); i += 3)
    {
        Vector3f pos0 = vertex[i + 0].pos * m_vertexScale,
                pos1 = vertex[i + 1].pos * m_vertexScale,
                pos2 = vertex[i + 2].pos * m_vertexScale;

        if(global)
        {
            pos0 = absmat * pos0;
            pos1 = absmat * pos1;
            pos2 = absmat * pos2;
        }

        float intr;

        if(RayCastTriangle(rayStart, rayDiri, intr, pos0, pos1, pos2))
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

void Mesh::applyShader(std::string materialName, Shader shader)
{
    m_materials[materialName]->setShader(shader);
    m_materials[materialName]->enable(Material::SHADER);
}

void Mesh::applyShader(Shader shader)
{
    for(Material::Map::iterator itt = m_materials.begin(); itt != m_materials.end(); itt++)
    {
        itt->second->setShader(shader);
        itt->second->enable(Material::SHADER);
    }
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

Material* Mesh::getMaterial(std::string name)
{
    if(m_materials.find(name) == m_materials.end())
        throw tbe::Exception("Mesh::GetMaterial; Material not found (%s)", name.c_str());

    return m_materials[name];
}

Material* Mesh::releaseMaterial(std::string name)
{
    if(m_materials.find(name) == m_materials.end())
        throw tbe::Exception("Mesh::ReleaseMaterial; Material not found (%s)", name.c_str());

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
    for(Material::Map::iterator it = m_materials.begin();
        it != m_materials.end(); ++it)
        if(it->second == material)
        {
            RenderProcess rp = {this, it->first, offset, size};
            m_renderProess.push_back(rp);
            return;
        }

    throw tbe::Exception("Mesh::ApplyMaterial; Material ptr not found");
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

Mesh::CtorMap Mesh::outputMaterial(std::string root)
{
    CtorMap ctormap;

    for(Material::Map::iterator it = m_materials.begin(); it != m_materials.end(); it++)
    {
        ctormap[it->first + ":alphaThershold"] = tools::numToStr(it->second->m_alphaThershold);
        ctormap[it->first + ":blendMod"] = tools::numToStr(it->second->isEnable(Material::BLEND_MOD));
        ctormap[it->first + ":color"] = tools::numToStr(it->second->m_color);
        ctormap[it->first + ":cullTrick"] = tools::numToStr(it->second->isEnable(Material::VERTEX_SORT_CULL_TRICK));
        ctormap[it->first + ":blendMul"] = tools::numToStr(it->second->isEnable(Material::BLEND_MUL));
        ctormap[it->first + ":blendAdd"] = tools::numToStr(it->second->isEnable(Material::BLEND_ADD));
        ctormap[it->first + ":alpha"] = tools::numToStr(it->second->isEnable(Material::ALPHA));
        ctormap[it->first + ":frontFaceCull"] = tools::numToStr(it->second->isEnable(Material::FRONTFACE_CULL));
        ctormap[it->first + ":backFaceCull"] = tools::numToStr(it->second->isEnable(Material::BACKFACE_CULL));
        ctormap[it->first + ":lighted"] = tools::numToStr(it->second->isEnable(Material::LIGHTED));
        ctormap[it->first + ":textured"] = tools::numToStr(it->second->isEnable(Material::TEXTURED));
        ctormap[it->first + ":colored"] = tools::numToStr(it->second->isEnable(Material::COLORED));

        unsigned txcount = it->second->getTexturesCount();

        for(unsigned i = 0; i < txcount; i++)
        {
            string key = it->first + ":texture:" + tools::numToStr(i);

            Texture tex = it->second->getTexture(i);

            ctormap[key] = tools::pathScope(root, tex.getFilename(), false);
            ctormap[key] += ";";

            unsigned blend = it->second->m_texApply[i].blend;

            if(blend == Material::MODULATE)
                ctormap[key] += "modulate";
            if(blend == Material::ADDITIVE)
                ctormap[key] += "additive";
            if(blend == Material::REPLACE)
                ctormap[key] += "replace";

            ctormap[key] += ";" + tools::numToStr(it->second->m_texApply[i].clipped);
            ctormap[key] += ";" + tools::numToStr(it->second->m_texApply[i].animation);
            ctormap[key] += ";" + it->second->m_texApply[i].frameSize.toStr();
            ctormap[key] += ";" + it->second->m_texApply[i].part.toStr();
        }
    }
}

Node::CtorMap Mesh::constructionMap(std::string root)
{
    Node::CtorMap ctormap = Node::constructionMap(root);

    ctormap["vertexScale"] = m_vertexScale.toStr();
    ctormap["billBoarding"] = m_billBoard.toStr();

    if(!m_materialFile.empty())
        ctormap["materials"] = m_materialFile;

    return ctormap;
}

void Mesh::setBillBoard(Vector2b billBoard)
{
    this->m_billBoard = billBoard;
}

Vector2b Mesh::getBillBoard() const
{
    return m_billBoard;
}

void Mesh::setVertexScale(Vector3f vertexScale)
{
    if(math::isAnyZero(vertexScale))
        return;

    m_vertexScale = vertexScale;
}

Vector3f Mesh::getVertexScale() const
{
    return m_vertexScale;
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

    ressPath.push_back(manager[this]);

    return ressPath;
}

void Mesh::generateMulTexCoord()
{
    Material* mat = m_materials.begin()->second;

    for(Texture::Map::iterator itt = mat->m_textures.begin(); itt != mat->m_textures.end(); itt++)
        m_hardwareBuffer->newMultiTexCoord(itt->first);

    m_hardwareBuffer->compile();
}
