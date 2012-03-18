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
    m_rootNode->setSceneManager(this);
}

SceneManager::~SceneManager()
{
    clearAll();

    delete m_frustum;
    delete m_skybox;
    delete m_fog;

    delete m_rootNode;
}

void SceneManager::updateViewParameter()
{
    glViewport(0, 0, m_viewport.x, m_viewport.y);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(m_fovy, m_ratio, m_zNear, m_zFar);
}

void SceneManager::setup(Vector2i viewport, float ratio, float fovy, float zNear, float zFar)
{
    m_viewport = viewport;
    m_fovy = fovy;
    m_ratio = ratio ? ratio : (float)m_viewport.x / m_viewport.y;
    m_zNear = zNear;
    m_zFar = zFar;

    updateViewParameter();
}

void SceneManager::clearCameras()
{
    for(unsigned i = 0; i < m_cameras.size(); i++)
        delete m_cameras[i];

    m_cameras.clear();
}

void SceneManager::clearParallelScenes(bool deleteHim)
{
    if(deleteHim)
    {
        for(unsigned i = 0; i < m_parallelScenes.size(); i++)
            delete m_parallelScenes[i];

        m_parallelScenes.clear();
    }

    else
        for(unsigned i = 0; i < m_parallelScenes.size(); i++)
            m_parallelScenes[i]->clear();

}

void SceneManager::clearAll()
{
    m_rootNode->clearAllChild();

    clearCameras();
    clearParallelScenes();

    m_skybox->clear();
    m_fog->clear();
}

void SceneManager::render(bool setupView)
{
    glMatrixMode(GL_MODELVIEW);

    if(setupView)
        glLoadIdentity();

    // Camera et Skybox
    if(!m_cameras.empty() && m_currentCamera != m_cameras.end())
    {
        if(setupView)
            (*m_currentCamera)->look();

        m_frustum->extractPlane();

        m_skybox->render((*m_currentCamera)->getPos());
    }

    // Traitement Logique
    m_rootNode->process();

    // Traitement Graphique : Rendue des scenes parallele
    std::for_each(m_parallelScenes.begin(), m_parallelScenes.end(),
                  mem_fun(&ParallelScene::render));
}

void SceneManager::addParallelScene(ParallelScene* scene)
{
    if(tools::find(m_parallelScenes, scene))
        throw Exception("SceneManager::AddParallelScene; Scene already exist");

    scene->setSceneManager(this);

    m_parallelScenes.push_back(scene);
}

void SceneManager::releaseParallelScene(ParallelScene* scene)
{
    if(tools::find(m_parallelScenes, scene))
        throw Exception("SceneManager::ReleaseParallelScene; ParallelScene not found");

    tools::erase(m_parallelScenes, scene);
}

ParallelScene* SceneManager::releaseParallelScene(unsigned index)
{
    if(index > m_parallelScenes.size())
        throw Exception("SceneManager::DeleteParallelScene; ParallelScene not found");

    ParallelScene* ps = m_parallelScenes[index];
    m_parallelScenes.erase(m_parallelScenes.begin() + index);

    return ps;
}

void SceneManager::deleteParallelScene(ParallelScene* scene)
{
    if(tools::find(m_parallelScenes, scene))
        throw Exception("SceneManager::ReleaseParallelScene; ParallelScene not found");

    delete scene;

    tools::erase(m_parallelScenes, scene);
}

void SceneManager::deleteParallelScene(unsigned index)
{
    if(index > m_parallelScenes.size())
        throw Exception("SceneManager::DeleteParallelScene; ParallelScene not found");

    delete m_parallelScenes[index];

    m_parallelScenes.erase(m_parallelScenes.begin() + index);
}

ParallelScene* SceneManager::getParallelScene(unsigned index)
{
    if(index > m_parallelScenes.size())
        throw tbe::Exception("SceneManager::GetParallelScene; ParallelScene not found");

    return m_parallelScenes[index];
}

Iterator<ParallelScene*> SceneManager::getParallelSceneIterator()
{
    return Iterator<ParallelScene*>(m_parallelScenes);
}

