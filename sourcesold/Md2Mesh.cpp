#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glext.h>

#include "md2mesh.h"

#define MD2_IDENT   (('2'<<24) + ('P'<<16) + ('D'<<8) + 'I')
#define MD2_VERSION 8

using namespace std;
using namespace tbe;
using namespace tbe::scene;

MD2Mesh::MD2Mesh()
{
    m_size = 1.0;
    m_interp = 0.0;
    m_framecur = 0;
    m_framestart = 0;
    m_frameend = 0;
    m_framenext = 0;
    m_interpspeed = 0.1;

    m_tris = NULL;
    m_texcoords = NULL;
    m_frames = NULL;
    m_skins = NULL;
    m_glcmds = NULL;

}

MD2Mesh::MD2Mesh(const std::string& path)
{
    m_size = 1.0;
    m_glcmds = NULL;
    m_interp = 0.0;
    m_framecur = 0;
    m_framestart = 0;
    m_frameend = 0;
    m_framenext = 0;
    m_interpspeed = 0.1;

    m_tris = NULL;
    m_texcoords = NULL;
    m_frames = NULL;
    m_skins = NULL;
    m_glcmds = NULL;

    Open(path);
}

MD2Mesh::~MD2Mesh()
{
    Free();
}

MD2Mesh& MD2Mesh::operator=(MD2Mesh& c)
{
    m_header = c.m_header;
    m_tris = c.m_tris;
    m_texcoords = c.m_texcoords;
    m_frames = c.m_frames;
    m_skins = c.m_skins;

    m_glcmds = new int[c.m_header.m_num_glcmds];
    memcpy(m_glcmds, c.m_glcmds, c.m_header.m_num_glcmds * sizeof(int));

    return *this;
}

void MD2Mesh::Free()
{
    if(m_skins)
        delete[] m_skins;

    if(m_texcoords)
        delete[] m_texcoords;

    if(m_glcmds)
        delete[] m_glcmds;

    if(m_tris)
        delete[] m_tris;

    if(m_frames)
    {
        for(int i = 0; i < m_header.m_num_frames; i++)
            if(m_frames[i].m_vertex)
                delete[] m_frames[i].m_vertex;

        delete[] m_frames;
    }
}

void MD2Mesh::Open(const std::string& path)
{
    FILE* file = fopen(path.c_str(), "rb");

    // Verfication
    if(!file)
        throw tbe::Exception("MD2Mesh::Open\nOpen file error (%s)\n\n", path.c_str());

    Free();

    fread(&m_header, sizeof(MD2Header), 1, file);

    if(m_header.m_id != MD2_IDENT)
        throw tbe::Exception("MD2Mesh::Open\nnNt a valid MD2 file (%s)\nMust be 8\n\n", path.c_str());

    if(m_header.m_version != MD2_VERSION)
        throw tbe::Exception("MD2Mesh::Open\nNot a valid MD2 version (%s)\nMust be 8\n\n", path.c_str());

    // Chargement en memoire
    m_skins = new MD2Skin[m_header.m_num_skins];
    m_texcoords = new MD2TexCoord[m_header.m_num_texcoord];
    m_tris = new MD2Tri[m_header.m_num_tris];
    m_glcmds = new int[m_header.m_num_glcmds];
    m_frames = new MD2Frame[m_header.m_num_frames];

    fseek(file, m_header.m_offset_skins, SEEK_SET);
    fread(m_skins, sizeof(MD2Skin), m_header.m_num_skins, file);

    fseek(file, m_header.m_offset_texcoord, SEEK_SET);
    fread(m_texcoords, sizeof(MD2TexCoord), m_header.m_num_texcoord, file);

    fseek(file, m_header.m_offset_tri, SEEK_SET);
    fread(m_tris, sizeof(MD2Tri), m_header.m_num_tris, file);

    fseek(file, m_header.m_offset_glcmds, SEEK_SET);
    fread(m_glcmds, sizeof(int), m_header.m_num_glcmds, file);

    fseek(file, m_header.m_offset_frames, SEEK_SET);

    for(int i = 0; i < m_header.m_num_frames; i++)
    {
        m_frames[i].m_vertex = new MD2Vertex[m_header.m_num_vertex];

        fread(m_frames[i].m_size, sizeof(float)*3, 1, file);
        fread(m_frames[i].m_translate, sizeof(float)*3, 1, file);
        fread(m_frames[i].m_name, sizeof(char), 16, file);
        fread(m_frames[i].m_vertex, sizeof(MD2Vertex), m_header.m_num_vertex, file);
    }

    // Fermeture du fichier
    fclose(file);

    // Recuperation des nom des frames
    std::string lastframename;

    for(int i = 0; i < m_header.m_num_frames; i++)
    {
        std::string framename = m_frames[i].m_name;
        framename = framename.substr(0, framename.length() - 3);

        if(lastframename == framename)
            continue;

        lastframename = framename;

        m_anim.push_back(framename);
    }

    // Enregistrement du chemin du fichier
    m_md2filepath = path;

    // Specifier lq premiere animation
    SetAnim(0);
}

