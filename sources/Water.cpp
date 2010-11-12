/* 
 * File:   Water.cpp
 * Author: b4n92uid
 * 
 * Created on 21 mai 2010, 22:02
 */

#include "Water.h"

#include "SceneManager.h"
#include "HardwareBuffer.h"
#include "Rtt.h"
#include "Clock.h"

#include <time.h>

const char* vertexShader =
        "varying vec3 light;"
        "varying vec3 eye;"

        "varying vec4 projectionCoordinates;"

        "uniform vec2 uvRepeat;"

        "void main()"
        "{"
        "vec3 t = normalize(gl_NormalMatrix * vec3(1, 0, 0));"
        "vec3 n = normalize(gl_NormalMatrix * vec3(0, 1, 0));"
        "vec3 b = cross(n, t);"

        "vec3 vVertex = vec3(gl_ModelViewMatrix * gl_Vertex);"

        "vec3 tmpVec;"

        "if(gl_LightSource[0].position.w == 1.0)"
        "tmpVec = gl_LightSource[0].position.xyz - vVertex;"

        "else "
        "tmpVec = gl_LightSource[0].position.xyz;"

        "light.x = dot(tmpVec, t);"
        "light.y = dot(tmpVec, b);"
        "light.z = dot(tmpVec, n);"

        "tmpVec = -vVertex;"
        "eye.x = dot(tmpVec, t);"
        "eye.y = dot(tmpVec, b);"
        "eye.z = dot(tmpVec, n);"

        "projectionCoordinates = gl_ModelViewProjectionMatrix * gl_Vertex;"

        "gl_TexCoord[0].xy   = gl_MultiTexCoord0.xy * uvRepeat;"
        "gl_Position         = ftransform();"
        "}";

const char* fragmentShader =
        "varying vec3 light;"
        "varying vec3 eye;"

        "varying vec4 projectionCoordinates;"

        "uniform sampler2D colorMap;"
        "uniform sampler2D normalMap;"
        "uniform sampler2D reflexionMap;"
        "uniform sampler2D refractionMap;"

        "uniform float timer;"
        "uniform float blend;"
        "uniform float deform;"
        "uniform float speed;"

        "void main()"
        "{"
        #if 0
        "vec4 ambient = gl_LightSource[0].ambient * gl_FrontMaterial.ambient + gl_LightModel.ambient * gl_FrontMaterial.ambient;"
        "vec4 emission = gl_FrontMaterial.emission;"
        "vec4 diffuse;"
        "vec4 specular;"

        "float att = 1.0;"

        "if(gl_LightSource[0].position.w == 1.0)"
        "{"
        "float distance = length(light);"

        "att = 1.0 / (gl_LightSource[0].constantAttenuation"
        "+ gl_LightSource[0].linearAttenuation"
        "* distance"
        "+ gl_LightSource[0].quadraticAttenuation"
        "* distance"
        "* distance);"
        "}"

        "vec3 lightDiri = normalize(light);"
        #endif

        //On passe les normals dans un optique de [-1;1] plutot que [0;1]
        "vec3 n1 = texture2D(normalMap, gl_TexCoord[0].st - vec2(timer)*speed).xyz * 2.0 - 1.0;"
        "vec3 n2 = texture2D(normalMap, gl_TexCoord[0].st + vec2(timer)*speed).xyz * 2.0 - 1.0;"
        "vec3 bumpNormal = normalize(n1 + n2);"

        #if 0
        "float NdotL = max(0.0, dot(bumpNormal, lightDiri));"

        "if(NdotL > 0.0)"
        "{"
        "float NdotHV = dot(bumpNormal, normalize(lightDiri + eye));"

        "diffuse  = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse * NdotL * att;"
        "specular = gl_FrontMaterial.specular * gl_LightSource[0].specular * pow(NdotHV, gl_FrontMaterial.shininess) * att;"
        "}"
        #endif

        // Coorodoné de texture de projection
        "vec2 projCoord = projectionCoordinates.xy / projectionCoordinates.w;"
        "projCoord = (projCoord + 1.0) * 0.5;"
        //"projCoord = clamp(projCoord, 0.001, 0.999);"
        "projCoord = clamp(projCoord, 0.0, 1.0);"
        "projCoord += bumpNormal * deform;"

        // Calcule de Reflection & Refraction
        "vec4 reflexion = texture2D(reflexionMap, projCoord);"
        "vec4 refraction = texture2D(refractionMap, projCoord);"

        #if 0
        "gl_FragColor = ambient + emission + diffuse;"
        "gl_FragColor *= mix(reflexion, refraction, blend);"
        "gl_FragColor += specular;"
        #endif
        "gl_FragColor = mix(reflexion, refraction, blend);"
        "}";

