#include "Camera.h"

#include <GL/glu.h>

using namespace std;
using namespace tbe;
using namespace tbe::scene;

Camera::Camera(CameraType type, Vector3f posistion, Vector3f target, float sensivity)
{
    m_pos = posistion;
    m_target = target;
    m_left = target.Rotate(90, 0);
    m_up = Vector3f(0, 1, 0);

    m_sensivity = sensivity;

    m_phi = 0;
    m_theta = 0;

    m_cameraType = type;
}

Camera::~Camera()
{

}

void Camera::Push()
{
    m_stackVector.push(m_pos);
    m_stackVector.push(m_target);
    m_stackVector.push(m_left);
    m_stackVector.push(m_up);

    m_stackFloat.push(m_theta);
    m_stackFloat.push(m_phi);
    m_stackFloat.push(m_sensivity);
}

void Camera::Pop()
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

void Camera::Engine()
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

void Camera::SetRotate(Vector2f rel)
{
    m_theta = rel.x;
    m_phi = rel.y;

    if(m_phi > 89)
        m_phi = 89;
    else if(m_phi < -89)
        m_phi = -89;

    m_target.Normalize();
    m_target.Rotate(m_theta, m_phi);

    m_left.Normalize();
    m_left.Rotate(m_theta - 90, m_phi);

    m_up.Normalize();
    m_up.Rotate(m_theta, m_phi + 90);
}

void Camera::SetRelRotate(Vector2f rel)
{
    m_theta -= rel.x*m_sensivity;
    m_phi += rel.y*m_sensivity;

    if(m_phi > 89)
        m_phi = 89;
    else if(m_phi < -89)
        m_phi = -89;

    m_target.Normalize();
    m_target.Rotate(m_theta, m_phi);

    m_left.Normalize();
    m_left.Rotate(m_theta - 90, 0);

    m_up.Normalize();
    m_up.Rotate(0, m_phi + 90);
}

void Camera::SetSensivity(float value)
{
    m_sensivity = value;
}

float Camera::GetSensivity() const
{
    return m_sensivity;
}

void Camera::SetUp(Vector3f up)
{
    this->m_up = up;
}

Vector3f Camera::GetUp() const
{
    return m_up;
}

void Camera::SetLeft(Vector3f left)
{
    this->m_left = left;
}

Vector3f Camera::GetLeft() const
{
    return m_left;
}

void Camera::SetTarget(Vector3f target)
{
    this->m_target = target;
}

Vector3f Camera::GetTarget() const
{
    return m_target;
}

void Camera::SetPos(Vector3f pos)
{
    this->m_pos = pos;
}

Vector3f Camera::GetPos() const
{
    return m_pos;
}

void Camera::SetCameraType(CameraType cameraType)
{
    this->m_cameraType = cameraType;
}

Camera::CameraType Camera::GetCameraType() const
{
    return m_cameraType;
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

void OrbitalCamera::OnEvent(EventManager* event)
{
    if(event->notify == EventManager::EVENT_MOUSE_MOVE)
        SetRelRotate(event->mousePosRel);

    float speed = 0.1;

    if(event->mouseState[keybind.SPEED])
        speed *= 4;

    if(event->mouseState[keybind.FORWARD] && m_distance > 0)
        m_distance -= m_distance * speed;

    if(event->mouseState[keybind.BACKWARD])
        m_distance += m_distance * speed;

    m_pos = m_center - m_target * m_distance;
}

void OrbitalCamera::SetDistance(float distance)
{
    this->m_distance = distance;
    m_pos = m_center - m_target * m_distance;
}

float OrbitalCamera::GetDistance() const
{
    return m_distance;
}

void OrbitalCamera::SetCenter(Vector3f center)
{
    this->m_center = center;
    m_pos = m_center - m_target * m_distance;
}

Vector3f OrbitalCamera::GetCenter() const
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

void FreeFlyCamera::OnEvent(EventManager* event)
{
    if(event->notify == EventManager::EVENT_MOUSE_MOVE)
        SetRelRotate(event->mousePosRel);

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

void FreeFlyCamera::SetSpeed(float speed)
{
    this->m_speed = speed;
}

float FreeFlyCamera::GetSpeed() const
{
    return m_speed;
}
