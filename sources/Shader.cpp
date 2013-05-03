#include "Shader.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/optional.hpp>
#include <boost/optional.hpp>
#include <boost/foreach.hpp>

#include "Exception.h"

using namespace std;
using namespace tbe;

class SharedShaderManager : public vector<GLuint>
{
public:

    ~SharedShaderManager()
    {
        for(iterator it = begin(); it != end(); ++it)
            glDeleteProgram(*it);
    }

} manager;

Shader::Shader()
{
    m_vert_shader = 0;
    m_frag_shader = 0;
    m_program = 0;
    m_enable = false;
}

Shader::Shader(const Shader& copy)
{
    m_vert_shader = copy.m_vert_shader;
    m_frag_shader = copy.m_frag_shader;
    m_program = copy.m_program;
    m_vertFilename = copy.m_vertFilename;
    m_fragFilename = copy.m_fragFilename;
    m_shaderFilename = copy.m_shaderFilename;
    m_enable = copy.m_enable;
    m_requestedUniform = copy.m_requestedUniform;
}

Shader::~Shader() { }

Shader& Shader::operator=(const Shader& copy)
{
    m_vert_shader = copy.m_vert_shader;
    m_frag_shader = copy.m_frag_shader;
    m_program = copy.m_program;
    m_vertFilename = copy.m_vertFilename;
    m_fragFilename = copy.m_fragFilename;
    m_shaderFilename = copy.m_shaderFilename;
    m_enable = copy.m_enable;
    m_requestedUniform = copy.m_requestedUniform;

    return *this;
}

GLuint ParseShader(const string& content, GLenum type)
{
    // Création du shader
    GLuint shader = glCreateShader(type);

    // Compilation du shader
    const char* source = content.c_str();
    glShaderSource(shader, 1, &source, 0);
    glCompileShader(shader);

    // Verification d'erreur a la compildation
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        Exception ex;

        int logsize;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logsize);

        char* c_log = new char[logsize + 1];
        glGetShaderInfoLog(shader, logsize, &logsize, c_log);

        switch(type)
        {
            case GL_VERTEX_SHADER: ex << "Shader::LoadShader; Vertex Shader compile error" << endl << c_log;
            case GL_FRAGMENT_SHADER: ex << "Shader::LoadShader; Fragment Shader compile error" << endl << c_log;
        }

        cout << ex.what() << endl;
        throw ex;
    }

    return shader;
}

void Shader::parseVertexShader(std::string content)
{
    m_vert_shader = ParseShader(content.c_str(), GL_VERTEX_SHADER);
}

void Shader::parseFragmentShader(std::string content)
{
    m_frag_shader = ParseShader(content.c_str(), GL_FRAGMENT_SHADER);
}

void Shader::loadVertexShader(std::string filepath)
{
    cout << "[Vertex shader] " << filepath << endl;

    ifstream file(filepath.c_str());

    if(!file)
        throw tbe::Exception("Shader::LoadVertexShader; Open shader file error; (%s)", filepath.c_str());

    stringstream sourceCode;
    sourceCode << file.rdbuf();

    file.close();

    try
    {
        m_vert_shader = ParseShader(sourceCode.str().c_str(), GL_VERTEX_SHADER);
    }
    catch(Exception& e)
    {
        throw Exception("[Vertex Shader] " + filepath + "\n" + e.what());
    }

    m_vertFilename = filepath;
}

void Shader::loadFragmentShader(std::string filepath)
{
    cout << "[Fragment shader] " << filepath << endl;

    ifstream file(filepath.c_str());

    if(!file)
        throw tbe::Exception("Shader::LoadFragmentShader; Open shader file error; (%s)", filepath.c_str());

    stringstream sourceCode;
    sourceCode << file.rdbuf();

    file.close();

    try
    {
        m_frag_shader = ParseShader(sourceCode.str().c_str(), GL_FRAGMENT_SHADER);
    }
    catch(Exception& e)
    {
        throw Exception("[Fragment Shader] " + filepath + "\n" + e.what());
    }

    m_fragFilename = filepath;
}

void Shader::use(bool use)
{
    if(m_enable)
        glUseProgram(use ? m_program : 0);
}

void Shader::loadProgram()
{
    cout << "[Setup shader program]" << endl;

    // Creation du program
    m_program = glCreateProgram();

    // Attache les shader aux programme
    if(m_vert_shader)
    {
        glAttachShader(m_program, m_vert_shader);
        glDeleteShader(m_vert_shader);
        m_vert_shader = 0;
    }

    if(m_frag_shader)
    {
        glAttachShader(m_program, m_frag_shader);
        glDeleteShader(m_frag_shader);
        m_frag_shader = 0;
    }

    // linkage
    glLinkProgram(m_program);

    // Verification d'erreur au linkage
    int success;
    glGetProgramiv(m_program, GL_LINK_STATUS, &success);

    if(!success)
    {
        tbe::Exception ex;

        ex << "Shader::LoadProgram; Program link error;" << endl;

        int logsize = 0;
        char* log = NULL;

        glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &logsize);
        log = new char[logsize + 1];

        std::fill(log, log + logsize + 1, 0);

        glGetProgramInfoLog(m_program, logsize, &logsize, log);
        ex << log;

        delete[] log;

        throw ex;
    }

    m_enable = true;

    manager.push_back(m_program);
}