void MD2Mesh::ObjectRender()
{
    glCullFace(GL_FRONT);

    RenderFrame(m_framecur, m_framenext, m_interp);

    m_interp += m_interpspeed;

    if(m_interp >= 1.0)
    {
        m_interp = 0.0;

        m_framenext++;
        m_framecur = m_framenext - 1;

        if(m_framenext == m_frameend + 1)
            m_framenext = m_framestart;

        if(m_framecur == m_frameend + 1)
            m_framecur = m_framestart;
    }
}

void MD2Mesh::RenderFrame(int frameA, int frameB, float interp)
{
    glRotatef(-90, 0, 1, 0);
    glRotatef(-90, 1, 0, 0);

    // Check if frames are valid
    if(frameA < 0 || frameB < 0)
        return;

    if(frameA > m_header.m_num_frames - 1 || frameB > m_header.m_num_frames - 1)
        return;

    // Render model
    glBegin(GL_TRIANGLES);

    // Render each triangle
    for(int i = 0; i < m_header.m_num_tris; ++i)
    {
        // Render each vertex of this triangle
        for(int j = 0; j < 3; ++j)
        {
            MD2Frame *pFrameA = &m_frames[frameA];
            MD2Frame *pFrameB = &m_frames[frameB];

            MD2Vertex *pVertA = &pFrameA->m_vertex[m_tris[i].m_vertex[j]];
            MD2Vertex *pVertB = &pFrameB->m_vertex[m_tris[i].m_vertex[j]];

            MD2TexCoord *pTexCoords = &m_texcoords[m_tris[i].m_texcoord[j]];

            // Compute final texture coords.
            GLfloat s = static_cast<GLfloat>(pTexCoords->s) / m_header.m_skinwidth;
            GLfloat t = static_cast<GLfloat>(pTexCoords->t) / m_header.m_skinheight;

            glTexCoord2f(s, t);

            // Compute interpolated normal vector
            const GLfloat *normA = m_kAnorms[pVertA->normalIndex];
            const GLfloat *normB = m_kAnorms[pVertB->normalIndex];

            float n[3];

            n[0] = normA[0] + interp * (normB[0] - normA[0]);
            n[1] = normA[1] + interp * (normB[1] - normA[1]);
            n[2] = normA[2] + interp * (normB[2] - normA[2]);

            glNormal3fv(n);

            // Compute final vertex position
            float vecA[3], vecB[3], v[3];

            // First, uncompress vertex positions
            vecA[0] = pFrameA->m_size[0] * pVertA->m_vertex[0] + pFrameA->m_translate[0];
            vecA[1] = pFrameA->m_size[1] * pVertA->m_vertex[1] + pFrameA->m_translate[1];
            vecA[2] = pFrameA->m_size[2] * pVertA->m_vertex[2] + pFrameA->m_translate[2];

            vecB[0] = pFrameB->m_size[0] * pVertB->m_vertex[0] + pFrameB->m_translate[0];
            vecB[1] = pFrameB->m_size[1] * pVertB->m_vertex[1] + pFrameB->m_translate[1];
            vecB[2] = pFrameB->m_size[2] * pVertB->m_vertex[2] + pFrameB->m_translate[2];

            // Linear interpolation and scaling
            v[0] = (vecA[0] + interp * (vecB[0] - vecA[0])) * m_size;
            v[1] = (vecA[1] + interp * (vecB[1] - vecA[1])) * m_size;
            v[2] = (vecA[2] + interp * (vecB[2] - vecA[2])) * m_size;

            glVertex3fv(v);
        }
    }

    glEnd();
}

