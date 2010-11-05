/*
 * File:   Mesh.cpp
 * Author: b4n92uid
 *
 * Created on 28 avril 2010, 18:36
 */

#include "Mesh.h"
#include "ParallelScene.h"
#include "SceneManager.h"
#include "Tools.h"

using namespace tbe;
using namespace tbe::scene;
using namespace std;

Mesh::Mesh()
{
    m_parent = NULL;

    m_triangulate = true;
    m_withNormal = false;
    m_withTexCoord = false;
}

Mesh::Mesh(const Mesh& mesh)
{
    m_parent = NULL;

    *this = mesh;
}

Mesh::~Mesh()
{
    for(Material::Map::iterator it = m_materials.begin(); it != m_materials.end(); it++)
        delete it->second;

    for(unsigned i = 0; i < m_childs.size(); i++)
        delete m_childs[i];
}

bool Mesh::operator=(const Mesh& mesh)
{
    m_triangulate = mesh.m_triangulate;
    m_withNormal = mesh.m_withNormal;
    m_withTexCoord = mesh.m_withTexCoord;

    m_hardwareBuffer = mesh.m_hardwareBuffer;

    for(Material::Map::const_iterator it = mesh.m_materials.begin(); it != mesh.m_materials.end(); it++)
        m_materials[it->first] = new Material(*it->second);

    m_renderProess = mesh.m_renderProess;

    for(unsigned i = 0; i < m_renderProess.size(); i++)
        m_renderProess[i].parent = this;

    return true;
}

void Mesh::ComputeAabb()
{
    m_aabb.Clear();

    Vertex* vertex = m_hardwareBuffer.Lock();

    for(unsigned i = 0; i < m_hardwareBuffer.GetVertexCount(); i++)
        m_aabb.Count(vertex[i].pos);

    m_hardwareBuffer.UnLock();
}

void Mesh::ComputeTangent()
{
    Vertex* vertex = m_hardwareBuffer.Lock();

    unsigned vertexCount = m_hardwareBuffer.GetVertexCount();

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
        vertex[i].tangent = (t - n * Vector3f::Dot(n, t));
        vertex[i].tangent.Normalize();
    }

    m_hardwareBuffer.UnLock();
}

void Mesh::ComputeAocc()
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

void Mesh::Render(Material* material, unsigned offset, unsigned size)
{
    GLint tangentAttribIndex = -1, aoccAttribIndex = -1;

    m_hardwareBuffer.BindBuffer();

    glPushAttrib(GL_ENABLE_BIT);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(true);

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

    // Sort polygones ----------------------------------------------------------

    if(material->m_renderFlags & Material::VERTEX_SORT)
    {
        if(material->m_frameSortWait <= 0)
        {
            static DepthSortVertexFunc cmp;
            cmp.camPos = m_parallelScene->GetSceneManager()->GetCurCamera()->GetPos();
            cmp.meshPos = m_matrix.GetPos();

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

            m_hardwareBuffer.BindTangent(true, tangentAttribIndex);

            #if 0
            // NOTE Debug render tangent space

            Vertex* vertexes = static_cast<Vertex*>(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE));

            glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
            glDisable(GL_LIGHTING);
            glDisable(GL_TEXTURE_2D);
            glDisable(GL_BLEND);

            for(unsigned i = 0; i < m_hardwareBuffer.GetVertexCount(); i++)
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

            m_hardwareBuffer.BindAocc(true, aoccAttribIndex);
        }

        material->m_shader.Use(true);
    }

    // Texture -----------------------------------------------------------------

    if(material->m_renderFlags & Material::TEXTURE)
    {
        m_hardwareBuffer.BindTexture();

        Texture::Map& textures = material->m_textures;

        for(Texture::Map::iterator itt = textures.begin();
            itt != textures.end(); itt++)
        {
            if(!itt->second)
                continue;

            unsigned textureIndex = GL_TEXTURE0 + itt->first;

            glClientActiveTexture(textureIndex);
            glActiveTexture(textureIndex);
            glEnable(GL_TEXTURE_2D);

            itt->second.Use(true);
        }
    }

    // Lumiere -----------------------------------------------------------------

    if(material->m_renderFlags & Material::LIGHT)
    {
        m_hardwareBuffer.BindNormal();

        glEnable(GL_LIGHTING);

        glMaterialfv(GL_FRONT, GL_AMBIENT, material->m_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, material->m_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, material->m_specular);
        glMaterialf(GL_FRONT, GL_SHININESS, material->m_shininess);
    }

    // Color -------------------------------------------------------------------

    if(material->m_renderFlags & Material::COLOR)
    {
        m_hardwareBuffer.BindColor();

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
        glBlendFunc(GL_DST_ALPHA, GL_SRC_COLOR);

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

    m_hardwareBuffer.Render(material->m_faceType, offset, size);

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
            m_hardwareBuffer.BindTangent(false, tangentAttribIndex);

        if(material->m_renderFlags & Material::AOCC)
            m_hardwareBuffer.BindAocc(false, aoccAttribIndex);


        material->m_shader.Use(false);
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

        m_hardwareBuffer.BindTexture(false);
    }

    if(material->m_renderFlags & Material::LIGHT)
    {
        m_hardwareBuffer.BindNormal(false);
    }

    if(material->m_renderFlags & Material::COLOR)
    {
        m_hardwareBuffer.BindColor(false);
    }

    m_hardwareBuffer.BindBuffer(false);

    glPopAttrib();
}

