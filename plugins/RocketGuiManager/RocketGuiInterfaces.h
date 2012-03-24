/* 
 * File:   RocketGuiInterfaces.h
 * Author: b4n92uid
 *
 * Created on 24 mars 2012, 13:41
 */

#ifndef ROCKETGUIINTERFACES_H
#define	ROCKETGUIINTERFACES_H

#include <Tbe.h>
#include <Rocket/Core.h>
#include <Rocket/Controls.h>

namespace tbe
{

namespace gui
{

class ShellSystemInterface : public Rocket::Core::SystemInterface
{
public:

    /// Get the number of seconds elapsed since the start of the application
    /// @returns Seconds elapsed
    float GetElapsedTime();

    /// Log the specified message.
    /// @param[in] type Type of log message, ERROR, WARNING, etc.
    /// @param[in] message Message to log.
    /// @return True to continue execution, false to break into the debugger.
    bool LogMessage(Rocket::Core::Log::Type type, const Rocket::Core::String& message);

private:
    ticks::Clock m_clock;
};

class ShellRenderInterfaceOpenGL : public Rocket::Core::RenderInterface
{
public:

    ShellRenderInterfaceOpenGL(Vector2i viewport);

    /// Called by Rocket when it wants to render geometry that it does not wish to optimise.
    void RenderGeometry(Rocket::Core::Vertex* vertices, int num_vertices, int* indices, int num_indices, const Rocket::Core::TextureHandle texture, const Rocket::Core::Vector2f& translation);

    // Called by Rocket when it wants to enable or disable scissoring to clip content.		
    void EnableScissorRegion(bool enable);

    // Called by Rocket when it wants to change the scissor region.		
    void SetScissorRegion(int x, int y, int width, int height);

    /// Called by Rocket when a texture is required by the library.
    bool LoadTexture(Rocket::Core::TextureHandle& texture_handle, Rocket::Core::Vector2i& texture_dimensions, const Rocket::Core::String& source);

    /// Called by Rocket when a texture is required to be built from an internally-generated sequence of pixels.
    bool GenerateTexture(Rocket::Core::TextureHandle& texture_handle, const Rocket::Core::byte* source, const Rocket::Core::Vector2i& source_dimensions);

    /// Called by Rocket when a loaded texture is no longer required.
    void ReleaseTexture(Rocket::Core::TextureHandle texture_handle);

    void SetViewport(Vector2i viewport);
    Vector2i GetViewport() const;

private:
    Vector2i m_viewport;
};

class ShellFileInterface : public Rocket::Core::FileInterface
{
public:
    /// Opens a file.		
    Rocket::Core::FileHandle Open(const Rocket::Core::String& path);

    /// Closes a previously opened file.		
    void Close(Rocket::Core::FileHandle file);

    // Reads data from a previously opened file.
    size_t Read(void* buffer, size_t size, Rocket::Core::FileHandle file);

    // Seeks to a point in a previously opened file.
    bool Seek(Rocket::Core::FileHandle file, long offset, int origin);

    // Returns the current position of the file pointer.
    size_t Tell(Rocket::Core::FileHandle file);
};

}
}

#endif	/* ROCKETGUIINTERFACES_H */
