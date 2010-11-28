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

namespace tbe
{

namespace scene
{

class SkyBox;

/**
 * \brief Gestionnaire de scene
 * inclus des fonctions de gestion des scenes parallel,
 * des lumiers et encapsule les gestionnaires Skybox, Fog
 * et du test du frustum culling ansi
 * que les propri�t�s relative au rendue en perspective
 */

class SceneManager
{
public:

    /// Ctor
    SceneManager();

    /// Dtor
    ~SceneManager();

    /// Supprime tout les lumieres
    void ClearLights();

    /// Supprime tout les scene paralleles
    void ClearParallelScenes();

    /// Supprime tout les cam�ras
    void ClearCameras();

    /// Vide tout la scene
    void ClearAll();

    /// Configuration
    void Setup(Vector2i viewport, float ratio, float fovy, float zNear, float zFar);

    /// Ajouter une sous-scene
    void AddParallelScene(ParallelScene* scene);

    /// Acc�s a une sous-scene
    ParallelScene* GetParallelScene(unsigned index);

    template <typename T> T* GetParallelScene(unsigned index)
    {
        return dynamic_cast<T*>(GetParallelScene(index));
    }

    /// Destruite une scene parallele
    void DeleteParallelScene(unsigned index);
    void DeleteParallelScene(ParallelScene* scene);

    /// D�tache une scene parallele du gestionnaire
    ParallelScene* ReleaseParallelScene(unsigned index);
    void ReleaseParallelScene(ParallelScene* scene);

    /// Rendue
    void Render(bool setupView = true);

    // Camera ------------------------------------------------------------------

    /// Ajoute une camera
    void AddCamera(std::string name, Camera* camera);

    /// Acc�s au camera name
    Camera* GetCamera(std::string name);

    /// Specifier la camera courante
    void SetCurCamera(std::string name);

    void SetCurCamera(Camera* ptr);

    /// Acc�s a la camera courante
    Camera* GetCurCamera();

    // Light -------------------------------------------------------------------

    /// Ajoute une lumiere
    void AddDynamicLight(std::string name, Light* light);

    /// Acc�s a une lumiere
    Light* GetDynamicLight(std::string name);

    // D�tache une lumiere par son nom du getionnaire (sans faire un delete)
    Light* ReleaseDynamicLight(std::string name);

    /// D�trute une lumiere par son nom
    void DeleteDynamicLight(std::string name);

    /// Specifer la composant ambient de la scene
    void SetAmbientLight(Vector4f color);
    Vector4f GetAmbientLight() const;

    unsigned GetLightCount() const;

    // External Manipulator ----------------------------------------------------

    Frustum* GetFrustum() const;

    SkyBox* GetSkybox() const;

    Fog* GetFog() const;

    // Video mode attributs ----------------------------------------------------

    void SetViewport(Vector2i viewport);
    Vector2i GetViewport() const;

    float GetFovy() const;
    void SetFovy(float fovy);

    void SetZNear(float zNear);
    float GetZNear() const;

    void SetZFar(float zFar);
    float GetZFar() const;

    void SetRatio(float ratio);
    float GetRatio() const;

    void UpdateViewParameter();

    // Picking -----------------------------------------------------------------

    /// Fonction de picking 2D->3D
    Vector3f ScreenToWorld(Vector2i target);
    Vector3f ScreenToWorld(Vector2i target, Rtt* rtt);

protected:

    ParallelScene::Array m_parallelScenes;

    Vector4f m_ambientLight;
    Light::Map m_lights;

    Camera::Map m_cameras;
    Camera::Map::iterator m_currentCamera;

    Vector2i m_viewport;
    float m_fovy, m_zNear, m_zFar, m_ratio;

    Frustum* m_frustum;

    Fog* m_fog;
    SkyBox* m_skybox;
};

}
}

#endif // SCENEMANAGER_H
