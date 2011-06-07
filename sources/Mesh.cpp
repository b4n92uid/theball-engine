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

Mesh::Mesh(MeshParallelScene* scene)
{
    m_triangulate = true;
    m_withNormal = false;
    m_withTexCoord = false;
    m_visible = true;
    m_outputMaterial = false;

    Node::m_parallelScene = m_parallelScene = scene;
    m_sceneManager = m_parallelScene->getSceneManager();

    m_parallelScene->registerNode(this);

    m_opacity = 1;
}

Mesh::Mesh(const Mesh& copy) : Node(copy)
{
    this->copy(copy);

    m_parallelScene->registerNode(this);
}

Mesh::~Mesh()
{
    for(Material::Map::iterator it = m_materials.begin(); it != m_materials.end(); ++it)
        delete it->second;

    m_parallelScene->unregisterNode(this);
}

Mesh& Mesh::operator=(const Mesh& copy)
{
    Node::operator=(copy);

    return this->copy(copy);
}

Mesh& Mesh::copy(const Mesh& copy)
{
    Node::m_parallelScene = m_parallelScene = copy.m_parallelScene;

    m_triangulate = copy.m_triangulate;
    m_withNormal = copy.m_withNormal;
    m_withTexCoord = copy.m_withTexCoord;
    m_visible = copy.m_visible;
    m_opacity = copy.m_opacity;
    m_outputMaterial = copy.m_outputMaterial;

    m_hardwareBuffer = copy.m_hardwareBuffer;

    for(Material::Map::const_iterator it = m_materials.begin(); it != m_materials.end(); ++it)
        delete it->second;

    m_materials.clear();

    for(Material::Map::const_iterator it = copy.m_materials.begin(); it != copy.m_materials.end(); ++it)
        m_materials[it->first] = new Material(*it->second);

    m_renderProess = copy.m_renderProess;

    for(unsigned i = 0; i < m_renderProess.size(); i++)
        m_renderProess[i].parent = this;

    return *this;
}

Mesh* Mesh::clone()
{
    return new Mesh(*this);
}

AABB Mesh::getAbsolutAabb()
{
    unsigned vertexCount = m_hardwareBuffer.getVertexCount();

    AABB aabb;

    Matrix4 mat = getAbsoluteMatrix();
    Vertex* vertex = m_hardwareBuffer.lock();

    for(unsigned i = 0; i < vertexCount; i++)
        aabb.count(mat * vertex[i].pos);

    m_hardwareBuffer.unlock();

    return aabb;
}

void Mesh::computeAabb()
{
    m_aabb.clear();

    unsigned vertexCount = m_hardwareBuffer.getVertexCount();

    Vertex* vertex = m_hardwareBuffer.lock();

    for(unsigned i = 0; i < vertexCount; i++)
        m_aabb.count(vertex[i].pos);

    m_hardwareBuffer.unlock();
}

void Mesh::computeTangent()
{
    Vertex* vertex = m_hardwareBuffer.lock();

    unsigned vertexCount = m_hardwareBuffer.getVertexCount();

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

    m_hardwareBuffer.unlock();
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
        return (meshPos + (face1.v1.pos + face1.v2.pos + face1.v3.pos) / 3.0f - camPos) >
                (meshPos + (face2.v1.pos + face2.v2.pos + face2.v3.pos) / 3.0f - camPos);
    }

    Vector3f camPos;
    Vector3f meshPos;
};

