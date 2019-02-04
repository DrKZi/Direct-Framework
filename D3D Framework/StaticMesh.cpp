#include "stdafx.h"
#include "StaticMesh.h"
#include "ms3dspec.h"
#include <fstream>
#include "macros.h"
#include "Util.h"
#include "Shader.h"
#include "Buffer.h"

using namespace D3D11Framework;
using namespace std;

struct Vertex
{
    XMFLOAT3 Pos;
    XMFLOAT2 Tex;
};

struct ConstantBuffer
{
    XMMATRIX WVP;
};

StaticMesh::StaticMesh(Render *render)
{
    m_render = render;
    m_vertexBuffer = nullptr;
    m_indexBuffer = nullptr;
    m_constantBuffer = nullptr;
    m_shader = nullptr;
}

bool StaticMesh::Init(wchar_t *name)
{
    Identity();

    m_shader = new Shader(m_render);
    if (!m_shader)
        return false;

    m_shader->AddInputElementDesc("POSITION", DXGI_FORMAT_R32G32B32_FLOAT);
    m_shader->AddInputElementDesc("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT);
    if (!m_shader->CreateShader(L"mesh.vs", L"mesh.ps"))
        return false;

    if (!m_loadMS3DFile(name))
        return false;

    return true;
}

bool StaticMesh::m_loadMS3DFile(wchar_t *Filename)
{
    unsigned short VertexCount = 0;
    unsigned short TriangleCount = 0;
    unsigned short GroupCount = 0;
    unsigned short MaterialCount = 0;
    MS3DVertex *pMS3DVertices = nullptr;
    MS3DTriangle *pMS3DTriangles = nullptr;
    MS3DGroup *pMS3DGroups = nullptr;
    MS3DMaterial *pMS3DMaterials = nullptr;
    MS3DHeader header;

    ifstream fin;

    fin.open(Filename, std::ios::binary);
    fin.read((char*)(&(header)), sizeof(header));
    if (header.version != 3 && header.version != 4)
        return false;

    fin.read((char*)(&VertexCount), sizeof(unsigned short));
    pMS3DVertices = new MS3DVertex[VertexCount];
    fin.read((char*)pMS3DVertices, VertexCount * sizeof(MS3DVertex));

    fin.read((char*)(&TriangleCount), sizeof(unsigned short));
    pMS3DTriangles = new MS3DTriangle[TriangleCount];
    fin.read((char*)pMS3DTriangles, TriangleCount * sizeof(MS3DTriangle));

    fin.read((char*)(&GroupCount), sizeof(unsigned short));
    pMS3DGroups = new MS3DGroup[GroupCount];
    for (int i = 0; i < GroupCount; i++)
    {
        fin.read((char*)&(pMS3DGroups[i].flags), sizeof(unsigned char));
        fin.read((char*)&(pMS3DGroups[i].name), sizeof(char[32]));
        fin.read((char*)&(pMS3DGroups[i].numtriangles), sizeof(unsigned short));
        unsigned short triCount = pMS3DGroups[i].numtriangles;
        pMS3DGroups[i].triangleIndices = new unsigned short[triCount];
        fin.read((char*)(pMS3DGroups[i].triangleIndices), sizeof(unsigned short) * triCount);
        fin.read((char*)&(pMS3DGroups[i].materialIndex), sizeof(char));
    }

    fin.read((char*)(&MaterialCount), sizeof(unsigned short));
    pMS3DMaterials = new MS3DMaterial[MaterialCount];
    fin.read((char*)pMS3DMaterials, MaterialCount * sizeof(MS3DMaterial));

    fin.close();

    m_indexCount = TriangleCount * 3;
    unsigned short *indices = new unsigned short[m_indexCount];
    if (!indices)
        return false;
    Vertex *vertices = new Vertex[VertexCount];
    if (!vertices)
        return false;

    for (int i = 0; i < TriangleCount; i++)
    {
        indices[3 * i + 0] = pMS3DTriangles[i].vertexIndices[0];
        indices[3 * i + 1] = pMS3DTriangles[i].vertexIndices[1];
        indices[3 * i + 2] = pMS3DTriangles[i].vertexIndices[2];
    }

    for (int i = 0; i < VertexCount; i++)
    {
        vertices[i].Pos.x = pMS3DVertices[i].vertex[0];
        vertices[i].Pos.y = pMS3DVertices[i].vertex[1];
        vertices[i].Pos.z = pMS3DVertices[i].vertex[2];

        for (int j = 0; j < TriangleCount; j++)
        {
            if (i == pMS3DTriangles[j].vertexIndices[0])
            {
                vertices[i].Tex.x = pMS3DTriangles[j].s[0];
                vertices[i].Tex.y = pMS3DTriangles[j].t[0];
            }
            else if (i == pMS3DTriangles[j].vertexIndices[1])
            {
                vertices[i].Tex.x = pMS3DTriangles[j].s[1];
                vertices[i].Tex.y = pMS3DTriangles[j].t[1];
            }
            else if (i == pMS3DTriangles[j].vertexIndices[2])
            {
                vertices[i].Tex.x = pMS3DTriangles[j].s[2];
                vertices[i].Tex.y = pMS3DTriangles[j].t[2];
            }
            else
                continue;
            break;
        }
    }

    wchar_t *name = CharToWChar(pMS3DMaterials[0].texture);
    if (!m_shader->AddTexture(name))
        return false;
    _DELETE_ARRAY(name);

    _DELETE_ARRAY(pMS3DMaterials);
    if (pMS3DGroups != nullptr)
    {
        for (int i = 0; i < GroupCount; i++)
            _DELETE_ARRAY(pMS3DGroups[i].triangleIndices);
        _DELETE_ARRAY(pMS3DGroups);
    }
    _DELETE_ARRAY(pMS3DTriangles);
    _DELETE_ARRAY(pMS3DVertices);

    m_vertexBuffer = Buffer::CreateVertexBuffer(m_render->m_pd3dDevice, sizeof(Vertex)*VertexCount, false, vertices);
    if (!m_vertexBuffer)
        return false;

    m_indexBuffer = Buffer::CreateIndexBuffer(m_render->m_pd3dDevice, sizeof(unsigned short)*m_indexCount, false, indices);
    if (!m_indexBuffer)
        return false;

    m_constantBuffer = Buffer::CreateConstantBuffer(m_render->m_pd3dDevice, sizeof(ConstantBuffer), false);
    if (!m_constantBuffer)
        return false;

    _DELETE_ARRAY(vertices);
    _DELETE_ARRAY(indices);

    return true;
}

void StaticMesh::Draw(CXMMATRIX viewmatrix)
{
    m_RenderBuffers();
    m_SetShaderParameters(viewmatrix);
    m_RenderShader();
}

void StaticMesh::m_RenderBuffers()
{
    unsigned int stride = sizeof(Vertex);
    unsigned int offset = 0;
    m_render->m_pImmediateContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
    m_render->m_pImmediateContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R16_UINT, 0);
    //m_render->m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void StaticMesh::m_SetShaderParameters(CXMMATRIX viewmatrix)
{
    XMMATRIX WVP = m_objMatrix * viewmatrix * m_render->m_Projection;
    ConstantBuffer cb;
    cb.WVP = XMMatrixTranspose(WVP);
    m_render->m_pImmediateContext->UpdateSubresource(m_constantBuffer, 0, NULL, &cb, 0, 0);

    m_render->m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_constantBuffer);
}

void StaticMesh::m_RenderShader()
{
    m_shader->Draw();
    m_render->m_pImmediateContext->DrawIndexed(m_indexCount, 0, 0);
}

void StaticMesh::Close()
{
    _RELEASE(m_indexBuffer);
    _RELEASE(m_vertexBuffer);
    _RELEASE(m_constantBuffer);
    _CLOSE(m_shader);
}

void StaticMesh::Translate(float x, float y, float z)
{
    m_objMatrix *= XMMatrixTranslation(x, y, z);
}
void StaticMesh::Rotate(float angle, float x, float y, float z)
{
    XMVECTOR v = XMVectorSet(x, y, z, 0.0f);
    m_objMatrix *= XMMatrixRotationAxis(v, angle);
}
void StaticMesh::Scale(float x, float y, float z)
{
    m_objMatrix *= XMMatrixScaling(x, y, z);
}

void StaticMesh::Identity()
{
    m_objMatrix = XMMatrixIdentity();
}