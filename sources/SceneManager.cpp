#include "SceneManager.h"

#include "Texture.h"
#include "Exception.h"
#include "Tools.h"
#include "Rtt.h"

#include "Skybox.h"
#include "Node.h"
#include "Primitives.h"
#include "ObjMesh.h"
#include "Heightmap.h"
#include "Camera.h"
#include "Light.h"
#include "Particles.h"

using namespace std;
using namespace tbe;
using namespace tbe::scene;

// SceneManager ----------------------------------------------------------------

SceneManager::SceneManager()
{
    glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);

    m_frustum = new Frustum;
    m_skybox = new SkyBox;
    m_fog = new Fog;
}

SceneManager::~SceneManager()
{
    ClearAll();

    delete m_frustum;
    delete m_skybox;
    delete m_fog;
}

void SceneManager::UpdateViewParameter()
{
    glViewport(0, 0, m_viewport.x, m_viewport.y);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(m_fovy, m_ratio, m_zNear, m_zFar);
}

void SceneManager::Setup(Vector2i viewport, float ratio, float fovy, float zNear, float zFar)
{
    m_viewport = viewport;
    m_fovy = fovy;
    m_ratio = ratio ? ratio : (float)m_viewport.x / m_viewport.y;
    m_zNear = zNear;
    m_zFar = zFar;

    UpdateViewParameter();
}

void SceneManager::ClearLights()
{
    for(Light::Map::iterator i = m_lights.begin(); i != m_lights.end(); i++)
        delete i->second;

    m_lights.clear();
}

void SceneManager::ClearCameras()
{
    for(Camera::Map::iterator i = m_cameras.begin(); i != m_cameras.end(); i++)
        delete i->second;

    m_cameras.clear();
}

void SceneManager::ClearParallelScenes()
{
    for(unsigned i = 0; i < m_parallelScenes.size(); i++)
        delete m_parallelScenes[i];

    m_parallelScenes.clear();
}

void SceneManager::ClearAll()
{
    ClearLights();
    ClearCameras();
    ClearParallelScenes();

    delete m_skybox, m_skybox = new SkyBox;
    delete m_fog, m_fog = new Fog;
}

void SceneManager::Render(bool setupView)
{
    glMatrixMode(GL_MODELVIEW);

    if(setupView)
        glLoadIdentity();

    // Camera et Skybox
    if(!m_cameras.empty())
    {
        if(setupView)
            m_currentCamera->second->Engine();

        m_frustum->ExtractPlane();

        m_skybox->Render(m_currentCamera->second->GetPos());
    }

    // Mise a jour des lumieres
    for(Light::Map::iterator itt = m_lights.begin(); itt != m_lights.end(); itt++)
        itt->second->Update();

    // Rendue des scenes parallele
    for(unsigned i = 0; i < m_parallelScenes.size(); i++)
        m_parallelScenes[i]->Render();
}

Light* SceneManager::GetDynamicLight(std::string name)
{
    if(m_lights.find(name) == m_lights.end())
        throw Exception("SceneManager::GetDynamicLight; Light not found (%s)", name.c_str());

    return m_lights[name];
}

void SceneManager::DeleteDynamicLight(std::string name)
{
    if(m_lights.find(name) == m_lights.end())
        throw Exception("SceneManager::DeleteDynamicLight; Light not found (%s)", name.c_str());

    delete m_lights[name];
    m_lights.erase(name);
}

Light* SceneManager::ReleaseDynamicLight(std::string name)
{
    if(m_lights.find(name) == m_lights.end())
        throw Exception("SceneManager::ReleaseDynamicLight; Light not found (%s)", name.c_str());

    Light* light = m_lights[name];
    m_lights.erase(name);

    return light;
}

void SceneManager::AddDynamicLight(std::string name, Light* light)
{
    if(name.empty())
        name = tools::NameGen(m_lights);

    else if(m_lights.find(name) != m_lights.end())
        throw Exception("SceneManager::AddDynamicLight; Light already exist (%s)", name.c_str());

    if(!light)
        throw Exception("SceneManager::AddDynamicLight; Try to add a NULL prt light");

    m_lights[name] = light;
}