void Shader::parseShaderFile(std::string path)
{
    using namespace boost;
    using namespace property_tree;

    cout << "[Shader Program] " << path << endl;

    ptree data;
    property_tree::read_info(path, data);

    if(!data.count("vertex"))
        throw new Exception("Shader::parseShaderFile; Parsing error (%s)", path.c_str());

    m_shaderFilename = path;

    if(data.get_optional<string>("vertex"))
    {
        string vertex = data.get<string>("vertex");

        if(!tools::isAbsoloutPath(vertex))
            vertex = tools::resolvePath(vertex, path);

        loadVertexShader(vertex);
    }

    if(data.get_optional<string>("fragment"))
    {
        string frag = data.get<string>("fragment");

        if(!tools::isAbsoloutPath(frag))
            frag = tools::resolvePath(frag, path);

        loadFragmentShader(frag);
    }

    loadProgram();

    if(data.count("bind"))
        BOOST_FOREACH(ptree::value_type & b, data.get_child("bind"))
    {
        setRequestedUniform(b.first, b.second.data());
    }

}

std::string Shader::getShaderFile() const
{
    return m_shaderFilename;
}

std::string Shader::getVertFilename() const
{
    return m_vertFilename;
}

std::string Shader::getFragFilename() const
{
    return m_fragFilename;
}

rtree Shader::serialize(std::string root)
{
    rtree scheme;

    scheme.put("vertex", tools::relativizePath(m_vertFilename, root));
    scheme.put("fragment", tools::relativizePath(m_fragFilename, root));

    rtree bindtree;

    BOOST_FOREACH(Shader::UniformMap::value_type v, m_requestedUniform)
    {
        bindtree.put(v.first, v.second);
    }

    scheme.put_child("bind", bindtree);

    return scheme;
}

void Shader::uniform(std::string name, bool value)
{
    GLint id = glGetUniformLocation(m_program, name.c_str());

    if(id == -1)
        cout << "Shader::SetUniform; Invalid variable 1b id (" << name << ")" << endl;
    else
        glUniform1i(id, value);
}

void Shader::uniform(std::string name, float value)
{
    GLint id = glGetUniformLocation(m_program, name.c_str());

    if(id == -1)
        cout << "Shader::SetUniform; Invalid variable 1f id (" << name << ")" << endl;
    else
        glUniform1f(id, value);
}

void Shader::uniform(std::string name, int value)
{
    GLint id = glGetUniformLocation(m_program, name.c_str());

    if(id == -1)
        cout << "Shader::SetUniform; Invalid variable 1i id (" << name << ")" << endl;
    else
        glUniform1i(id, value);
}

void Shader::uniform(std::string name, Vector4f value)
{
    GLint id = glGetUniformLocation(m_program, name.c_str());

    if(id == -1)
        cout << "Shader::SetUniform; Invalid variable 4f id (" << name << ")" << endl;
    else
        glUniform4f(id, value.x, value.y, value.z, value.w);
}

void Shader::uniform(std::string name, Vector4i value)
{
    GLint id = glGetUniformLocation(m_program, name.c_str());

    if(id == -1)
        cout << "Shader::SetUniform; Invalid variable 4i id (" << name << ")" << endl;
    else
        glUniform4i(id, value.x, value.y, value.z, value.w);
}

void Shader::uniform(std::string name, Vector3f value)
{
    GLint id = glGetUniformLocation(m_program, name.c_str());

    if(id == -1)
        cout << "Shader::SetUniform; Invalid variable 3f id (" << name << ")" << endl;
    else
        glUniform3f(id, value.x, value.y, value.z);
}

void Shader::uniform(std::string name, Vector3i value)
{
    GLint id = glGetUniformLocation(m_program, name.c_str());

    if(id == -1)
        cout << "Shader::SetUniform; Invalid variable 3i id (" << name << ")" << endl;
    else
        glUniform3i(id, value.x, value.y, value.z);
}

void Shader::uniform(std::string name, Vector2f value)
{
    GLint id = glGetUniformLocation(m_program, name.c_str());

    if(id == -1)
        cout << "Shader::SetUniform; Invalid variable 2f id (" << name << ")" << endl;
    else
        glUniform2f(id, value.x, value.y);
}

void Shader::uniform(std::string name, Vector2i value)
{
    GLint id = glGetUniformLocation(m_program, name.c_str());

    if(id == -1)
        cout << "Shader::SetUniform; Invalid variable 2i id (" << name << ")" << endl;
    else
        glUniform2i(id, value.x, value.y);
}

void Shader::uniform(std::string name, Matrix4 value)
{
    GLint id = glGetUniformLocation(m_program, name.c_str());

    if(id == -1)
        cout << "Shader::SetUniform; Invalid variable m4 id (" << name << ")" << endl;
    else
        glUniformMatrix4fv(id, 1, false, (float*) value);
}

bool Shader::m_hardwareSupport = false;
bool Shader::m_hardwareCheked = false;

bool Shader::checkHardware()
{
    if(!m_hardwareCheked)
    {
        m_hardwareSupport = GLEE_ARB_shading_language_100 && GLEE_ARB_shader_objects && GLEE_ARB_vertex_shader && GLEE_ARB_fragment_shader;
        m_hardwareCheked = true;
    }

    return m_hardwareSupport;
}

void Shader::forceHardware(bool enable)
{
    m_hardwareSupport = enable;
}

void Shader::setRequestedUniform(std::string what, std::string var)
{
    this->m_requestedUniform[what] = var;
}

const Shader::UniformMap& Shader::getRequestedUniform()
{
    return m_requestedUniform;
}

void Shader::setEnable(bool enable)
{
    this->m_enable = enable;
}

bool Shader::isEnable() const
{
    return m_enable;
}

void Shader::bind(Shader& shader)
{
    if(shader.m_enable)
        glUseProgram(shader.m_program);
}

void Shader::unbind()
{
    glUseProgram(0);
}
