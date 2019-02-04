#include "stdafx.h"
#include "RenderState.h"
#include "macros.h"

using namespace D3D11Framework;

RenderState::RenderState(ID3D11Device *pd3dDevice, ID3D11DeviceContext *pImmediateContext)
{
    m_pd3dDevice = pd3dDevice;
    m_pImmediateContext = pImmediateContext;

    m_pDepthEnableStencilState = nullptr;
    m_pDepthDisabledStencilState = nullptr;
    m_pAlphaEnableBlendingState = nullptr;
    m_pAlphaDisableBlendingState = nullptr;
    m_pSampleState = nullptr;

    m_depthenable = false;
    m_alphaenable = false;
}

bool RenderState::Init()
{
    if (!m_createdepthstencilstate())
        return false;
    if (!m_createblendingstate())
        return false;
    if (!m_createsamplerstate())
        return false;

    TurnZBufferOn();
    TurnOnAlphaBlending();

    m_pImmediateContext->PSSetSamplers(0, 1, &m_pSampleState);
    m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    return true;
}

bool RenderState::m_createdepthstencilstate()
{
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthStencilDesc.StencilEnable = true;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;
    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    HRESULT hr = m_pd3dDevice->CreateDepthStencilState(&depthStencilDesc, &m_pDepthEnableStencilState);
    if (FAILED(hr))
        return false;

    depthStencilDesc.DepthEnable = false;
    hr = m_pd3dDevice->CreateDepthStencilState(&depthStencilDesc, &m_pDepthDisabledStencilState);
    if (FAILED(hr))
        return false;

    return true;
}

bool RenderState::m_createblendingstate()
{
    D3D11_BLEND_DESC blendStateDescription;
    ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));
    blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
    blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;
    if (FAILED(m_pd3dDevice->CreateBlendState(&blendStateDescription, &m_pAlphaEnableBlendingState)))
        return false;

    blendStateDescription.RenderTarget[0].BlendEnable = FALSE;
    if (FAILED(m_pd3dDevice->CreateBlendState(&blendStateDescription, &m_pAlphaDisableBlendingState)))
        return false;

    return true;
}

bool RenderState::m_createsamplerstate()
{
    D3D11_SAMPLER_DESC samplerDesc;
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.BorderColor[0] = 0;
    samplerDesc.BorderColor[1] = 0;
    samplerDesc.BorderColor[2] = 0;
    samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    if (FAILED(m_pd3dDevice->CreateSamplerState(&samplerDesc, &m_pSampleState)))
        return false;

    return true;
}

void RenderState::Close()
{
    _RELEASE(m_pAlphaEnableBlendingState);
    _RELEASE(m_pAlphaDisableBlendingState);
    _RELEASE(m_pDepthEnableStencilState);
    _RELEASE(m_pDepthDisabledStencilState);
    _RELEASE(m_pSampleState);
}

void RenderState::TurnZBufferOn()
{
    if (!m_depthenable)
    {
        m_pImmediateContext->OMSetDepthStencilState(m_pDepthEnableStencilState, 1);
        m_depthenable = true;
    }
}

void RenderState::TurnZBufferOff()
{
    if (m_depthenable)
    {
        m_pImmediateContext->OMSetDepthStencilState(m_pDepthDisabledStencilState, 1);
        m_depthenable = false;
    }
}

void RenderState::TurnOnAlphaBlending()
{
    if (!m_alphaenable)
    {
        float blendFactor[4] = { 0.0f };
        m_pImmediateContext->OMSetBlendState(m_pAlphaEnableBlendingState, blendFactor, 0xffffffff);
        m_alphaenable = true;
    }

}

void RenderState::TurnOffAlphaBlending()
{
    if (m_alphaenable)
    {
        float blendFactor[4] = { 0.0f };
        m_pImmediateContext->OMSetBlendState(m_pAlphaDisableBlendingState, blendFactor, 0xffffffff);
        m_alphaenable = false;
    }
}