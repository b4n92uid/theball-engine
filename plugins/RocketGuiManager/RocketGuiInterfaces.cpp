/* 
 * File:   RocketGuiInterfaces.cpp
 * Author: b4n92uid
 * 
 * Created on 24 mars 2012, 13:41
 */

#include "RocketGuiInterfaces.h"

using namespace tbe;
using namespace tbe::gui;

float ShellSystemInterface::GetElapsedTime()
{
    return m_clock.getEsplanedTime(false);
}

bool ShellSystemInterface::LogMessage(Rocket::Core::Log::Type type, const Rocket::Core::String& message)
{
    std::cout << message.CString() << std::endl;
    return true;
}

ShellRenderInterfaceOpenGL::ShellRenderInterfaceOpenGL(Vector2i viewport)
{
    m_viewport = viewport;
}

void ShellRenderInterfaceOpenGL::RenderGeometry(Rocket::Core::Vertex* vertices, int num_vertices, int* indices, int num_indices, const Rocket::Core::TextureHandle texture, const Rocket::Core::Vector2f& translation)
{
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPushMatrix();
    glTranslatef(translation.x, translation.y, 0);

    glVertexPointer(2, GL_FLOAT, sizeof(Rocket::Core::Vertex), &vertices[0].position);
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Rocket::Core::Vertex), &vertices[0].colour);

    if(!texture)
    {
        glDisable(GL_TEXTURE_2D);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
    else
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, (GLuint)texture);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, sizeof(Rocket::Core::Vertex), &vertices[0].tex_coord);
    }

    glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, indices);

    glPopMatrix();
}

void ShellRenderInterfaceOpenGL::EnableScissorRegion(bool enable)
{
    if(enable)
        glEnable(GL_SCISSOR_TEST);
    else
        glDisable(GL_SCISSOR_TEST);
}

void ShellRenderInterfaceOpenGL::SetScissorRegion(int x, int y, int width, int height)
{
    glScissor(x, m_viewport.y - (y + height), width, height);
}

bool ShellRenderInterfaceOpenGL::LoadTexture(Rocket::Core::TextureHandle& texture_handle, Rocket::Core::Vector2i& texture_dimensions, const Rocket::Core::String& source)
{
    try
    {
        Texture tex;

        tex.load(source.CString());
        tex.setPersistent(true);

        texture_handle = tex.getTextureName();

        texture_dimensions.x = tex.getSize().x;
        texture_dimensions.y = tex.getSize().y;

        return true;
    }
    catch(std::exception& e)
    {
        Rocket::Core::Log::Message(Rocket::Core::Log::LT_WARNING, e.what());
        return false;
    }
}

bool ShellRenderInterfaceOpenGL::GenerateTexture(Rocket::Core::TextureHandle& texture_handle, const Rocket::Core::byte* source, const Rocket::Core::Vector2i& source_dimensions)
{
    GLuint texture_id = 0;
    glGenTextures(1, &texture_id);
    if(texture_id == 0)
    {
        printf("Failed to generate textures\n");
        return false;
    }

    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, source_dimensions.x, source_dimensions.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, source);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    texture_handle = (Rocket::Core::TextureHandle) texture_id;

    return true;
}

void ShellRenderInterfaceOpenGL::ReleaseTexture(Rocket::Core::TextureHandle texture_handle)
{
    GLuint id = (GLuint)texture_handle;

    glDeleteTextures(1, &id);
}

void ShellRenderInterfaceOpenGL::SetViewport(Vector2i viewport)
{
    this->m_viewport = viewport;
}

Vector2i ShellRenderInterfaceOpenGL::GetViewport() const
{
    return m_viewport;
}

Rocket::Core::FileHandle ShellFileInterface::Open(const Rocket::Core::String& path)
{
    // Attempt to open the file relative to the current working directory.
    FILE* fp = fp = fopen(path.CString(), "rb");
    return(Rocket::Core::FileHandle) fp;
}

void ShellFileInterface::Close(Rocket::Core::FileHandle file)
{
    fclose((FILE*)file);
}

size_t ShellFileInterface::Read(void* buffer, size_t size, Rocket::Core::FileHandle file)
{
    return fread(buffer, 1, size, (FILE*)file);
}

bool ShellFileInterface::Seek(Rocket::Core::FileHandle file, long offset, int origin)
{
    return fseek((FILE*)file, offset, origin) == 0;
}

size_t ShellFileInterface::Tell(Rocket::Core::FileHandle file)
{
    return ftell((FILE*)file);
}
