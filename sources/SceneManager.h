#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include <iostream>
#include <vector>
#include <string>
#include <map>

#include "Mathematics.h"
#include "Texture.h"
#include "Exception.h"
#include "ParallelScene.h"

#include "Light.h"
#include "Camera.h"
#include "Frustum.h"
#include "Fog.h"
#include "PostProcessManager.h"
#include "Iterator.h"

namespace tbe
{

namespace scene
{

class SkyBox;
class ShadowMap;

/**
 * \brief Gestionnaire de scene
 * inclus des fonctions de gestion des scenes parallel,
 * des lumiers et encapsule les gestionnaires Skybox, Fog
 * et du test du frustum culling ansi
 * que les propriétés relative au rendue en perspective
 */

class SceneManager
{
public:

    /// Ctor
    SceneManager();

    /// Dtor
    ~SceneManager();

    /// Supprime tout les scene paralleles
    void clearParallelScenes(bool deleteThem = true);

    /// Supprime tout les caméras
    void clearCameras();

    /// Vide tout la scene
    void clearAll();

    /// Configuration
    void setup(Vector2i viewport, float ratio, float fovy, float zNear, float zFar);

    /// Ajouter une sous-scene
    void addParallelScene(ParallelScene* scene);

    /// Accès a une sous-scene
    ParallelScene* getParallelScene(unsigned index);

    /// Destruite une scene parallele
    void deleteParallelScene(unsigned index);
    void deleteParallelScene(ParallelScene* scene);

    /// Détache une scene parallele du gestionnaire
    ParallelScene* releaseParallelScene(unsigned index);
    void releaseParallelScene(ParallelScene* scene);

    Iterator<ParallelScene*> getParallelSceneIterator();

    /// Rendue
    void render(bool setupView = true);

    // Node --------------------------------------------------------------------

    Node* getRootNode() const;

    // Camera ------------------------------------------------------------------

    /// Ajoute une camera
    void addCamera(Camera* camera);

    /// Specifier la camera courante
    void setCurCamera(Camera* camera);

    /// Accès a la camera courante
    Camera* getCurCamera();

    void deleteCamera(Camera* camera);

    void releaseCamera(Camera* camera);

    Iterator<Camera*> getCameraIterator();

    // Light -------------------------------------------------------------------

    /// Specifer la composant ambient de la scene
    void setAmbientLight(Vector4f color);

    /// Renvois la composant ambient de la scene
    Vector4f getAmbientLight() const;

    // External Manipulator ----------------------------------------------------

    Frustum* getFrustum() const;

    SkyBox* getSkybox() const;

    Fog* getFog() const;

    ShadowMap* getShadowMap() const;

    // View mode attributs ----------------------------------------------------

    void setViewport(Vector2i viewport);
    Vector2i getViewport() const;

    float getFovy() const;
    void setFovy(float fovy);

    void setZNear(float zNear);
    float getZNear() const;

    void setZFar(float zFar);
    float getZFar() const;

    void setRatio(float ratio);
    float getRatio() const;

    void updateViewParameter();

    Matrix4 getProjectionMatrix() const;
    Matrix4 getViewMatrix() const;

    ticks::Clock getTimestamp() const;

    // Picking -----------------------------------------------------------------

    /// Picking 2D->3D
    Vector3f screenToWorld(Vector2i target);

    /// Picking 2D->3D, dans le cas d'utilisation des PPE
    Vector3f screenToWorld(Vector2i target, Rtt* rtt);

    /// Calcule de la matrice de rotation d'un billboard (face a la caméra)
    Quaternion computeBillboard(Vector3f obj, Vector3f cam = 0, Vector2b diri = true);

protected:

    ParallelScene::Array m_parallelScenes;

    Vector4f m_ambientLight;

    Camera::Array m_cameras;
    Camera::Array::iterator m_currentCamera;

    Node* m_rootNode;

    Vector2i m_viewport;
    float m_fovy, m_zNear, m_zFar, m_ratio;

    ShadowMap* m_shadowMap;
    Frustum* m_frustum;

    Fog* m_fog;
    SkyBox* m_skybox;

    Matrix4 m_projectionMatrix;
    Matrix4 m_viewMatrix;

    ticks::Clock m_timestamp;
};

}
}

#endif // SCENEMANAGER_H
