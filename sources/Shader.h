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

    Shader& operator=(const Shader& copy);

    /// Chargement d'un FragmentShader depuis content
    void ParseFragmentShader(std::string content);

    /// Chargement d'un VertexShader depuis content
    void ParseVertexShader(std::string content);

    /// Chargement d'un FragmentShader depuis un fichier
    void LoadFragmentShader(std::string filepath);

    /// Chargement d'un VertexShader depuis un fichier
    void LoadVertexShader(std::string filepath);

    /// Chargement du program
    void LoadProgram();

    /// Utilisation du program
    void Use(bool use = true);

    /// Transmet une variable au shader
    void SetUniform(const char* name, int value);
    void SetUniform(const char* name, float value);
    void SetUniform(const char* name, Vector3f value);
    void SetUniform(const char* name, Vector3i value);
    void SetUniform(const char* name, Vector4f value);
    void SetUniform(const char* name, Vector4i value);
    void SetUniform(const char* name, Vector2f value);
    void SetUniform(const char* name, Vector2i value);

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

    static bool CheckHardware();

protected:
    GLuint m_program;
    GLuint m_frag_shader;
    GLuint m_vert_shader;
    std::string m_vShaderDump;
    std::string m_fShaderDump;
};

}

#endif