void SceneManager::SetAmbientLight(Vector4f ambient)
{
    m_ambientLight = ambient;
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, m_ambientLight);
}

void SceneManager::AddParallelScene(ParallelScene* scene)
{
    if(tools::find(m_parallelScenes, scene))
        throw Exception("SceneManager::AddParallelScene; Scene already exist");

    scene->SetSceneManager(this);

    m_parallelScenes.push_back(scene);
}

void SceneManager::ReleaseParallelScene(ParallelScene* scene)
{
    if(tools::find(m_parallelScenes, scene))
        throw Exception("SceneManager::ReleaseParallelScene; ParallelScene not found");

    tools::erase(m_parallelScenes, scene);
}

ParallelScene* SceneManager::ReleaseParallelScene(unsigned index)
{
    if(index > m_parallelScenes.size())
        throw Exception("SceneManager::DeleteParallelScene; ParallelScene not found");

    ParallelScene* ps = m_parallelScenes[index];
    tools::erase(m_parallelScenes, index);

    return ps;
}

void SceneManager::DeleteParallelScene(ParallelScene* scene)
{
    if(tools::find(m_parallelScenes, scene))
        throw Exception("SceneManager::ReleaseParallelScene; ParallelScene not found");

    delete scene;

    tools::erase(m_parallelScenes, scene);
}

void SceneManager::DeleteParallelScene(unsigned index)
{
    if(index > m_parallelScenes.size())
        throw Exception("SceneManager::DeleteParallelScene; ParallelScene not found");

    delete m_parallelScenes[index];

    tools::erase(m_parallelScenes, index);
}

ParallelScene* SceneManager::GetParallelScene(unsigned index)
{
    if(index > m_parallelScenes.size())
        throw tbe::Exception("SceneManager::GetParallelScene; ParallelScene not found");

    return m_parallelScenes[index];
}

Vector4f SceneManager::GetAmbientLight() const
{
    return m_ambientLight;
}

unsigned SceneManager::GetLightCount() const
{
    return m_lights.size();
}

void SceneManager::AddCamera(std::string name, Camera* camera)
{
    if(name.empty())
        name = tools::NameGen(m_cameras);

    else if(m_cameras.find(name) != m_cameras.end())
        throw Exception("SceneManager::AddCamera; Camera already exist (%s)", name.c_str());


    m_cameras[name] = camera;
    m_currentCamera = m_cameras.find(name);
}

Camera* SceneManager::GetCamera(std::string name)
{
    if(m_cameras.find(name) == m_cameras.end())
        throw Exception("SceneManager::GetCamera; Camera not found (%s)", name.c_str());

    return m_cameras[name];
}

void SceneManager::SetCurCamera(Camera* ptr)
{
    for(Camera::Map::iterator itt = m_cameras.begin(); itt != m_cameras.end(); itt++)
        if(itt->second == ptr)
        {
            m_currentCamera = itt;
            return;
        }

    throw Exception("SceneManager::SetCurCamera; Camera not found (%p)", ptr);
}

void SceneManager::SetCurCamera(std::string name)
{
    m_currentCamera = m_cameras.find(name);

    if(m_currentCamera == m_cameras.end())
        throw Exception("SceneManager::SetCurCamera; Camera not found (%s)", name.c_str());
}

Camera* SceneManager::GetCurCamera()
{
    return m_currentCamera->second;
}

Frustum* SceneManager::GetFrustum() const
{
    return m_frustum;
}

SkyBox* SceneManager::GetSkybox() const
{
    return m_skybox;
}

Fog* SceneManager::GetFog() const
{
    return m_fog;
}

void SceneManager::SetViewport(Vector2i viewport)
{
    m_viewport = viewport;
}

Vector2i SceneManager::GetViewport() const
{
    return m_viewport;
}

void SceneManager::SetZFar(float zFar)
{
    this->m_zFar = zFar;
}

float SceneManager::GetZFar() const
{
    return m_zFar;
}

void SceneManager::SetZNear(float zNear)
{
    this->m_zNear = zNear;
}

float SceneManager::GetZNear() const
{
    return m_zNear;
}

void SceneManager::SetFovy(float fovy)
{
    this->m_fovy = fovy;
}

float SceneManager::GetFovy() const
{
    return m_fovy;
}

