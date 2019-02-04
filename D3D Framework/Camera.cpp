#include "stdafx.h"
#include "Camera.h"

using namespace D3D11Framework;

Camera::Camera()
{
    m_frameTime = 0.0f;
    m_leftTurnSpeed = 0.0f;
    m_rightTurnSpeed = 0.0f;

    m_pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
    m_rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

void Camera::Render(float time)
{
    m_frameTime = time;

    float radians = m_rot.y * 0.0174532925f;
    float lAtx = sinf(radians) + m_pos.x;
    float lAty = m_pos.y;
    float lAtz = cosf(radians) + m_pos.z;

    XMVECTOR camPos = XMVectorSet(m_pos.x, m_pos.y, m_pos.z, 0.0f);
    XMVECTOR camLookAt = XMVectorSet(lAtx, lAty, lAtz, 0.0f);
    XMVECTOR camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    m_viewMatrix = XMMatrixLookAtLH(camPos, camLookAt, camUp);
}

void Camera::TurnLeft(bool keydown)
{
    if (keydown)
    {
        m_leftTurnSpeed += m_frameTime * 0.01f;

        if (m_leftTurnSpeed > (m_frameTime * 0.15f))
            m_leftTurnSpeed = m_frameTime * 0.15f;
    }
    else
    {
        m_leftTurnSpeed -= m_frameTime * 0.005f;

        if (m_leftTurnSpeed < 0.0f)
            m_leftTurnSpeed = 0.0f;
    }

    m_rot.y -= m_leftTurnSpeed;
    if (m_rot.y < 0.0f)
        m_rot.y += 360.0f;
}

void Camera::TurnRight(bool keydown)
{
    if (keydown)
    {
        m_rightTurnSpeed += m_frameTime * 0.01f;

        if (m_rightTurnSpeed > (m_frameTime * 0.15f))
            m_rightTurnSpeed = m_frameTime * 0.15f;
    }
    else
    {
        m_rightTurnSpeed -= m_frameTime * 0.005f;

        if (m_rightTurnSpeed < 0.0f)
            m_rightTurnSpeed = 0.0f;
    }

    m_rot.y += m_rightTurnSpeed;
    if (m_rot.y > 360.0f)
        m_rot.y -= 360.0f;
}

void Camera::SetPosition(float x, float y, float z)
{
    m_pos.x = x;
    m_pos.y = y;
    m_pos.z = z;
}

void Camera::SetRotation(float x, float y, float z)
{
    m_rot.x = x;
    m_rot.y = y;
    m_rot.z = z;
}

CXMMATRIX Camera::GetViewMatrix()
{
    return m_viewMatrix;
}