void Mesh::render(Material* material, unsigned offset, unsigned size)
{
    GLint tangentAttribIndex = -1, aoccAttribIndex = -1;

    m_hardwareBuffer.bindBuffer();

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
            TriangleFace* end = start + size / 3;

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
        // Tangent -------------------------------------------------------------

        if(material->m_renderFlags & Material::TANGENT)
        {
            tangentAttribIndex = glGetAttribLocation(material->m_shader, material->m_tangentLocation.c_str());

            if(tangentAttribIndex == -1)
                throw Exception("MeshConstruction::Render; Invalid tangent location (%s)", material->m_tangentLocation.c_str());

            m_hardwareBuffer.bindTangent(true, tangentAttribIndex);

            #if 0
            // NOTE Debug render tangent space

            Vertex* vertexes = static_cast<Vertex*>(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE));

            glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
            glDisable(GL_LIGHTING);
            glDisable(GL_TEXTURE_2D);
            glDisable(GL_BLEND);

            for(unsigned i = 0; i < m_hardwareBuffer.getVertexCount(); i++)
            {
                glPushMatrix();
                glTranslatef(vertexes[i].pos.x, vertexes[i].pos.y, vertexes[i].pos.z);

                glBegin(GL_LINES);

                glColor3f(1, 0, 0);
                glVertex3f(0, 0, 0);
                glVertex3fv(vertexes[i].tangent * 0.1f);

                glColor3f(0, 1, 0);
                glVertex3f(0, 0, 0);
                glVertex3fv(vertexes[i].normal * 0.1f);

                glEnd();

                glPopMatrix();
            }

            glPopAttrib();

            glUnmapBuffer(GL_ARRAY_BUFFER);
            #endif
        }

        // Amobient occlusion --------------------------------------------------

        if(material->m_renderFlags & Material::AOCC)
        {
            aoccAttribIndex = glGetAttribLocation(material->m_shader, material->m_aoccLocation.c_str());

            if(aoccAttribIndex == -1)
                throw Exception("MeshConstruction::Render; Invalid tangent location (%s)", material->m_aoccLocation.c_str());

            m_hardwareBuffer.bindAocc(true, aoccAttribIndex);
        }

        material->m_shader.use(true);
    }

    // Texture -----------------------------------------------------------------

    if(material->m_renderFlags & Material::TEXTURE)
    {
        Texture::Map& textures = material->m_textures;

        for(Texture::Map::iterator itt = textures.begin();
            itt != textures.end(); itt++)
        {
            if(!itt->second)
                continue;

            unsigned textureIndex = GL_TEXTURE0 + itt->first;

            glClientActiveTexture(textureIndex);
            m_hardwareBuffer.bindTexture();

            glActiveTexture(textureIndex);
            glEnable(GL_TEXTURE_2D);

            itt->second.use(true);

            unsigned multexb = itt->second.getMulTexBlend();

            if(multexb == Texture::REPLACE)
                glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

            else if(multexb == Texture::ADDITIVE)
                glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);

            else if(multexb == Texture::MODULATE)
                glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        }
    }

    // Lumiere -----------------------------------------------------------------

    if(material->m_renderFlags & Material::LIGHT)
    {
        m_hardwareBuffer.bindNormal();

        glEnable(GL_LIGHTING);

        glMaterialfv(GL_FRONT, GL_AMBIENT, material->m_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, material->m_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, material->m_specular);
        glMaterialf(GL_FRONT, GL_SHININESS, material->m_shininess);
    }

    // Color -------------------------------------------------------------------

    if(material->m_renderFlags & Material::COLOR)
    {
        m_hardwareBuffer.bindColor();

        glEnable(GL_COLOR_MATERIAL);
    }

    // Blend -------------------------------------------------------------------

    if(material->m_renderFlags & Material::BLEND_ADD)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
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

    // Line width --------------------------------------------------------------

    if(material->m_lineWidth)
        glLineWidth(material->m_lineWidth);

    // Rendue ------------------------------------------------------------------

    m_hardwareBuffer.render(material->m_faceType, offset, size);

    // Réstorations ------------------------------------------------------------

    if(material->m_renderFlags & Material::VERTEX_SORT_CULL_TRICK)
    {
        glCullFace(GL_BACK);
        glDrawArrays(material->m_faceType, offset, size);

        glDepthMask(true);
    }

    if(material->m_renderFlags & Material::SHADER)
    {
        if(material->m_renderFlags & Material::TANGENT)
            m_hardwareBuffer.bindTangent(false, tangentAttribIndex);

        if(material->m_renderFlags & Material::AOCC)
            m_hardwareBuffer.bindAocc(false, aoccAttribIndex);


        material->m_shader.use(false);
    }

    if(material->m_renderFlags & Material::TEXTURE)
    {
        Texture::Map& textures = material->m_textures;

        for(Texture::Map::reverse_iterator itt = textures.rbegin();
            itt != textures.rend(); itt++)
        {
            if(!itt->second)
                continue;

            unsigned textureIndex = GL_TEXTURE0 + itt->first;

            glClientActiveTexture(textureIndex);
            glActiveTexture(textureIndex);
            glDisable(GL_TEXTURE_2D);
        }

        m_hardwareBuffer.bindTexture(false);
    }

    if(material->m_renderFlags & Material::LIGHT)
    {
        m_hardwareBuffer.bindNormal(false);
    }

    if(material->m_renderFlags & Material::COLOR)
    {
        m_hardwareBuffer.bindColor(false);
    }

    m_hardwareBuffer.bindBuffer(false);

    // NOTE OpenGL 1.4
    glColor4f(1, 1, 1, 1);

    glPopAttrib();
}

