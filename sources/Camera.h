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

    void look();

    void push();
    void pop();

    /// Rotation du vecteur target (en degres)
    void setRotate(Vector2f rel);

    /// Rotation relative du vecteur target (en degres)
    void rotate(Vector2f rel);

    void setSensivity(float value);
    float getSensivity() const;

    void setUp(Vector3f up);
    Vector3f getUp() const;

    void setLeft(Vector3f left);
    Vector3f getLeft() const;

    void setTarget(Vector3f target);
    Vector3f getTarget() const;

    void setPos(Vector3f pos);
    Vector3f getPos() const;

    void setCameraType(CameraType cameraType);
    CameraType getCameraType() const;

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

    virtual void onEvent(EventManager* event) = 0;

    struct Moves
    {
        int FORWARD;
        int BACKWARD;
        int STRAFLEFT;
        int STRAFRIGHT;

        int UP;
        int DOWN;

        int SPEED;

    } keybind;
};

/**
 * \brief Caméra pré-conçue pour la vue en orbite
 */
class OrbitalCamera : public UserCamera
{
public:
    OrbitalCamera();

    void onEvent(EventManager* event);

    void setDistance(float distance);
    float getDistance() const;

    void setCenter(Vector3f center);
    Vector3f getCenter() const;

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

    void onEvent(EventManager* event);

    void setSpeed(float speed);
    float getSpeed() const;

protected:
    float m_speed;
};

}
}

#endif
