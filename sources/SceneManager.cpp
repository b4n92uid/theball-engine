#include <map>

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

    m_currentCamera = m_cameras.end();

    m_fovy = m_zNear = m_zFar = m_ratio = 0;

    m_rootNode = new BullNode;
    m_rootNode->SetSceneManager(this);
}

SceneManager::~SceneManager()
{
    ClearAll();

    delete m_frustum;
    delete m_skybox;
    delete m_fog;

    delete m_rootNode;
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

void SceneManager::ClearCameras()
{
    for(unsigned i = 0; i < m_cameras.size(); i++)
        delete m_cameras[i];

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
    m_rootNode->ClearAllChild();

    ClearCameras();
    ClearParallelScenes();

    m_skybox->Clear();
    m_fog->Clear();
}

void SceneManager::Render(bool setupView)
{
    glMatrixMode(GL_MODELVIEW);

    if(setupView)
        glLoadIdentity();

    // Camera et Skybox
    if(!m_cameras.empty() && m_currentCamera != m_cameras.end())
    {
        if(setupView)
            (*m_currentCamera)->Engine();

        m_frustum->ExtractPlane();

        m_skybox->Render((*m_currentCamera)->GetPos());
    }

    // Traitement Logique
    m_rootNode->Process();

    // Traitement Graphique : Rendue des scenes parallele
    std::for_each(m_parallelScenes.begin(), m_parallelScenes.end(),
                  mem_fun(&ParallelScene::Render));
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

Iterator<ParallelScene*> SceneManager::GetParallelSceneIterator()
{
    return Iterator<ParallelScene*>(m_parallelScenes);
}

void SceneManager::AddCamera(Camera* camera)
{
    if(tools::find(m_cameras, camera))
        throw Exception("SceneManager::AddCamera; Camera already exist");

    m_cameras.push_back(camera);
    m_currentCamera = --m_cameras.end();
}

void SceneManager::SetCurCamera(Camera* camera)
{
    if(!tools::find(m_cameras, camera))
        throw Exception("SceneManager::SetCurCamera; Camera not found");

    m_currentCamera = find(m_cameras.begin(), m_cameras.end(), camera);
}

Camera* SceneManager::GetCurCamera()
{
    return *m_currentCamera;
}

Iterator<Camera*> SceneManager::GetCameraIterator()
{
    return Iterator<Camera*>(m_cameras);
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

Matrix4f SceneManager::computeBillboard(Vector3f obj, Matrix4f init, Vector3f cam)
{
    Matrix4f rotation = init;

    Vector3f look(0, 0, 1);
    Vector3f campos = cam.IsNull() ? (*m_currentCamera)->GetPos() : cam;

    Vector3f proj = campos - obj;
    proj.y = 0;
    proj.Normalize();

    Vector3f up = Vector3f::Cross(look, proj);
    up.Normalize();

    float rotateH = Vector3f::Dot(look, proj);

    Vector3f proj2 = campos - obj;
    proj2.Normalize();

    float rotateV = Vector3f::Dot(proj, proj2);

    if(campos.y < obj.y)
        rotation.SetRotate(acos(rotateV), Vector3f(1, 0, 0));
    else
        rotation.SetRotate(acos(rotateV), Vector3f(-1, 0, 0));

    rotation.SetRotate(acos(rotateH), up);

    return rotation;
}

void SceneManager::SetAmbientLight(Vector4f ambient)
{
    m_ambientLight = ambient;
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, m_ambientLight);
}

Vector4f SceneManager::GetAmbientLight() const
{
    return m_ambientLight;
}

Node* SceneManager::GetRootNode() const
{
    return m_rootNode;
}
