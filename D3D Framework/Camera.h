#pragma once

namespace D3D11Framework
{
    //------------------------------------------------------------------

    class Camera
    {
    public:
        Camera();

        void Render(float time);

        void TurnLeft(bool keydown);
        void TurnRight(bool keydown);

        void SetPosition(float x, float y, float z);
        void SetRotation(float x, float y, float z);

        CXMMATRIX GetViewMatrix();

    private:
        XMMATRIX m_viewMatrix;
        XMFLOAT3 m_pos;
        XMFLOAT3 m_rot;

        float m_frameTime;
        float m_leftTurnSpeed, m_rightTurnSpeed;
    };

    //------------------------------------------------------------------
}