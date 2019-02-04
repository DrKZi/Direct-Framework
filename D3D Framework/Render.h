#pragma once

#include "RenderState.h"

namespace D3D11Framework
{
    //------------------------------------------------------------------

    class Render
    {
        friend class StaticMesh;
        friend class Image;
        friend class BitmapFont;
        friend class Text;
        friend class Shader;
    public:
        Render();
        virtual ~Render();

        bool CreateDevice(HWND hwnd);
        void BeginFrame();
        void EndFrame();
        void Shutdown();

        virtual bool Init() = 0;
        virtual bool Draw() = 0;
        virtual void Close() = 0;

        void TurnZBufferOn();
        void TurnZBufferOff();

        void TurnOnAlphaBlending();
        void TurnOffAlphaBlending();

        void* operator new(size_t i)
        {
            return _aligned_malloc(i, 16);
        }

        void operator delete(void* p)
        {
            _aligned_free(p);
        }

    protected:
        bool m_createdevice();
        bool m_createdepthstencil();
        void m_initmatrix();
        void m_resize();

        RenderState *m_renderstate;

        ID3D11Device            *m_pd3dDevice;
        ID3D11DeviceContext     *m_pImmediateContext;
        IDXGISwapChain          *m_pSwapChain;
        ID3D11RenderTargetView  *m_pRenderTargetView;
        ID3D11DepthStencilView  *m_pDepthStencilView;

        XMMATRIX m_Ortho;
        XMMATRIX m_Projection;
        HWND m_hwnd;
        unsigned int m_width;
        unsigned int m_height;
    };

    //------------------------------------------------------------------
}

