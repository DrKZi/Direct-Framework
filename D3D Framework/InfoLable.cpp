#include "stdafx.h"
#include "InfoLable.h"


InfoLable::InfoLable()
{
	m_vertexBuffer = nullptr;
	m_indexBuffer = nullptr;
	photo = nullptr;
	m_rt = new RenderTarget;
}

bool InfoLable::Init(ID3D11Device* device, Shader *shader, int screenWidth, int screenHeight, int bitmapWidth, int bitmapHeight)
{
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;
	m_bitmapWidth = bitmapWidth;
	m_bitmapHeight = bitmapHeight;
	m_previousPosX = -1;
	m_previousPosY = -1;

	Vertex vertices[4];

	signed int centreW = m_screenWidth / 2 * -1;
	signed int centreH = m_screenHeight / 2;
	float left = (float)centreW;
	float right = left + m_bitmapWidth;
	float top = (float)centreH;
	float bottom = top - m_bitmapHeight;

	vertices[0].pos = XMFLOAT3(left, top, 0.0f);
	vertices[0].tex = XMFLOAT2(0.0f, 0.0f);

	vertices[1].pos = XMFLOAT3(right, bottom, 0.0f);
	vertices[1].tex = XMFLOAT2(1.0f, 1.0f);

	vertices[2].pos = XMFLOAT3(left, bottom, 0.0f);
	vertices[2].tex = XMFLOAT2(0.0f, 1.0f);

	vertices[3].pos = XMFLOAT3(right, top, 0.0f);
	vertices[3].tex = XMFLOAT2(1.0f, 0.0f);

	unsigned long indices[6] =
	{
		0,1,2,
		0,3,1
	};

	m_vertexBuffer = Buffer::CreateVertexBuffer(device, sizeof(Vertex) * 4, false, &vertices);
	if (!m_vertexBuffer)
		return false;

	m_indexBuffer = Buffer::CreateIndexBuffer(device, sizeof(unsigned long) * 6, false, &indices);
	if (!m_indexBuffer)
		return false;

	m_shader = shader;

	if (!m_initShder())
		return false;

	return true;
}

void InfoLable::Close()
{
	_RELEASE(m_indexBuffer);
	_RELEASE(m_vertexBuffer);
	_CLOSE(photo);
	for (int i = 0; i < txt.size(); i++)
		_CLOSE(txt[i]);
	_CLOSE(m_rt);
}

void InfoLable::Render(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride = sizeof(Vertex);
	unsigned int offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->DrawIndexed(6, 0, 0);
}

void InfoLable::Clear()
{
	m_shader->RemoveTexture();
	_CLOSE(m_rt);
	m_rt = new RenderTarget;
	_CLOSE(photo);
	m_photoFile.clear();
	for (int i = 0; i < txt.size(); i++)
	{
		_CLOSE(txt[i]);
		txt.pop_back();
	}
}

RenderTarget * InfoLable::GetRT()
{
	return m_rt;
}

Image * InfoLable::GetPhoto()
{
	return photo;
}

std::vector<Text*> InfoLable::GetText()
{
	return txt;
}

float InfoLable::GetImgWidth()
{
	return photo->GetBitmapWidth();
}

float InfoLable::GetImgHeight()
{
	return photo->GetBitmapHeight();
}

bool InfoLable::isPhoto()
{
	if (!photo)
		return false;
	else
		return true;
}

bool InfoLable::equilPhoto(std::wstring fileName)
{
	if (fileName == m_photoFile)
		return true;
	else
		return false;
}

void InfoLable::SetPhoto(std::wstring file, Image * imageObj, float height)
{
	imageObj->Init(file.data(), height);
	photo = imageObj;
	m_photoFile = file;
}

void InfoLable::SetText(std::vector<Text*> text)
{
	txt = text;
}

bool InfoLable::m_initShder()
{
	m_shader->AddInputElementDesc("POSITION", DXGI_FORMAT_R32G32B32_FLOAT);
	m_shader->AddInputElementDesc("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT);

	if (!m_shader->CreateShader(L"image.vs", L"imageInfo.ps"))
		return false;

	if (!m_shader->AddTexture(L"texture1.png"))
		return false;

	return true;
}
