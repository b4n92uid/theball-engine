#ifndef CameraFile
#define CameraFile

#include "GLee.h"

#include <map>
#include <stack>

#include "Mathematics.h"
#include "Eventrecv.h"

namespace tbe
{
namespace scene
{

/**
 * \brief Gestion de la Camera
 * Objet qui represant le poit de vue de la scene
 * inclus une fonction de rotation
 */
class Camera
{
public:

    typedef std::vector<Camera*> Array;
    typedef std::map<std::string, Camera*> Map;

    enum CameraType
    {
        /// le vecteur 'target' est relative au vecteur 'pos'
        TARGET_RELATIVE,
        /// le vecteur 'target' est absolue
        TARGET_ABSOLUTE,
    };

    Camera(CameraType type = TARGET_ABSOLUTE, Vector3f posistion = 0, Vector3f target = Vector3f(0, 0, 1), float sensivity = 0.1);
    virtual ~Camera();

    void Engine();

    void Push();
    void Pop();

    /// Rotation du vecteur target (en degres)
    void SetRotate(Vector2f rel);

    /// Rotation relative du vecteur target (en degres)
    void SetRelRotate(Vector2f rel);

    void SetSensivity(float value);
    float GetSensivity() const;

    void SetUp(Vector3f up);
    Vector3f GetUp() const;

    void SetLeft(Vector3f left);
    Vector3f GetLeft() const;

    void SetTarget(Vector3f target);
    Vector3f GetTarget() const;

    void SetPos(Vector3f pos);
    Vector3f GetPos() const;

    void SetCameraType(CameraType cameraType);
    CameraType GetCameraType() const;

protected:
    CameraType m_cameraType;
    Vector3f m_pos, m_target, m_left, m_up;
    float m_theta, m_phi, m_sensivity;
    std::stack<Vector3f> m_stackVector;
    std::stack<float> m_stackFloat;

};

/**
 * \brief Class de base pour les cameras pré-conçue
 */
class UserCamera : public Camera
{
public:
    UserCamera();
    virtual ~UserCamera();
    virtual void OnEvent(EventManager* event) = 0;
};

/**
 * \brief Caméra pré-conçue pour la vue en orbite
 */
class OrbitalCamera : public UserCamera
{
public:
    OrbitalCamera();
    void OnEvent(EventManager* event);

    void SetDistance(float distance);
    float GetDistance() const;

    void SetCenter(Vector3f center);
    Vector3f GetCenter() const;

protected:
    Vector3f m_center;
    float m_distance;
};

/**
 * \brief Caméra pré-conçue pour la vue en libre mouvements (Free Fly)
 */
class FreeFlyCamera : public UserCamera
{
public:
    FreeFlyCamera();
    void OnEvent(EventManager* event);

    void SetSpeed(float speed);
    float GetSpeed() const;

protected:
    float m_speed;
};

}
}

#endif