void SceneManager::addCamera(Camera* camera)
{
    if(tools::find(m_cameras, camera))
        throw Exception("SceneManager::AddCamera; Camera already exist");

    m_cameras.push_back(camera);
    m_currentCamera = --m_cameras.end();
}

void SceneManager::setCurCamera(Camera* camera)
{
    if(!tools::find(m_cameras, camera))
        throw Exception("SceneManager::SetCurCamera; Camera not found");

    m_currentCamera = find(m_cameras.begin(), m_cameras.end(), camera);
}

Camera* SceneManager::getCurCamera()
{
    return *m_currentCamera;
}

Iterator<Camera*> SceneManager::getCameraIterator()
{
    return Iterator<Camera*>(m_cameras);
}

Frustum* SceneManager::getFrustum() const
{
    return m_frustum;
}

SkyBox* SceneManager::getSkybox() const
{
    return m_skybox;
}

Fog* SceneManager::getFog() const
{
    return m_fog;
}

void SceneManager::setViewport(Vector2i viewport)
{
    m_viewport = viewport;
}

Vector2i SceneManager::getViewport() const
{
    return m_viewport;
}

void SceneManager::setZFar(float zFar)
{
    this->m_zFar = zFar;
}

float SceneManager::getZFar() const
{
    return m_zFar;
}

void SceneManager::setZNear(float zNear)
{
    this->m_zNear = zNear;
}

float SceneManager::getZNear() const
{
    return m_zNear;
}

void SceneManager::setFovy(float fovy)
{
    this->m_fovy = fovy;
}

float SceneManager::getFovy() const
{
    return m_fovy;
}

void SceneManager::setRatio(float ratio)
{
    this->m_ratio = ratio;
}

float SceneManager::getRatio() const
{
    return m_ratio;
}

Vector3f SceneManager::screenToWorld(Vector2i target, Rtt* rtt)
{
    Vector3f pick;

    Vector2i mouse;
    mouse.x = target.x * rtt->getFrameSize().x / m_viewport.x - 1;
    mouse.y = target.y * rtt->getFrameSize().y / m_viewport.y - 1;

    bool useRb = FrameBufferObject::checkHardware() && rtt->FBO_IsUseRenderBuffer();

    if(useRb)
    {
        rtt->FBO_BlitToTexutre();
        rtt->FBO_SetUseRenderBuffer(false);
    }

    rtt->use(true);

    pick = screenToWorld(mouse);

    rtt->use(false);

    if(useRb)
    {
        rtt->FBO_SetUseRenderBuffer(true);
    }

    return pick;
}

Vector3f SceneManager::screenToWorld(Vector2i target)
{
    int iViewport[4] = {0};
    double fModelview[16] = {0};
    double fProjection[16] = {0};

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

Quaternion SceneManager::computeBillboard(Vector3f obj, Vector3f cam, Vector2b diri)
{
    Quaternion rotation;

    Vector3f look(0, 0, 1);
    Vector3f campos = !cam ? (*m_currentCamera)->getPos() : cam;

    Vector3f proj = campos - obj;
    proj.y = 0;
    proj.normalize();

    Vector3f up = Vector3f::cross(look, proj);
    up.normalize();

    float rotateH = Vector3f::dot(look, proj);

    Vector3f proj2 = campos - obj;
    proj2.normalize();

    float rotateV = Vector3f::dot(proj, proj2);

    if(diri.y)
    {
        if(campos.y < obj.y)
            rotation *= Quaternion(acos(rotateV), Vector3f(diri.x ? 1 : -1, 0, 0));
        else
            rotation *= Quaternion(acos(rotateV), Vector3f(diri.x ? -1 : 1, 0, 0));
    }

    if(diri.x)
        rotation *= Quaternion(acos(-rotateH), up);

    return rotation;
}

void SceneManager::setAmbientLight(Vector4f ambient)
{
    m_ambientLight = ambient;
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, m_ambientLight);
}

Vector4f SceneManager::getAmbientLight() const
{
    return m_ambientLight;
}

Node* SceneManager::getRootNode() const
{
    return m_rootNode;
}
