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
    m_shader.parseVertexShader(vertexShader);
    m_shader.parseFragmentShader(fragmentShader);
    m_shader.loadProgram();

    m_shader.use(true);
    m_shader.uniform("normalMap", 0);
    m_shader.uniform("reflexionMap", 1);
    m_shader.uniform("refractionMap", 2);
    m_shader.use(false);

    m_reflection.setFrameSize(128);
    m_reflection.setCaptureColor(true);
    m_reflection.setCaptureDepth(true);

    m_refraction.setFrameSize(128);
    m_refraction.setCaptureColor(true);
    m_refraction.setCaptureDepth(true);

    Vertex vertex[6] = {
        Vertex(Vector3f(1, 0, -1), Vector3f(0, 1, 0), 1, Vector2f(0, 1)),
        Vertex(Vector3f(1, 0, 1), Vector3f(0, 1, 0), 1, Vector2f(0, 0)),
        Vertex(Vector3f(-1, 0, 1), Vector3f(0, 1, 0), 1, Vector2f(1, 0)),
        Vertex(Vector3f(-1, 0, 1), Vector3f(0, 1, 0), 1, Vector2f(1, 0)),
        Vertex(Vector3f(-1, 0, -1), Vector3f(0, 1, 0), 1, Vector2f(1, 1)),
        Vertex(Vector3f(1, 0, -1), Vector3f(0, 1, 0), 1, Vector2f(0, 1)),
    };

    m_buffer.addVertex(vertex, 6);

    m_buffer.compile();

    Node::m_parallelScene = m_parallelScene = scene;
    m_sceneManager = m_parallelScene->getSceneManager();

    m_parallelScene->registerNode(this);
}

Water::~Water()
{
}

