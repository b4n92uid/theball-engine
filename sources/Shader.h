#ifndef ShaderFile
#define ShaderFile

#include <windows.h>
#include "GLee.h"

#include <map>
#include <boost/optional.hpp>

#include "Clock.h"
#include "Mathematics.h"
#include "Tools.h"

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

    /// Chargement d'un fragment shader (pixel shader) depuis content
    void parseFragmentShader(std::string content);

    /// Chargement d'un vertex shader depuis content
    void parseVertexShader(std::string content);

    /// Chargement d'un fragment shader (pixel shader) depuis un fichier
    void loadFragmentShader(std::string filepath);

    /// Chargement d'un vertex shader depuis un fichier
    void loadVertexShader(std::string filepath);

    /**
     * Chargement du program. Cette fonction deveras etre appeller au moin
     * apres le chargement d'un vertex shader et éventuellement aprés le
     * chargement d'un fragment shader
     */
    void loadProgram();

    /**
     * Charge un program a partir d'un fichier au format 
     * property_tree info
     * @param path
     */
    void parseShaderFile(std::string path);

    std::string getVertFilename() const;

    std::string getFragFilename() const;

    std::string getShaderFile() const;

    rtree serialize(std::string root);

    void setEnable(bool enable);

    bool isEnable() const;

    typedef std::map<std::string, std::string> UniformMap;

    void setRequestedUniform(std::string what, std::string var);

    const UniformMap& getRequestedUniform();

    bool isRequested(std::string value);

    /**
     * Utilisation du program. Le rendue effectuer apres cette appelle
     * sera traiter par le shader
     *
     * @param use Etat d'utilisation du program shader
     */
    void use(bool use = true);

    /// Transmet une variable au shader de type bool
    void uniform(std::string name, bool value);

    /// Transmet une variable au shader de type int
    void uniform(std::string name, int value);

    /// Transmet une variable au shader de type float
    void uniform(std::string name, float value);

    /// Transmet une variable au shader de type Vector3f
    void uniform(std::string name, Vector3f value);

    /// Transmet une variable au shader de type Vector3i
    void uniform(std::string name, Vector3i value);

    /// Transmet une variable au shader de type Vector4f
    void uniform(std::string name, Vector4f value);

    /// Transmet une variable au shader de type Vector4i
    void uniform(std::string name, Vector4i value);

    /// Transmet une variable au shader de type Vector2f
    void uniform(std::string name, Vector2f value);

    /// Transmet une variable au shader de type Vector2i
    void uniform(std::string name, Vector2i value);

    /// Transmet une variable au shader de type Matrix4
    void uniform(std::string name, Matrix4 value);

    /// Récuperation d'identifiant OpenGL

    operator GLuint()
    {
        return m_program;
    }

    /**
     * Cette fonction vérifie si le materiel actuelle supporte l'utilisation
     * des shader.
     *
     * Pour déterminer le support des shader,
     * les Extention suivante sont tester :
     *  GL_ARB_shading_language_100
     *  GL_ARB_shader_objects
     *  GL_ARB_vertex_shader
     *  GL_ARB_fragment_shader
     */
    static bool checkHardware();
    static void forceHardware(bool enable);

    static void bind(Shader& shader);
    static void unbind();

protected:
    GLuint m_program;
    GLuint m_frag_shader;
    GLuint m_vert_shader;
    bool m_enable;

    std::string m_fragFilename;
    std::string m_vertFilename;
    std::string m_shaderFilename;

    UniformMap m_requestedUniform;

private:
    static bool m_hardwareSupport;
    static bool m_hardwareCheked;
};

}

#endif