void Mesh::Render()
{
    glPushMatrix();
    glMultMatrixf(m_matrix);

    for(unsigned i = 0; i < m_childs.size(); i++)
        m_childs[i]->Render();

    if(m_renderProess.empty())
    {
        Material defaultMateral;
        Render(&defaultMateral, 0, m_hardwareBuffer.GetVertexCount());
    }

    else
    {
        std::sort(m_renderProess.begin(), m_renderProess.end(), RenderProcessSortFunc);

        for(unsigned i = 0; i < m_renderProess.size(); i++)
        {
            std::string name = m_renderProess[i].applyMaterial;
            Material* mat = m_materials[name];

            Render(mat, m_renderProess[i].offset, m_renderProess[i].size);
        }
    }

    glPopMatrix();
}

Vector3f RayCastTriangle(Vector3f p, Vector3f d, Vector3f v0, Vector3f v1, Vector3f v2)
{
    Vector3f e1, e2, h, s, q;

    float a, f, u, v;

    e1 = v1 - v0;
    e2 = v2 - v0;

    h = Vector3f::Cross(d, e2);

    a = Vector3f::Dot(e1, h);

    if(a > -0.00001 && a < 0.00001)
        return p;

    f = 1.0f / a;
    s = p - v0;
    u = f * (Vector3f::Dot(s, h));

    if(u < 0.0 || u > 1.0)
        return p;

    q = Vector3f::Cross(s, e1);

    v = f * Vector3f::Dot(d, q);

    if(v < 0.0 || u + v > 1.0)
        return p;

    // at this stage we can compute t to find out where
    // the intersection point is on the line
    float t = f * Vector3f::Dot(e2, q);

    if(t > 0.00001) // ray intersection
        return p + Vector3f::Normalize(d) * t;

        // this means that there is a line intersection
        // but not a ray intersection
    else
        return p;
}

Vector3f Mesh::RayCast(Vector3f rayStart, Vector3f rayDiri)
{
    Vertex* vertex = m_hardwareBuffer.Lock();

    const unsigned vertexCount = m_hardwareBuffer.GetVertexCount();

    Vector3f intersect = 0;

    for(unsigned i = 0; i < vertexCount; i += 3)
    {
        intersect = RayCastTriangle(rayStart,
                                    rayDiri,
                                    vertex[i].pos,
                                    vertex[i + 1].pos,
                                    vertex[i + 2].pos);

        if(intersect != rayStart)
            break;
    }

    m_hardwareBuffer.UnLock();

    return intersect;
}

Vector3f Mesh::FindFloor(float x, float z)
{
    return RayCast(Vector3f(x, m_aabb.max.y + 1, z), Vector3f(0, -1, 0));
}

