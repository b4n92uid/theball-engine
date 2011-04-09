#ifndef ShaderFile
#define ShaderFile

#include "GLee.h"
#include <map>

#include "Mathematics.h"

namespace tbe
{

/// \brief Class d'utilisation des shader GLSL

class Shader
{
public:

    Shader();
    Shader(const Shader& copy);
    ~Shader();

    Shader & operator=(const Shader& copy);

    /// Chargement d'un FragmentShader depuis content
    void parseFragmentShader(std::string content);

    /// Chargement d'un VertexShader depuis content
    void parseVertexShader(std::string content);

    /// Chargement d'un FragmentShader depuis un fichier
    void loadFragmentShader(std::string filepath);

    /// Chargement d'un VertexShader depuis un fichier
    void loadVertexShader(std::string filepath);

    /// Chargement du program
    void loadProgram();

    /// Utilisation du program
    void use(bool use = true);

    /// Transmet une variable au shader
    void uniform(const char* name, int value);
    void uniform(const char* name, float value);
    void uniform(const char* name, Vector3f value);
    void uniform(const char* name, Vector3i value);
    void uniform(const char* name, Vector4f value);
    void uniform(const char* name, Vector4i value);
    void uniform(const char* name, Vector2f value);
    void uniform(const char* name, Vector2i value);

    operator bool()
    {
        return m_program;
    }

    bool operator!()
    {
        return m_program;
    }

    operator GLuint()
    {
        return m_program;
    }

    static bool checkHardware();

protected:
    GLuint m_program;
    GLuint m_frag_shader;
    GLuint m_vert_shader;
};

}

#endif
