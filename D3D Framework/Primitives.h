#pragma once

#include "InfoLable.h"

struct Vertex
{
	XMFLOAT3 pos;
	XMFLOAT3 tex;
	XMFLOAT3 norm;
};

class Primitives
{
public:
	Primitives();
	Primitives(int id);

	bool Init(Shader *shader, ID3D11Device *pd3dDevice);

	ID3D11Buffer* GetVertexBuffer();
	ID3D11Buffer* GetIndexBuffer();

	int GetID();

	XMFLOAT4 GetPosition();

	unsigned GetNrOfIndices();
	unsigned GetNrOfVerteces();

	virtual void Close();

protected:
	virtual bool m_initBuffers() = 0;
	virtual bool m_initLayout() = 0;

	Shader* m_shader;
	ID3D11Device *m_pd3dDevice;

	int id;

	ID3D11Buffer *m_VBtext;
	ID3D11Buffer *m_IBtext;

	unsigned nrOfIndices;
	unsigned nrOfVerteces;

	XMFLOAT4 m_position;
};

class Label : public Primitives
{
public:
	Label(XMFLOAT3 center);
	Label(int id, XMFLOAT3 center);

	void AddText(std::wstring text, Text *txtObject);
	void AddImg(const wchar_t* file, Image *imageObject);

	std::vector<Text *> GetTextes();
	std::vector<Image *> GetImgs();
	RenderTarget* GetRT();
	long GetRTwidth();
	float GetImgWidth();

	void Close();

private:
	bool m_initBuffers();
	bool m_initLayout();

	XMFLOAT3 m_center;

	std::vector<Text *> textes;
	std::vector<Image *> imgs;

	long m_RTwidth;
	RenderTarget *m_rt;
};

class Pipe : public Primitives
{
public:
	Pipe(XMFLOAT3 parent, XMFLOAT3 son);
	Pipe(int parent, int son);

	void Close();

private:
	bool m_initBuffers();
	bool m_initLayout();

	XMFLOAT3 m_parent, m_son;
};