void MD2Mesh::RenderFrame(int frame)
{
    glRotatef(-90, 0, 1, 0);
    glRotatef(-90, 1, 0, 0);

    // v�rification de la validit� de frame
    if(frame < 0 || frame > m_header.m_num_frames - 1)
        throw tbe::Exception("MD2 File Error", "frame out of range (%d)", frame);

    // activation de la texture du mod�le
    m_texture.Use();

    // dessin du mod�le
    glBegin(GL_TRIANGLES);

    // dessine chaque triangle
    for(int i = 0; i < m_header.m_num_tris; i++)
    {
        // dessigne chaque vertex du triangle
        for(int k = 0; k < 3; k++)
        {
            MD2Frame *pFrame = &m_frames[frame];
            MD2Vertex *pVert = &m_frames[frame].m_vertex[m_tris[i].m_vertex[k]];

            // [texture]
            GLfloat s = (GLfloat)m_texcoords[m_tris[i].m_texcoord[k]].s / m_header.m_skinwidth;
            GLfloat t = (GLfloat)m_texcoords[m_tris[i].m_texcoord[k]].t / m_header.m_skinheight;

            glTexCoord2f(s, t);

            // [normale]
            glNormal3fv(m_kAnorms[pVert->normalIndex]);

            // [position]
            float vertex[3];

            // calcul de la position absolue du vertex et redimensionnement
            vertex[0] = (pFrame->m_size[0] * pVert->m_vertex[0] + pFrame->m_translate[0]) * m_size;
            vertex[1] = (pFrame->m_size[1] * pVert->m_vertex[1] + pFrame->m_translate[1]) * m_size;
            vertex[2] = (pFrame->m_size[2] * pVert->m_vertex[2] + pFrame->m_translate[2]) * m_size;

            glVertex3fv(vertex);
        }
    }

    glEnd();
}

void MD2Mesh::SetDefaultTexture()
{
    std::string texFile = m_skins[0].m_name;

    int texFilePos = texFile.find_last_of('\\');

    if(texFilePos == std::string::npos)
        texFilePos = texFile.find_last_of('/');

    int md2FilePos = m_md2filepath.find_last_of('\\');

    if(md2FilePos == std::string::npos)
        md2FilePos = m_md2filepath.find_last_of('/');

    m_texture = m_md2filepath.substr(0, md2FilePos + 1) + texFile.substr(texFilePos + 1);
}

void MD2Mesh::SetAnim(int frameStart, int frameEnd)
{
    m_framestart = frameStart;
    m_framecur = frameStart;
    m_framenext = frameStart + 1;
    m_frameend = frameEnd;
}

void MD2Mesh::SetAnim(int index)
{
    SetAnim(m_anim[index]);
}

void MD2Mesh::SetAnim(std::string name)
{
    using namespace std;

    if(m_curanimname == name)
        return;

    else
        m_curanimname = name;

    bool Setstart = false;

    for(int i = 0; i < m_header.m_num_frames; i++)
    {
        std::string framename = m_frames[i].m_name;
        framename = framename.substr(0, framename.length() - 3);

        if(framename == name && !Setstart)
        {
            m_framestart = i;
            m_framenext = i;

            Setstart = true;
        }

        else if(framename != name && Setstart)
        {
            m_frameend = i - 1;
            break;
        }
    }
}

void MD2Mesh::SetSize(float value)
{
    m_size = value;
}

void MD2Mesh::SetAnimSpeed(float value)
{
    m_interpspeed = value;
}

bool MD2Mesh::isFrameEnd()
{
    return(m_framecur == m_frameend);
}

