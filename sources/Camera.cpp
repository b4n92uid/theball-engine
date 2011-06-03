#include "Camera.h"

#include <GL/glu.h>

using namespace std;
using namespace tbe;
using namespace tbe::scene;

Camera::Camera(CameraType type, Vector3f posistion, Vector3f target, float sensivity)
{
    m_pos = posistion;
    m_target = target;
    m_left = target.rotate(90, 0);
    m_up = Vector3f(0, 1, 0);

    m_sensivity = sensivity;

    m_phi = 0;
    m_theta = 0;

    m_cameraType = type;
}

Camera::~Camera()
{

}

void Camera::push()
{
    m_stackVector.push(m_pos);
    m_stackVector.push(m_target);
    m_stackVector.push(m_left);
    m_stackVector.push(m_up);

    m_stackFloat.push(m_theta);
    m_stackFloat.push(m_phi);
    m_stackFloat.push(m_sensivity);
}

void Camera::pop()
{
    m_up = m_stackVector.top();
    m_stackVector.pop();
    m_left = m_stackVector.top();
    m_stackVector.pop();
    m_target = m_stackVector.top();
    m_stackVector.pop();
    m_pos = m_stackVector.top();
    m_stackVector.pop();

    m_sensivity = m_stackFloat.top();
    m_stackFloat.pop();
    m_phi = m_stackFloat.top();
    m_stackFloat.pop();
    m_theta = m_stackFloat.top();
    m_stackFloat.pop();
}

void Camera::look()
{
    switch(m_cameraType)
    {
        case TARGET_RELATIVE:
        {
            Vector3f set = m_pos + m_target;
            gluLookAt(m_pos.x, m_pos.y, m_pos.z, set.x, set.y, set.z, 0, 1, 0);
        }
            break;

        case TARGET_ABSOLUTE:
            gluLookAt(m_pos.x, m_pos.y, m_pos.z, m_target.x, m_target.y, m_target.z, 0, 1, 0);
            break;
    }
}

void Camera::setRotate(Vector2f rel)
{
    m_theta = rel.x;
    m_phi = rel.y;

    if(m_phi > 89)
        m_phi = 89;
    else if(m_phi < -89)
        m_phi = -89;

    m_target.normalize();
    m_target.rotate(m_theta, m_phi);

    m_left.normalize();
    m_left.rotate(m_theta - 90, m_phi);

    m_up.normalize();
    m_up.rotate(m_theta, m_phi + 90);
}

void Camera::rotate(Vector2f rel)
{
    m_theta -= rel.x*m_sensivity;
    m_phi += rel.y*m_sensivity;

    if(m_phi > 89)
        m_phi = 89;
    else if(m_phi < -89)
        m_phi = -89;

    m_target.normalize();
    m_target.rotate(m_theta, m_phi);

    m_left.normalize();
    m_left.rotate(m_theta - 90, 0);

    m_up.normalize();
    m_up.rotate(0, m_phi + 90);
}

void Camera::setSensivity(float value)
{
    m_sensivity = value;
}

float Camera::getSensivity() const
{
    return m_sensivity;
}

void Camera::setUp(Vector3f up)
{
    this->m_up = up;
}

Vector3f Camera::getUp() const
{
    return m_up;
}

void Camera::setLeft(Vector3f left)
{
    this->m_left = left;
}

Vector3f Camera::getLeft() const
{
    return m_left;
}

void Camera::setTarget(Vector3f target)
{
    this->m_target = target;
}

Vector3f Camera::getTarget() const
{
    return m_target;
}

void Camera::setPos(Vector3f pos)
{
    this->m_pos = pos;
}

Vector3f Camera::getPos() const
{
    return m_pos;
}

void Camera::setCameraType(CameraType cameraType)
{
    this->m_cameraType = cameraType;
}

Camera::CameraType Camera::getCameraType() const
{
    return m_cameraType;
}

void Camera::setAngle(float phi, float theta)
{
    m_phi = phi;
    m_theta = theta;
}

// UserCamera ------------------------------------------------------------------

UserCamera::UserCamera() : Camera(TARGET_RELATIVE)
{
    keybind.FORWARD = 'z';
    keybind.BACKWARD = 's';
    keybind.STRAFLEFT = 'q';
    keybind.STRAFRIGHT = 'd';

    keybind.UP = EventManager::KEY_SPACE;
    keybind.DOWN = EventManager::KEY_LCTRL;

    keybind.SPEED = EventManager::KEY_LSHIFT;
}

UserCamera::~UserCamera()
{
}

// OrbitalCamera ---------------------------------------------------------------

OrbitalCamera::OrbitalCamera()
{
    keybind.FORWARD = EventManager::MOUSE_BUTTON_WHEEL_UP;
    keybind.BACKWARD = EventManager::MOUSE_BUTTON_WHEEL_DOWN;

    m_distance = 16;
    m_pos = -m_target * m_distance + m_center;
}

void OrbitalCamera::onEvent(EventManager* event)
{
    if(event->notify == EventManager::EVENT_MOUSE_MOVE)
        rotate(event->mousePosRel);

    float speed = 0.1;

    if(event->mouseState[keybind.SPEED])
        speed *= 4;

    if(event->mouseState[keybind.FORWARD] && m_distance > 0)
        m_distance -= m_distance * speed;

    if(event->mouseState[keybind.BACKWARD])
        m_distance += m_distance * speed;

    m_pos = m_center - m_target * m_distance;
}

void OrbitalCamera::setDistance(float distance)
{
    this->m_distance = distance;
    m_pos = m_center - m_target * m_distance;
}

float OrbitalCamera::getDistance() const
{
    return m_distance;
}

void OrbitalCamera::setCenter(Vector3f center)
{
    this->m_center = center;
    m_pos = m_center - m_target * m_distance;
}

Vector3f OrbitalCamera::getCenter() const
{
    return m_center;
}

// FreeFlyCamera ---------------------------------------------------------------

FreeFlyCamera::FreeFlyCamera()
{
    m_pos = 8;
    m_target = -1;
    m_speed = 1;
}

void FreeFlyCamera::onEvent(EventManager* event)
{
    if(event->notify == EventManager::EVENT_MOUSE_MOVE)
        rotate(event->mousePosRel);

    if(event->lastPollTimestamp > 0)
    {
        float speed = m_speed;

        if(event->keyState[keybind.SPEED])
            speed *= 4;

        if(event->keyState[keybind.FORWARD])
            m_pos += event->lastPollTimestamp * 0.001f * m_target * speed;

        if(event->keyState[keybind.BACKWARD])
            m_pos -= event->lastPollTimestamp * 0.001f * m_target * speed;

        if(event->keyState[keybind.STRAFLEFT])
            m_pos -= event->lastPollTimestamp * 0.001f * m_left * speed;

        if(event->keyState[keybind.STRAFRIGHT])
            m_pos += event->lastPollTimestamp * 0.001f * m_left * speed;

        if(event->keyState[keybind.UP])
            m_pos.y += event->lastPollTimestamp * 0.001f * speed;

        if(event->keyState[keybind.DOWN])
            m_pos.y -= event->lastPollTimestamp * 0.001f * speed;

    }
}

void FreeFlyCamera::setSpeed(float speed)
{
    this->m_speed = speed;
}

float FreeFlyCamera::getSpeed() const
{
    return m_speed;
}
