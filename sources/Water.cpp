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
#include "WaterParallelScene.h"

#include <time.h>

const char* vertexShader =
        "varying vec3 eye;\n"
        "varying vec3 light;\n"

        "varying vec4 projectionCoordinates;\n"

        "uniform vec2 uvRepeat;\n"

        "void main()\n"
        "{\n"
        "    // Coordonés de projection\n"
        "    projectionCoordinates = gl_ModelViewProjectionMatrix * gl_Vertex;\n"

        "    gl_TexCoord[0].xy   = gl_MultiTexCoord0.xy * uvRepeat;\n"
        "    gl_Position         = ftransform();\n"
        "    vec3 vVertex = vec3(gl_ModelViewMatrix * gl_Vertex);\n"

        "    // Vecteur vertex -> lumiere (Lumiere dirictionelle)\n"
        "    light = gl_LightSource[0].position.xyz;\n"

        "    // Vecteur vertex -> caméra\n"
        "    eye = -vVertex;\n"

        "    // Il faut se mettre dans la TBN\n"
        "    vec3 T = normalize(gl_NormalMatrix * vec3(0, 0, 1));\n"
        "    vec3 B = normalize(gl_NormalMatrix * vec3(1, 0, 0));\n"
        "    vec3 N = normalize(gl_NormalMatrix * vec3(0, 1, 0));\n"

        "    vec3 tbnEye;\n"
        "    tbnEye.x = dot(eye, T);\n"
        "    tbnEye.y = dot(eye, B);\n"
        "    tbnEye.z = dot(eye, N);\n"
        "    eye = tbnEye;\n"

        "    vec3 tbnLight;\n"
        "    tbnLight.x = dot(light, T);\n"
        "    tbnLight.y = dot(light, B);\n"
        "    tbnLight.z = dot(light, N);\n"
        "    light = tbnLight;\n"
        "}";

const char* fragmentShader =
        "varying vec3 eye;\n"
        "varying vec3 light;\n"

        "varying vec4 projectionCoordinates;\n"

        "uniform sampler2D colorMap;\n"
        "uniform sampler2D normalMap;\n"
        "uniform sampler2D reflexionMap;\n"
        "uniform sampler2D refractionMap;\n"

        "uniform float timer;\n"
        "uniform float blend;\n"
        "uniform float deform;\n"
        "uniform float speed;\n"
        "uniform float endborder;\n"

        "void main()\n"
        "{\n"
        "    // -- On passe les normals dans un optique de [-1;1] plutot que [0;1]\n"
        "    vec3 n1 = texture2D(normalMap, gl_TexCoord[0].st + vec2(timer, -timer) * speed).xyz * 2.0 - 1.0;\n"
        "    vec3 n2 = texture2D(normalMap, gl_TexCoord[0].st + vec2(-timer, timer) * speed).xyz * 2.0 - 1.0;\n"
        "    vec3 N = normalize(n1 + n2);\n"

        "    // -- Coorodoné de texture de projection\n"
        "    vec2 projCoord = projectionCoordinates.xy / projectionCoordinates.w;\n"
        "    projCoord = (projCoord + 1.0) * 0.5;\n"
        "    projCoord = clamp(projCoord, 0.0, 1.0);\n"
        "    projCoord += N * deform;"

        "    // -- Intensité spéculaire\n"
        "    vec3 L = normalize(light);\n"
        "    vec3 E = normalize(eye);\n"

        "    // Le reflet du vecteur incident 'lumiere' sur la normal de la surface\n"
        "    vec3 ref = normalize(reflect(-L, N));\n"

        "    // L'angle entre la caméra et le reflet de la lumiere\n"
        "    float stemp = clamp(dot(E, ref), 0.0, 1.0);\n"

        "    // Intensité de la spéculaire\n"
        "    float ispecular = pow(stemp, 16.0);\n"

        "    // -- Calcule de Reflection & Refraction\n"
        "    vec4 reflexion = texture2D(reflexionMap, projCoord);\n"
        "    vec4 refraction = texture2D(refractionMap, projCoord);\n"

        "    gl_FragColor = mix(reflexion, refraction, blend);\n"
        "    gl_FragColor += gl_LightSource[0].specular * ispecular;\n"

        "    // Attinuation par transparence sur les bords\n"
        "    gl_FragColor.a = 1.0 - (gl_FragCoord.z / gl_FragCoord.w) / endborder;\n"
        "}";

using namespace std;
using namespace tbe;
using namespace tbe::scene;

Water::Water(WaterParallelScene* scene)
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

    Node::m_parallelScene = m_parallelScene = scene;
    m_sceneManager = m_parallelScene->GetSceneManager();

    m_parallelScene->Register(this);
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

    m_parallelScene->Register(this);
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
    m_sceneManager = copy.m_sceneManager;

    Node::m_parallelScene = m_parallelScene = copy.m_parallelScene;

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
        glClipPlane(GL_CLIP_PLANE0, Vector4<double>(0, -1, 0, 0));

    else
        glClipPlane(GL_CLIP_PLANE0, Vector4<double>(0, 1, 0, 0));


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
    this->m_deform = max(min(deform, 1.0f), 0.0f);

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
    this->m_blend = max(min(blend, 1.0f), 0.0f);

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
    if(!m_enable)
        return;

    for_each(m_childs.begin(), m_childs.end(), mem_fun(&Node::Process));
}

void Water::Render()
{
    m_buffer.BindBuffer();
    m_buffer.BindTexture();

    m_shader.Use(true);
    m_shader.SetUniform("timer", (float)(clock() * 0.001f));

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

    m_shader.Use(true);
    m_shader.SetUniform("endborder", max(m_size.x, m_size.y));
    m_shader.Use(false);
}

Vector2f Water::GetSize() const
{
    return m_size;
}

Node::CtorMap Water::ConstructionMap(std::string root)
{
    Node::CtorMap ctormap = Node::ConstructionMap(root);

    ctormap["class"] = "Water";

    ctormap["normalMap"] = m_normalMap.GetFilename();
    ctormap["size"] = m_size;
    ctormap["speed"] = m_speed;
    ctormap["deform"] = m_deform;
    ctormap["blend"] = m_blend;
    ctormap["uvRepeat"] = m_uvRepeat;

    return ctormap;
}