Water::Water(const Water& copy)
{
    m_shader.parseVertexShader(vertexShader);
    m_shader.parseFragmentShader(fragmentShader);
    m_shader.loadProgram();

    m_shader.use(true);
    m_shader.uniform("normalMap", 0);
    m_shader.uniform("reflexionMap", 1);
    m_shader.uniform("refractionMap", 2);
    m_shader.use(false);

    m_reflection.setFrameSize(128);
    m_reflection.setCaptureColor(true);
    m_reflection.setCaptureDepth(true);

    m_refraction.setFrameSize(128);
    m_refraction.setCaptureColor(true);
    m_refraction.setCaptureDepth(true);

    Vertex vertex[6] = {
        Vertex(Vector3f(1, 0, -1), Vector3f(0, 1, 0), 1, Vector2f(0, 1)),
        Vertex(Vector3f(1, 0, 1), Vector3f(0, 1, 0), 1, Vector2f(0, 0)),
        Vertex(Vector3f(-1, 0, 1), Vector3f(0, 1, 0), 1, Vector2f(1, 0)),
        Vertex(Vector3f(-1, 0, 1), Vector3f(0, 1, 0), 1, Vector2f(1, 0)),
        Vertex(Vector3f(-1, 0, -1), Vector3f(0, 1, 0), 1, Vector2f(1, 1)),
        Vertex(Vector3f(1, 0, -1), Vector3f(0, 1, 0), 1, Vector2f(0, 1)),
    };

    m_buffer.addVertex(vertex, 6);

    m_buffer.compile();

    *this = copy;

    m_parallelScene->registerNode(this);
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

void Water::beginReflection()
{
    m_reflection.use(true);
    m_reflection.clear();

    glPushMatrix();
    glLoadIdentity();

    Camera* cam = m_parallelScene->getSceneManager()->getCurCamera();
    cam->look();

    glEnable(GL_CLIP_PLANE0);

    float height = m_matrix.getPos().y;

    if(cam->getPos().y > height)
        glClipPlane(GL_CLIP_PLANE0, Vector4<double>(0, -1, 0, 0));

    else
        glClipPlane(GL_CLIP_PLANE0, Vector4<double>(0, 1, 0, 0));


    cam->push();
    cam->setPos(cam->getPos() * Vector3f(1, -1, 1));

    glScalef(1, -1, 1);
}

void Water::endReflection()
{
    glDisable(GL_CLIP_PLANE0);

    Camera* cam = m_parallelScene->getSceneManager()->getCurCamera();
    cam->pop();

    glPopMatrix();

    m_reflection.use(false);
}

void Water::beginRefraction()
{
    m_refraction.use(true);
    m_refraction.clear();

    glPushMatrix();
    glLoadIdentity();

    Camera* cam = m_parallelScene->getSceneManager()->getCurCamera();
    cam->look();

    float height = m_matrix.getPos().y;

    if(cam->getPos().y > height)
    {
        glEnable(GL_CLIP_PLANE0);
        glClipPlane(GL_CLIP_PLANE0, Vector4<double>(0, -1, 0, 0));
    }
}

void Water::endRefraction()
{
    glDisable(GL_CLIP_PLANE0);

    glPopMatrix();

    m_refraction.use(false);
}

Rtt& Water::getReflection()
{
    return m_reflection;
}

Rtt& Water::getRefraction()
{
    return m_refraction;
}

void Water::setSpeed(float speed)
{
    this->m_speed = speed;

    m_shader.use(true);
    m_shader.uniform("speed", m_speed);
    m_shader.use(false);
}

float Water::getSpeed() const
{
    return m_speed;
}

void Water::setDeform(float deform)
{
    this->m_deform = max(min(deform, 1.0f), 0.0f);

    m_shader.use(true);
    m_shader.uniform("deform", m_deform);
    m_shader.use(false);
}

float Water::getDeform() const
{
    return m_deform;
}

void Water::setBlend(float blend)
{
    this->m_blend = max(min(blend, 1.0f), 0.0f);

    m_shader.use(true);
    m_shader.uniform("blend", m_blend);
    m_shader.use(false);
}

float Water::getBlend() const
{
    return m_blend;
}

void Water::setUvRepeat(Vector2f uvRepeat)
{
    this->m_uvRepeat = uvRepeat;

    m_shader.use(true);
    m_shader.uniform("uvRepeat", m_uvRepeat);
    m_shader.use(false);
}

Vector2f Water::getUvRepeat() const
{
    return m_uvRepeat;
}

Water* Water::clone()
{
    return new Water(*this);
}

void Water::process()
{
    if(!m_enable)
        return;

    for_each(m_childs.begin(), m_childs.end(), mem_fun(&Node::process));
}

void Water::render()
{
    m_buffer.bindBuffer();
    m_buffer.bindTexture();

    m_shader.use(true);
    m_shader.uniform("timer", (float)(clock() * 0.001f));

    glClientActiveTexture(GL_TEXTURE0);
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    m_normalMap.use(true);

    glClientActiveTexture(GL_TEXTURE1);
    glActiveTexture(GL_TEXTURE1);
    glEnable(GL_TEXTURE_2D);
    m_reflection.getColor().use(true);

    glClientActiveTexture(GL_TEXTURE2);
    glActiveTexture(GL_TEXTURE2);
    glEnable(GL_TEXTURE_2D);
    m_refraction.getColor().use(true);

    glPushMatrix();
    glMultMatrixf(m_matrix);

    m_buffer.render();

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

    m_shader.use(false);

    m_buffer.bindTexture(false);
    m_buffer.bindBuffer(false);
}

void Water::setNormalMap(Texture normalMap)
{
    this->m_normalMap = normalMap;
}

Texture Water::getNormalMap() const
{
    return m_normalMap;
}

void Water::setSize(Vector2f size)
{
    this->m_size = size;

    Vertex* vertex = m_buffer.bindBuffer().lock();

    for(unsigned i = 0; i < 6; i++)
    {
        if(vertex[i].pos.x)
            vertex[i].pos.x = vertex[i].pos.x > 0 ? m_size.x : -m_size.x;

        if(vertex[i].pos.z)
            vertex[i].pos.z = vertex[i].pos.z > 0 ? m_size.y : -m_size.y;

        m_aabb.count(vertex[i].pos);
    }

    m_buffer.snapshot();

    m_buffer.unlock().unbindBuffer();

    m_shader.use(true);
    m_shader.uniform("endborder", max(m_size.x, m_size.y));
    m_shader.use(false);
}

Vector2f Water::getSize() const
{
    return m_size;
}

Node::CtorMap Water::constructionMap(std::string root)
{
    Node::CtorMap ctormap = Node::constructionMap(root);

    ctormap["class"] = "Water";

    ctormap["normalMap"] = tools::pathScope(root, m_normalMap.getFilename(), false);
    ctormap["size"] = tools::numToStr(m_size);
    ctormap["speed"] = tools::numToStr(m_speed);
    ctormap["deform"] = tools::numToStr(m_deform);
    ctormap["blend"] = tools::numToStr(m_blend);
    ctormap["uvRepeat"] = tools::numToStr(m_uvRepeat);

    return ctormap;
}