using namespace tbe;
using namespace tbe::scene;

Water::Water()
{
    m_shader.ParseVertexShader(vertexShader);
    m_shader.ParseFragmentShader(fragmentShader);
    m_shader.LoadProgram();

    m_shader.Use(true);
    m_shader.SetUniform("normalMap", 0);
    m_shader.SetUniform("reflexionMap", 1);
    m_shader.SetUniform("refractionMap", 2);
    m_shader.Use(false);

    m_reflection.SetFrameSize(128);
    m_reflection.SetCaptureColor(true);
    m_reflection.SetCaptureDepth(true);

    m_refraction.SetFrameSize(128);
    m_refraction.SetCaptureColor(true);
    m_refraction.SetCaptureDepth(true);

    Vertex vertex[6] = {
        Vertex(Vector3f(1, 0, -1), Vector3f(0, 1, 0), 1, Vector2f(0, 1)),
        Vertex(Vector3f(1, 0, 1), Vector3f(0, 1, 0), 1, Vector2f(0, 0)),
        Vertex(Vector3f(-1, 0, 1), Vector3f(0, 1, 0), 1, Vector2f(1, 0)),
        Vertex(Vector3f(-1, 0, 1), Vector3f(0, 1, 0), 1, Vector2f(1, 0)),
        Vertex(Vector3f(-1, 0, -1), Vector3f(0, 1, 0), 1, Vector2f(1, 1)),
        Vertex(Vector3f(1, 0, -1), Vector3f(0, 1, 0), 1, Vector2f(0, 1)),
    };

    m_buffer.AddVertex(vertex, 6);

    m_buffer.Compile();
}

Water::~Water()
{
}

Water::Water(const Water& copy)
{
    m_shader.ParseVertexShader(vertexShader);
    m_shader.ParseFragmentShader(fragmentShader);
    m_shader.LoadProgram();

    m_shader.Use(true);
    m_shader.SetUniform("normalMap", 0);
    m_shader.SetUniform("reflexionMap", 1);
    m_shader.SetUniform("refractionMap", 2);
    m_shader.Use(false);

    m_reflection.SetFrameSize(128);
    m_reflection.SetCaptureColor(true);
    m_reflection.SetCaptureDepth(true);

    m_refraction.SetFrameSize(128);
    m_refraction.SetCaptureColor(true);
    m_refraction.SetCaptureDepth(true);

    Vertex vertex[6] = {
        Vertex(Vector3f(1, 0, -1), Vector3f(0, 1, 0), 1, Vector2f(0, 1)),
        Vertex(Vector3f(1, 0, 1), Vector3f(0, 1, 0), 1, Vector2f(0, 0)),
        Vertex(Vector3f(-1, 0, 1), Vector3f(0, 1, 0), 1, Vector2f(1, 0)),
        Vertex(Vector3f(-1, 0, 1), Vector3f(0, 1, 0), 1, Vector2f(1, 0)),
        Vertex(Vector3f(-1, 0, -1), Vector3f(0, 1, 0), 1, Vector2f(1, 1)),
        Vertex(Vector3f(1, 0, -1), Vector3f(0, 1, 0), 1, Vector2f(0, 1)),
    };

    m_buffer.AddVertex(vertex, 6);

    m_buffer.Compile();

    *this = copy;
}

Water& Water::operator=(const Water& copy)
{
    Node::operator=(copy);

    m_size = copy.m_size;
    m_normalMap = copy.m_normalMap;

    m_uvRepeat = copy.m_uvRepeat;
    m_blend = copy.m_blend;
    m_deform = copy.m_deform;
    m_speed = copy.m_speed;

    return *this;
}

void Water::BeginReflection()
{
    m_reflection.Use(true);
    m_reflection.Clear();

    glPushMatrix();
    glLoadIdentity();

    Camera* cam = m_parallelScene->GetSceneManager()->GetCurCamera();
    cam->Engine();

    glEnable(GL_CLIP_PLANE0);

    float height = m_matrix.GetPos().y;

    if(cam->GetPos().y > height)
    {
        glClipPlane(GL_CLIP_PLANE0, Vector4<double>(0, -1, 0, 0));
    }

    else
    {
        glEnable(GL_CLIP_PLANE0);
        glClipPlane(GL_CLIP_PLANE0, Vector4<double>(0, 1, 0, 0));
    }


    cam->Push();
    cam->SetPos(cam->GetPos() * Vector3f(1, -1, 1));

    glScalef(1, -1, 1);
}

