#include <iostream>
#include <sstream>
#include <fstream>

#include "Shader.h"
#include "Exception.h"

using namespace std;
using namespace tbe;

class SharedShaderManager : public map<Shader*, string>
{
public:

    Shader* IsExist(string path)
    {
        for(iterator itt = begin(); itt != end(); itt++)
            if(itt->second == path)
                return itt->first;

        return NULL;
    }
};

Shader::Shader()
{
    m_vert_shader = 0;
    m_frag_shader = 0;
    m_program = 0;
}

GLuint ParseShader(const char* content, GLenum type)
{
    // Création du shader
    GLuint shader = glCreateShader(type);

    // Compilation du shader
    glShaderSource(shader, 1, &content, 0);
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

        throw ex;
    }

    return shader;
}

GLuint LoadShader(const char* path, GLenum type)
{
    // Lecteure du code source
    ifstream file(path);

    if(!file)
        throw tbe::Exception("Shader::LoadShader; Open shader file error; (%s)", path);

    stringstream sourceCode;
    sourceCode << file.rdbuf();

    file.close();

    return ParseShader(sourceCode.str().c_str(), type);
}

void Shader::ParseVertexShader(std::string content)
{
    m_vert_shader = ParseShader(content.c_str(), GL_VERTEX_SHADER);
}

void Shader::ParseFragmentShader(std::string content)
{
    m_frag_shader = ParseShader(content.c_str(), GL_FRAGMENT_SHADER);
}

void Shader::LoadVertexShader(std::string filepath)
{
    cout << "Load vertex shader file : " << filepath << endl;
    m_vert_shader = LoadShader(filepath.c_str(), GL_VERTEX_SHADER);
}

void Shader::LoadFragmentShader(std::string filepath)
{
    cout << "Load fragment shader file : " << filepath << endl;
    m_frag_shader = LoadShader(filepath.c_str(), GL_FRAGMENT_SHADER);
}

void Shader::Use(bool use)
{
    glUseProgram(use ? m_program : 0);
}

void Shader::LoadProgram()
{
    cout << "Load shader program" << endl;

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
}

void Shader::SetUniform(const char* name, float value)
{
    GLint id = glGetUniformLocation(m_program, name);

    if(id == -1)
        throw tbe::Exception("Shader::SetUniform : 1f Invalid variable id (%s)", name);

    glUniform1f(id, value);
}

void Shader::SetUniform(const char* name, int value)
{
    GLint id = glGetUniformLocation(m_program, name);

    if(id == -1)
        throw tbe::Exception("Shader::SetUniform; Invalid variable 1I id (%s)", name);

    glUniform1i(id, value);
}

void Shader::SetUniform(const char* name, Vector4f value)
{
    GLint id = glGetUniformLocation(m_program, name);

    if(id == -1)
        throw tbe::Exception("Shader::SetUniform : 4f, Invalid variable id (%s)", name);

    glUniform4f(id, value.x, value.y, value.z, value.w);
}

void Shader::SetUniform(const char* name, Vector4i value)
{
    GLint id = glGetUniformLocation(m_program, name);

    if(id == -1)
        throw tbe::Exception("Shader::SetUniform : 4i, Invalid variable id (%s)", name);

    glUniform4i(id, value.x, value.y, value.z, value.w);
}

void Shader::SetUniform(const char* name, Vector3f value)
{
    GLint id = glGetUniformLocation(m_program, name);

    if(id == -1)
        throw tbe::Exception("Shader::SetUniform : 3f, Invalid variable id (%s)", name);

    glUniform3f(id, value.x, value.y, value.z);
}

void Shader::SetUniform(const char* name, Vector3i value)
{
    GLint id = glGetUniformLocation(m_program, name);

    if(id == -1)
        throw tbe::Exception("Shader::SetUniform : 3i, Invalid variable id (%s)", name);

    glUniform3i(id, value.x, value.y, value.z);
}

void Shader::SetUniform(const char* name, Vector2f value)
{
    GLint id = glGetUniformLocation(m_program, name);

    if(id == -1)
        throw tbe::Exception("Shader::SetUniform : 2f, Invalid variable id (%s)", name);

    glUniform2f(id, value.x, value.y);
}

void Shader::SetUniform(const char* name, Vector2i value)
{
    GLint id = glGetUniformLocation(m_program, name);

    if(id == -1)
        throw tbe::Exception("Shader::SetUniform : 2i, Invalid variable id (%s)", name);

    glUniform2i(id, value.x, value.y);
}

bool Shader::CheckHardware()
{
    static bool supported = GLEE_ARB_shading_language_100 && GLEE_ARB_shader_objects
            && GLEE_ARB_vertex_shader && GLEE_ARB_fragment_shader
            && GLEE_VERSION_2_0;

    return supported;
}