bool Mesh::IsTransparent()
{
    for(Material::Map::iterator itt = m_materials.begin(); itt != m_materials.end(); itt++)
        if(itt->second->IsTransparent())
            return true;

    return false;
}

void Mesh::SetParent(Mesh* parent)
{
    if(m_parent)
        m_parent->ReleaseChild(this);

    parent->AddChild(this);
}

Mesh* Mesh::GetParent()
{
    return m_parent;
}

void Mesh::AddChild(Mesh* child)
{
    if(find(m_childs.begin(), m_childs.end(), child) == m_childs.end())
        m_childs.push_back(child);

    else
        throw Exception("Mesh::AddChild; child already exist");
}

Mesh* Mesh::ReleaseChild(Mesh* child)
{
    Mesh::Array::iterator it = find(m_childs.begin(), m_childs.end(), child);

    if(it == m_childs.end())
        return NULL;

    Mesh* ret = *it;
    m_childs.erase(it);

    return ret;
}

Mesh* Mesh::ReleaseChild(unsigned index)
{
    if(index >= m_childs.size())
        throw Exception("Mesh::ReleaseChild; index out of range %d", index);

    Mesh* ret = m_childs[index];
    m_childs.erase(m_childs.begin() + index);

    return ret;
}

Mesh* Mesh::GetChild(unsigned index)
{
    if(index >= m_childs.size())
        throw Exception("Mesh::GetChild; index out of range %d", index);

    return m_childs[index];
}

void Mesh::AddMaterial(std::string name, Material* material)
{
    m_materials[name] = material;

    material->SetName(name);
}

void Mesh::DeleteMaterial(std::string name)
{
    m_materials.erase(name);
}

unsigned Mesh::GetMaterialCount()
{
    return m_materials.size();
}

void Mesh::ApplyColor(std::string materialName, Vector4f color)
{
    Vector2i::Array apply = GetMaterialApply(materialName);

    Vertex* vertex = m_hardwareBuffer.Lock();

    for(unsigned i = 0; i < apply.size(); i++)
        for(int j = 0; j < apply[i].y; j++)
            vertex[j + apply[i].x].color = color;

    m_hardwareBuffer.UnLock();
}

void Mesh::ApplyShader(Shader shader)
{
    for(Material::Map::iterator itt = m_materials.begin(); itt != m_materials.end(); itt++)
    {
        itt->second->SetShader(shader);
        itt->second->Enable(Material::SHADER);
    }
}

Material::Array Mesh::GetAllMaterial()
{
    std::vector<Material*> ret;
    ret.reserve(m_materials.size());

    for(Material::Map::iterator itt = m_materials.begin(); itt != m_materials.end(); itt++)
        ret.push_back(itt->second);

    return ret;
}

Vector2i::Array Mesh::GetMaterialApply(std::string name)
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

Material* Mesh::GetMaterial(std::string name)
{
    if(m_materials.find(name) == m_materials.end())
        throw tbe::Exception("Mesh::GetMaterial; Material not found (%s)", name.c_str());

    return m_materials[name];
}

Material* Mesh::ReleaseMaterial(std::string name)
{
    if(m_materials.find(name) == m_materials.end())
        throw tbe::Exception("Mesh::ReleaseMaterial; Material not found (%s)", name.c_str());

    Material* toRet = m_materials[name];
    m_materials.erase(name);
    return toRet;
}

void Mesh::ApplyMaterial(std::string name, unsigned offset, unsigned size)
{
    RenderProcess rp = {this, name, offset, size};
    m_renderProess.push_back(rp);
}

void Mesh::ApplyMaterial(Material* material, unsigned offset, unsigned size)
{
    for(Material::Map::iterator it = m_materials.begin();
        it != m_materials.end(); it++)
        if(it->second == material)
        {
            RenderProcess rp = {this, it->first, offset, size};
            m_renderProess.push_back(rp);
            return;
        }

    throw tbe::Exception("Mesh::ApplyMaterial; Material ptr not found");
}

HardwareBuffer& Mesh::GetHardwareBuffer()
{
    return m_hardwareBuffer;
}

bool Mesh::CheckHardware()
{
    return GLEE_ARB_vertex_buffer_object;
}
