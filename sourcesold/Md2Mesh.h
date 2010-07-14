#ifndef MD2MESH_H
#define MD2MESH_H

#include "node.h"

namespace tbe
{
namespace scene
{

/// \brief Loader de fichier Quake MD2 (.md2)

class MD2Mesh : public Node
{
public:

    /// Ctor avec construction depuis fichier
    MD2Mesh(const std::string& path);

    /// Ctor
    MD2Mesh();

    /// Dtor
    ~MD2Mesh();

    /// Construction depuis un fichier
    void Open(const std::string& path);

    /// Selection de la texture par default
    void SetDefaultTexture();

    /// Selectione l'animation entre frameStart et frameEnd
    void SetAnim(int frameStart, int frameEnd);

    /// Selection l'animation d'apres sont index
    void SetAnim(int index);

    /// Selection l'animation d'apres sont nom
    void SetAnim(std::string name);

    /// Operateur d'assignation
    MD2Mesh & operator=(MD2Mesh& target);

    /// Taille
    void SetSize(float value);

    /// Vittess d'animation
    void SetAnimSpeed(float value);

    /// Renvoi true si fin de l'animation
    bool isFrameEnd();

protected:

    /// Header d'un fichier MD2

    struct MD2Header
    {
        int m_id;
        int m_version;

        int m_skinwidth;
        int m_skinheight;

        int m_framesize;

        int m_num_skins;
        int m_num_vertex;
        int m_num_texcoord;
        int m_num_tris;
        int m_num_glcmds;
        int m_num_frames;

        int m_offset_skins;
        int m_offset_texcoord;
        int m_offset_tri;
        int m_offset_frames;
        int m_offset_glcmds;
        int m_offset_end;

    } m_header;

    /// Vertex d'un maillage MD2

    struct MD2Vertex
    {

        MD2Vertex & operator=(MD2Vertex & c)
        {
            for (int i = 0; i < 3; i++) m_vertex[i] = c.m_vertex[i];
            return *this;
        }

        unsigned char m_vertex[3];
        unsigned char normalIndex;

    };

    /// Triangle (face) d'un maillage MD2

    struct MD2Tri
    {

        MD2Tri & operator=(MD2Tri & c)
        {
            for (int i = 0; i < 3; i++)
            {
                m_vertex[i] = c.m_vertex[i];
                m_texcoord[i] = c.m_texcoord[i];
            }
            return *this;
        }

        unsigned short m_vertex[3];
        unsigned short m_texcoord[3];

    } *m_tris;

    /// Coordonne de texture d'un maillage MD2

    struct MD2TexCoord
    {
        short s;
        short t;

    } *m_texcoords;

    /// Frame d'un maillage MD2

    struct MD2Frame
    {

        MD2Frame & operator=(MD2Frame & c)
        {
            for (int i = 0; i < 3; i++)
            {
                m_size[i] = c.m_size[i];
                m_translate[i] = c.m_translate[i];
            }
            strcpy(m_name, c.m_name);
            return *this;
        }

        float m_size[3];
        float m_translate[3];
        char m_name[16];

        MD2Vertex *m_vertex;

    } *m_frames;

    /// Apparence d'un maillage MD2

    struct MD2Skin
    {

        MD2Skin & operator=(MD2Skin & c)
        {
            strcpy(m_name, c.m_name);
            return *this;
        }

        char m_name[64];

    } *m_skins;

    int m_framestart, m_framecur, m_framenext, m_frameend;
    float m_interp, m_interpspeed, m_size;
    int* m_glcmds;

    std::string m_curanimname, m_md2filepath;
    std::vector<std::string> m_anim;

    static float m_kAnorms[162][3];

    void ObjectRender();

    void RenderFrame(int frame);
    void RenderFrame(int frameA, int frameB, float interp);


    void Free();
};

}
}

#endif