void Water::EndReflection()
{
    glDisable(GL_CLIP_PLANE0);

    Camera* cam = m_parallelScene->GetSceneManager()->GetCurCamera();
    cam->Pop();

    glPopMatrix();

    m_reflection.Use(false);
}

void Water::BeginRefraction()
{
    m_refraction.Use(true);
    m_refraction.Clear();

    glPushMatrix();
    glLoadIdentity();

    Camera* cam = m_parallelScene->GetSceneManager()->GetCurCamera();
    cam->Engine();

    float height = m_matrix.GetPos().y;

    if(cam->GetPos().y > height)
    {
        glEnable(GL_CLIP_PLANE0);
        glClipPlane(GL_CLIP_PLANE0, Vector4<double>(0, -1, 0, 0));
    }
}

void Water::EndRefraction()
{
    glDisable(GL_CLIP_PLANE0);

    glPopMatrix();

    m_refraction.Use(false);
}

Rtt& Water::GetReflection()
{
    return m_reflection;
}

Rtt& Water::GetRefraction()
{
    return m_refraction;
}

void Water::SetSpeed(float speed)
{
    this->m_speed = speed;

    m_shader.Use(true);
    m_shader.SetUniform("speed", m_speed);
    m_shader.Use(false);
}

float Water::GetSpeed() const
{
    return m_speed;
}

void Water::SetDeform(float deform)
{
    this->m_deform = deform;

    m_shader.Use(true);
    m_shader.SetUniform("deform", m_deform);
    m_shader.Use(false);
}

float Water::GetDeform() const
{
    return m_deform;
}

void Water::SetBlend(float blend)
{
    this->m_blend = blend;

    m_shader.Use(true);
    m_shader.SetUniform("blend", m_blend);
    m_shader.Use(false);
}

float Water::GetBlend() const
{
    return m_blend;
}

void Water::SetUvRepeat(Vector2f uvRepeat)
{
    this->m_uvRepeat = uvRepeat;

    m_shader.Use(true);
    m_shader.SetUniform("uvRepeat", m_uvRepeat);
    m_shader.Use(false);
}

Vector2f Water::GetUvRepeat() const
{
    return m_uvRepeat;
}

Water* Water::Clone()
{
    return new Water(*this);
}

void Water::Process()
{

}

void Water::Render()
{
    m_buffer.BindBuffer();
    m_buffer.BindTexture();

    m_shader.Use(true);
    m_shader.SetUniform("timer", (float)clock());

    glClientActiveTexture(GL_TEXTURE0);
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    m_normalMap.Use(true);

    glClientActiveTexture(GL_TEXTURE1);
    glActiveTexture(GL_TEXTURE1);
    glEnable(GL_TEXTURE_2D);
    m_reflection.GetColor().Use(true);

    glClientActiveTexture(GL_TEXTURE2);
    glActiveTexture(GL_TEXTURE2);
    glEnable(GL_TEXTURE_2D);
    m_refraction.GetColor().Use(true);

    glPushMatrix();
    glMultMatrixf(m_matrix);

    m_buffer.Render();

    glPopMatrix();

    glClientActiveTexture(GL_TEXTURE2);
    glActiveTexture(GL_TEXTURE2);
    glDisable(GL_TEXTURE_2D);

    glClientActiveTexture(GL_TEXTURE1);
    glActiveTexture(GL_TEXTURE1);
    glDisable(GL_TEXTURE_2D);

    glClientActiveTexture(GL_TEXTURE0);
    glActiveTexture(GL_TEXTURE0);
    glDisable(GL_TEXTURE_2D);

    m_shader.Use(false);

    m_buffer.BindTexture(false);
    m_buffer.BindBuffer(false);
}

void Water::SetNormalMap(Texture normalMap)
{
    this->m_normalMap = normalMap;
}

Texture Water::GetNormalMap() const
{
    return m_normalMap;
}

void Water::SetSize(Vector2f size)
{
    this->m_size = size;

    Vertex* vertex = m_buffer.Lock();

    for(unsigned i = 0; i < 6; i++)
    {
        if(vertex[i].pos.x)
            vertex[i].pos.x = vertex[i].pos.x > 0 ? m_size.x : -m_size.x;

        if(vertex[i].pos.z)
            vertex[i].pos.z = vertex[i].pos.z > 0 ? m_size.y : -m_size.y;

        m_aabb.Count(vertex[i].pos);
    }

    m_buffer.UnLock();
}

Vector2f Water::GetSize() const
{
    return m_size;
}