float MD2Mesh::m_kAnorms[162][3] = {
    { -0.525731f, 0.000000f, 0.850651f},
    { -0.442863f, 0.238856f, 0.864188f},
    { -0.295242f, 0.000000f, 0.955423f},
    { -0.309017f, 0.500000f, 0.809017f},
    { -0.162460f, 0.262866f, 0.951056f},
    { 0.000000f, 0.000000f, 1.000000f},
    { 0.000000f, 0.850651f, 0.525731f},
    { -0.147621f, 0.716567f, 0.681718f},
    { 0.147621f, 0.716567f, 0.681718f},
    { 0.000000f, 0.525731f, 0.850651f},
    { 0.309017f, 0.500000f, 0.809017f},
    { 0.525731f, 0.000000f, 0.850651f},
    { 0.295242f, 0.000000f, 0.955423f},
    { 0.442863f, 0.238856f, 0.864188f},
    { 0.162460f, 0.262866f, 0.951056f},
    { -0.681718f, 0.147621f, 0.716567f},
    { -0.809017f, 0.309017f, 0.500000f},
    { -0.587785f, 0.425325f, 0.688191f},
    { -0.850651f, 0.525731f, 0.000000f},
    { -0.864188f, 0.442863f, 0.238856f},
    { -0.716567f, 0.681718f, 0.147621f},
    { -0.688191f, 0.587785f, 0.425325f},
    { -0.500000f, 0.809017f, 0.309017f},
    { -0.238856f, 0.864188f, 0.442863f},
    { -0.425325f, 0.688191f, 0.587785f},
    { -0.716567f, 0.681718f, -0.147621f},
    { -0.500000f, 0.809017f, -0.309017f},
    { -0.525731f, 0.850651f, 0.000000f},
    { 0.000000f, 0.850651f, -0.525731f},
    { -0.238856f, 0.864188f, -0.442863f},
    { 0.000000f, 0.955423f, -0.295242f},
    { -0.262866f, 0.951056f, -0.162460f},
    { 0.000000f, 1.000000f, 0.000000f},
    { 0.000000f, 0.955423f, 0.295242f},
    { -0.262866f, 0.951056f, 0.162460f},
    { 0.238856f, 0.864188f, 0.442863f},
    { 0.262866f, 0.951056f, 0.162460f},
    { 0.500000f, 0.809017f, 0.309017f},
    { 0.238856f, 0.864188f, -0.442863f},
    { 0.262866f, 0.951056f, -0.162460f},
    { 0.500000f, 0.809017f, -0.309017f},
    { 0.850651f, 0.525731f, 0.000000f},
    { 0.716567f, 0.681718f, 0.147621f},
    { 0.716567f, 0.681718f, -0.147621f},
    { 0.525731f, 0.850651f, 0.000000f},
    { 0.425325f, 0.688191f, 0.587785f},
    { 0.864188f, 0.442863f, 0.238856f},
    { 0.688191f, 0.587785f, 0.425325f},
    { 0.809017f, 0.309017f, 0.500000f},
    { 0.681718f, 0.147621f, 0.716567f},
    { 0.587785f, 0.425325f, 0.688191f},
    { 0.955423f, 0.295242f, 0.000000f},
    { 1.000000f, 0.000000f, 0.000000f},
    { 0.951056f, 0.162460f, 0.262866f},
    { 0.850651f, -0.525731f, 0.000000f},
    { 0.955423f, -0.295242f, 0.000000f},
    { 0.864188f, -0.442863f, 0.238856f},
    { 0.951056f, -0.162460f, 0.262866f},
    { 0.809017f, -0.309017f, 0.500000f},
    { 0.681718f, -0.147621f, 0.716567f},
    { 0.850651f, 0.000000f, 0.525731f},
    { 0.864188f, 0.442863f, -0.238856f},
    { 0.809017f, 0.309017f, -0.500000f},
    { 0.951056f, 0.162460f, -0.262866f},
    { 0.525731f, 0.000000f, -0.850651f},
    { 0.681718f, 0.147621f, -0.716567f},
    { 0.681718f, -0.147621f, -0.716567f},
    { 0.850651f, 0.000000f, -0.525731f},
    { 0.809017f, -0.309017f, -0.500000f},
    { 0.864188f, -0.442863f, -0.238856f},
    { 0.951056f, -0.162460f, -0.262866f},
    { 0.147621f, 0.716567f, -0.681718f},
    { 0.309017f, 0.500000f, -0.809017f},
    { 0.425325f, 0.688191f, -0.587785f},
    { 0.442863f, 0.238856f, -0.864188f},
    { 0.587785f, 0.425325f, -0.688191f},
    { 0.688191f, 0.587785f, -0.425325f},
    { -0.147621f, 0.716567f, -0.681718f},
    { -0.309017f, 0.500000f, -0.809017f},
    { 0.000000f, 0.525731f, -0.850651f},
    { -0.525731f, 0.000000f, -0.850651f},
    { -0.442863f, 0.238856f, -0.864188f},
    { -0.295242f, 0.000000f, -0.955423f},
    { -0.162460f, 0.262866f, -0.951056f},
    { 0.000000f, 0.000000f, -1.000000f},
    { 0.295242f, 0.000000f, -0.955423f},
    { 0.162460f, 0.262866f, -0.951056f},
    { -0.442863f, -0.238856f, -0.864188f},
    { -0.309017f, -0.500000f, -0.809017f},
    { -0.162460f, -0.262866f, -0.951056f},
    { 0.000000f, -0.850651f, -0.525731f},
    { -0.147621f, -0.716567f, -0.681718f},
    { 0.147621f, -0.716567f, -0.681718f},
    { 0.000000f, -0.525731f, -0.850651f},
    { 0.309017f, -0.500000f, -0.809017f},
    { 0.442863f, -0.238856f, -0.864188f},
    { 0.162460f, -0.262866f, -0.951056f},
    { 0.238856f, -0.864188f, -0.442863f},
    { 0.500000f, -0.809017f, -0.309017f},
    { 0.425325f, -0.688191f, -0.587785f},
    { 0.716567f, -0.681718f, -0.147621f},
    { 0.688191f, -0.587785f, -0.425325f},
    { 0.587785f, -0.425325f, -0.688191f},
    { 0.000000f, -0.955423f, -0.295242f},
    { 0.000000f, -1.000000f, 0.000000f},
    { 0.262866f, -0.951056f, -0.162460f},
    { 0.000000f, -0.850651f, 0.525731f},
    { 0.000000f, -0.955423f, 0.295242f},
    { 0.238856f, -0.864188f, 0.442863f},
    { 0.262866f, -0.951056f, 0.162460f},
    { 0.500000f, -0.809017f, 0.309017f},
    { 0.716567f, -0.681718f, 0.147621f},
    { 0.525731f, -0.850651f, 0.000000f},
    { -0.238856f, -0.864188f, -0.442863f},
    { -0.500000f, -0.809017f, -0.309017f},
    { -0.262866f, -0.951056f, -0.162460f},
    { -0.850651f, -0.525731f, 0.000000f},
    { -0.716567f, -0.681718f, -0.147621f},
    { -0.716567f, -0.681718f, 0.147621f},
    { -0.525731f, -0.850651f, 0.000000f},
    { -0.500000f, -0.809017f, 0.309017f},
    { -0.238856f, -0.864188f, 0.442863f},
    { -0.262866f, -0.951056f, 0.162460f},
    { -0.864188f, -0.442863f, 0.238856f},
    { -0.809017f, -0.309017f, 0.500000f},
    { -0.688191f, -0.587785f, 0.425325f},
    { -0.681718f, -0.147621f, 0.716567f},
    { -0.442863f, -0.238856f, 0.864188f},
    { -0.587785f, -0.425325f, 0.688191f},
    { -0.309017f, -0.500000f, 0.809017f},
    { -0.147621f, -0.716567f, 0.681718f},
    { -0.425325f, -0.688191f, 0.587785f},
    { -0.162460f, -0.262866f, 0.951056f},
    { 0.442863f, -0.238856f, 0.864188f},
    { 0.162460f, -0.262866f, 0.951056f},
    { 0.309017f, -0.500000f, 0.809017f},
    { 0.147621f, -0.716567f, 0.681718f},
    { 0.000000f, -0.525731f, 0.850651f},
    { 0.425325f, -0.688191f, 0.587785f},
    { 0.587785f, -0.425325f, 0.688191f},
    { 0.688191f, -0.587785f, 0.425325f},
    { -0.955423f, 0.295242f, 0.000000f},
    { -0.951056f, 0.162460f, 0.262866f},
    { -1.000000f, 0.000000f, 0.000000f},
    { -0.850651f, 0.000000f, 0.525731f},
    { -0.955423f, -0.295242f, 0.000000f},
    { -0.951056f, -0.162460f, 0.262866f},
    { -0.864188f, 0.442863f, -0.238856f},
    { -0.951056f, 0.162460f, -0.262866f},
    { -0.809017f, 0.309017f, -0.500000f},
    { -0.864188f, -0.442863f, -0.238856f},
    { -0.951056f, -0.162460f, -0.262866f},
    { -0.809017f, -0.309017f, -0.500000f},
    { -0.681718f, 0.147621f, -0.716567f},
    { -0.681718f, -0.147621f, -0.716567f},
    { -0.850651f, 0.000000f, -0.525731f},
    { -0.688191f, 0.587785f, -0.425325f},
    { -0.587785f, 0.425325f, -0.688191f},
    { -0.425325f, 0.688191f, -0.587785f},
    { -0.425325f, -0.688191f, -0.587785f},
    { -0.587785f, -0.425325f, -0.688191f},
    { -0.688191f, -0.587785f, -0.425325f},
};