void Mesh::render()
{
    if(!m_enable || m_hardwareBuffer.isEmpty() || !m_visible)
        return;

    glPushMatrix();

    if(m_parent)
        glMultMatrixf(m_parent->getAbsoluteMatrix());

    glMultMatrixf(m_matrix);

    if(m_renderProess.empty())
    {
        Material defaultMateral;
        render(&defaultMateral, 0, m_hardwareBuffer.getVertexCount());
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

Vector3f RayCastTriangle(Vector3f p, Vector3f d, Vector3f v0, Vector3f v1, Vector3f v2)
{
    Vector3f e1, e2, h, s, q;

    float a, f, u, v;

    e1 = v1 - v0;
    e2 = v2 - v0;

    h = Vector3f::cross(d, e2);

    a = Vector3f::dot(e1, h);

    if(a > -0.00001 && a < 0.00001)
        return p;

    f = 1.0f / a;
    s = p - v0;
    u = f * (Vector3f::dot(s, h));

    if(u < 0.0 || u > 1.0)
        return p;

    q = Vector3f::cross(s, e1);

    v = f * Vector3f::dot(d, q);

    if(v < 0.0 || u + v > 1.0)
        return p;

    // at this stage we can compute t to find out where
    // the intersection point is on the line
    float t = f * Vector3f::dot(e2, q);

    if(t > 0.00001) // ray intersection
        return p + Vector3f::normalize(d) * t;

        // this means that there is a line intersection
        // but not a ray intersection
    else
        return p;
}

bool Mesh::rayCast(Vector3f rayStart, Vector3f rayDiri, Vector3f& intersect, bool global)
{
    Vertex* vertex = m_hardwareBuffer.lock();

    Matrix4 absmat = getAbsoluteMatrix();

    const unsigned vertexCount = m_hardwareBuffer.getVertexCount();

    Vector3f::Array hits;

    for(unsigned i = 0; i < vertexCount; i += 3)
    {
        Vector3f pos0 = vertex[i].pos,
                pos1 = vertex[i + 1].pos,
                pos2 = vertex[i + 2].pos;

        if(global)
        {
            pos0 = absmat * pos0;
            pos1 = absmat * pos1;
            pos2 = absmat * pos2;
        }

        Vector3f intr = RayCastTriangle(rayStart, rayDiri, pos0, pos1, pos2);

        if(intr != rayStart)
            hits.push_back(intr);
    }

    m_hardwareBuffer.unlock();

    if(hits.empty())
    {
        return false;
    }

    else
    {
        intersect = *std::max_element(hits.begin(), hits.end());

        return true;
    }
}

bool Mesh::findFloor(Vector3f& pos, bool global)
{
    float y = global ? (getAbsoluteMatrix() * m_aabb.max).y + 1 : m_aabb.max.y + 1;

    return rayCast(Vector3f(pos.x, y, pos.z),
                   Vector3f(0, -1, 0), pos, global);
}

bool Mesh::isTransparent()
{
    for(Material::Map::iterator itt = m_materials.begin(); itt != m_materials.end(); itt++)
        if(itt->second->isTransparent())
            return true;

    return false;
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

void Mesh::setOpacity(float opacity)
{
    this->m_opacity = opacity;

    Vertex* vertex = m_hardwareBuffer.lock();

    unsigned count = m_hardwareBuffer.getVertexCount();
    for(unsigned i = 0; i < count; i++)
        vertex[i].color.w = m_opacity;

    m_hardwareBuffer.unlock();
}

float Mesh::getOpacity() const
{
    return m_opacity;
}

void Mesh::setColor(Vector4f color)
{
    Vertex* vertex = m_hardwareBuffer.lock();

    unsigned count = m_hardwareBuffer.getVertexCount();
    for(unsigned i = 0; i < count; i++)
        vertex[i].color = color;

    m_hardwareBuffer.unlock();
}

Vector4f Mesh::getColor()
{
    return m_color;
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

HardwareBuffer& Mesh::getHardwareBuffer()
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

Node::CtorMap Mesh::constructionMap(std::string root)
{
    Node::CtorMap ctormap = Node::constructionMap(root);

    if(m_outputMaterial)
    {
        for(Material::Map::iterator it = m_materials.begin(); it != m_materials.end(); it++)
        {
            ctormap["!" + it->first + ":alphaThershold"] = tools::numToStr(it->second->m_alphaThershold);
            ctormap["!" + it->first + ":blendMod"] = tools::numToStr(it->second->isEnable(Material::BLEND_MOD));
            ctormap["!" + it->first + ":blendMul"] = tools::numToStr(it->second->isEnable(Material::BLEND_MUL));
            ctormap["!" + it->first + ":blendAdd"] = tools::numToStr(it->second->isEnable(Material::BLEND_ADD));
            ctormap["!" + it->first + ":alpha"] = tools::numToStr(it->second->isEnable(Material::ALPHA));
            ctormap["!" + it->first + ":frontFaceCull"] = tools::numToStr(it->second->isEnable(Material::FRONTFACE_CULL));
            ctormap["!" + it->first + ":backFaceCull"] = tools::numToStr(it->second->isEnable(Material::BACKFACE_CULL));
            ctormap["!" + it->first + ":lighted"] = tools::numToStr(it->second->isEnable(Material::LIGHT));
            ctormap["!" + it->first + ":textured"] = tools::numToStr(it->second->isEnable(Material::TEXTURE));
            ctormap["!" + it->first + ":colored"] = tools::numToStr(it->second->isEnable(Material::COLOR));

            unsigned txcount = it->second->getTexturesCount();

            for(unsigned i = 0; i < txcount; i++)
            {
                string key = "!" + it->first + ":texture:" + tools::numToStr(i);

                Texture tex = it->second->getTexture(i);

                ctormap[key] = tex.getFilename();
                ctormap[key] += ";";

                if(tex.getMulTexBlend() == Texture::MODULATE)
                    ctormap[key] += "modulate";
                if(tex.getMulTexBlend() == Texture::ADDITIVE)
                    ctormap[key] += "additive";
                if(tex.getMulTexBlend() == Texture::REPLACE)
                    ctormap[key] += "replace";
            }
        }

        ctormap["!opacity"] = tools::numToStr(m_opacity);
    }

    return ctormap;
}

void Mesh::setOutputMaterial(bool outputMaterial)
{
    this->m_outputMaterial = outputMaterial;
}

bool Mesh::isOutputMaterial() const
{
    return m_outputMaterial;
}