void SceneManager::SetRatio(float ratio)
{
    this->m_ratio = ratio;
}

float SceneManager::GetRatio() const
{
    return m_ratio;
}

Vector3f SceneManager::ScreenToWorld(Vector2i target, Rtt* rtt)
{
    Vector3f pick;

    Vector2i mouse;
    mouse.x = target.x * rtt->GetFrameSize().x / m_viewport.x - 1;
    mouse.y = target.y * rtt->GetFrameSize().y / m_viewport.y - 1;

    bool useRb = FrameBufferObject::CheckHardware() && rtt->FBO_IsUseRenderBuffer();

    if(useRb)
    {
        rtt->FBO_BlitToTexutre();
        rtt->FBO_SetUseRenderBuffer(false);
    }

    rtt->Use(true);

    pick = ScreenToWorld(mouse);

    rtt->Use(false);

    if(useRb)
    {
        rtt->FBO_SetUseRenderBuffer(true);
    }

    return pick;
}

Vector3f SceneManager::ScreenToWorld(Vector2i target)
{

    int iViewport[4];
    double fModelview[16];
    double fProjection[16];

    float fPosX, fPosY, fPosZ;

    Vector3<double> pick;

    // On récupére la matrice de visualisation
    glGetDoublev(GL_MODELVIEW_MATRIX, fModelview);
    // On récupére la matrice de projection
    glGetDoublev(GL_PROJECTION_MATRIX, fProjection);
    // On récupére le viewport
    glGetIntegerv(GL_VIEWPORT, iViewport);

    fPosX = (float)target.x;
    fPosY = (float)target.y;

    // On lit la profondeur dans le tampon de profondeur
    glReadPixels(int(fPosX), int(fPosY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &fPosZ);

    // Et hop la fonction magic de GLU, pour obtenir la position dans la scene.
    gluUnProject(fPosX, fPosY, fPosZ, fModelview, fProjection, iViewport, &pick.x, &pick.y, &pick.z);

    return Vector3f(float(pick.x), float(pick.y), float(pick.z));
}


#if 0

Box* SceneManager::AddBox(std::string name, Vector3f size, Node* parent)
{
    if(name.empty())
        name = Node::NameGen(m_entity);

    else if(m_entity.find(name) != m_entity.end())
        throw Exception("SceneManager::Add*; Name already exist (%s)", name.c_str());

    Box* O = new Box(size);

    if(parent)
        O->m_parent = parent;
    else
        m_entity[name] = O;
    return O;
}

Sphere* SceneManager::AddSphere(std::string name, Vector3f size, Node* parent)
{
    if(name.empty())
        name = Node::NameGen(m_entity);

    else if(m_entity.find(name) != m_entity.end())
        throw Exception("SceneManager::Add*; Name already exist (%s)", name.c_str());

    Sphere* O = new Sphere(size.XYZ());

    if(parent)
        O->m_parent = parent;
    else
        m_entity[name] = O;
    return O;
}

Heightmap* SceneManager::AddHeightmap(std::string name, std::string filepath, Node* parent)
{
    if(m_entity.find(name) != m_entity.end())
        throw Exception("SceneManager::Add*; Name already exist (%s)", name.c_str());

    Heightmap* O = new Heightmap(filepath);

    if(parent)
        O->m_parent = parent;
    else
        m_entity[name] = O;
    return O;
}

MD2Mesh* SceneManager::AddMD2Mesh(std::string name, std::string filepath, Node* parent)
{
    if(m_entity.find(name) != m_entity.end())
        throw Exception("SceneManager::Add*; Name already exist (%s)", name.c_str());

    MD2Mesh* O = new MD2Mesh(filepath);

    if(parent)
        O->m_parent = parent;
    else
        m_entity[name] = O;
    return O;
}

OBJMesh* SceneManager::AddOBJMesh(std::string name, std::string filepath, Node* parent)
{
    if(m_entity.find(name) != m_entity.end())
        throw Exception("SceneManager::Add*; Name already exist (%s)", name.c_str());

    OBJMesh* O = new OBJMesh(filepath);

    if(parent)
        O->m_parent = parent;
    else
        m_entity[name] = O;
    return O;
}
#endif
