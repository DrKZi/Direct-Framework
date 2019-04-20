#pragma once

#include "Primitives.h"

struct TD {
	std::wstring name;
	std::wstring surname;
	std::wstring ffn;
	std::wstring shortname;
	std::wstring cathedra;
	std::wstring titul;
	std::wstring scinDegr;
	std::wstring additionInf;
};

class RenderTree : public Render
{
public:
	RenderTree();

	bool Init();
	void AddConnector(int parent, int son);
	void AddLabel(int id, XMFLOAT3 center);

	bool Draw();
	void Close();

	void SetTimer(Timer* timer);

	void MouseTrap(float x, float y);

	void MouseMove(float speed, const char axis);

	void AngelFall();

private:
	bool m_initPhotos(int i, std::wstring fileName);
	void m_initTextes(int i, std::wstring txt1, std::wstring txt2);

	void m_drawLables(int i);
	void m_drawInfoLable();

	void m_renderScene();
	XMMATRIX m_update();

	int m_labelSearch(float x, float y);
	std::vector<Text*> m_prepareInfoForLable(int id);

	bool m_LoadFromFile();

	void tempDataFilling();

	ID3D11Buffer *constLightBuffer;
	ID3D11Buffer *constMatrixBuffer;

	BitmapFont* m_font;

	std::vector<Label *> m_lables;
	std::vector<Shader *> m_shaders;

	std::vector<Pipe *> m_pipes;
	std::vector<Shader *> m_pipeShaders;

	Shader* m_infoShader;
	InfoLable* m_infoLable;
	ID3D11Buffer *m_constantBuffer;
	bool m_lockInfo1 = false, m_lockInfo2 = false;

	Timer* m_timer;

	float m_key;
	XMFLOAT3 m_camKey;
	bool m_lock, m_freedom;
	float m_radian;

	XMMATRIX WVP;

	Camera *m_cam;
	XMFLOAT3 m_camPos;

	bool m_notClosed;

	float m_infoHeight;
	float m_infoWidth;

	std::vector<TD*> td;
};

