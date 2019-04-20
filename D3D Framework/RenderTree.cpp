#pragma once
#include "stdafx.h"
#include "RenderTree.h"


// буфер констант
struct cbMatrixData
{
	XMMATRIX WVP;
	XMMATRIX world;
};

struct cbLightData
{
	XMFLOAT4 vLightDir;
	XMFLOAT4 vLightColor;
};

// буфер для InfoLable
struct ConstantBuffer
{
	XMMATRIX WVP;
};

//-----------------------------------------------------------------------

RenderTree::RenderTree()
{
	constMatrixBuffer = nullptr;
	constLightBuffer = nullptr;
	m_infoLable = nullptr;
	m_infoShader = nullptr;
	m_constantBuffer = nullptr;
	m_key = -15.6f;
	m_radian = 0;
	m_lock = false;
	m_cam = new Camera;
	m_notClosed = true;
	m_freedom = true;
}

bool RenderTree::Init()
{
	tempDataFilling();
	//m_pSwapChain->SetFullscreenState(false, nullptr);
	// Определение табличек
	// Центры после третьего этапа сортировки(формации дерева)
	AddLabel(1, XMFLOAT3(0.f, 0.75f, 0.f));
	AddLabel(2, XMFLOAT3(0.f, 0.f, 0.f));
	AddLabel(3, XMFLOAT3(0.f, -0.75f, 0.f));
	AddLabel(4, XMFLOAT3(-1.5f, -1.5f, 0.f));
	AddLabel(5, XMFLOAT3(1.5f, -1.5f, 0.f));
	AddLabel(6, XMFLOAT3(-0.6f, -2.25f, 0.f));
	AddLabel(7, XMFLOAT3(-1.5f, -2.25f, 0.f));
	AddLabel(8, XMFLOAT3(-2.4f, -2.25f, 0.f));
	AddLabel(9, XMFLOAT3(0.6f, -2.25f, 0.f));
	AddLabel(10, XMFLOAT3(1.5f, -2.25f, 0.f));
	AddLabel(11, XMFLOAT3(2.4f, -2.25f, 0.f));


	// Определение соединений
	// Переделать конструктор класса на ID!!!!!!!!
	m_pipes.push_back(new Pipe(XMFLOAT3(0.f, 0.75f, 0.f), XMFLOAT3(0.f, 0.f, 0.f)));
	m_pipes.push_back(new Pipe(XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(0.f, -0.75f, 0.f)));
	m_pipes.push_back(new Pipe(XMFLOAT3(0.f, -0.75f, 0.f), XMFLOAT3(-1.5f, -1.5f, 0.f)));
	m_pipes.push_back(new Pipe(XMFLOAT3(0.f, -0.75f, 0.f), XMFLOAT3(1.5f, -1.5f, 0.f)));
	m_pipes.push_back(new Pipe(XMFLOAT3(-1.5f, -1.5f, 0.f), XMFLOAT3(-0.6f, -2.25f, 0.f)));
	m_pipes.push_back(new Pipe(XMFLOAT3(-1.5f, -1.5f, 0.f), XMFLOAT3(-1.5f, -2.25f, 0.f)));
	m_pipes.push_back(new Pipe(XMFLOAT3(-1.5f, -1.5f, 0.f), XMFLOAT3(-2.4f, -2.25f, 0.f)));
	m_pipes.push_back(new Pipe(XMFLOAT3(1.5f, -1.5f, 0.f), XMFLOAT3(0.6f, -2.25f, 0.f)));
	m_pipes.push_back(new Pipe(XMFLOAT3(1.5f, -1.5f, 0.f), XMFLOAT3(1.5f, -2.25f, 0.f)));
	m_pipes.push_back(new Pipe(XMFLOAT3(1.5f, -1.5f, 0.f), XMFLOAT3(2.4f, -2.25f, 0.f)));

	// Константные буферы
	constMatrixBuffer = Buffer::CreateConstantBuffer(m_pd3dDevice, sizeof(cbMatrixData), false);
	constLightBuffer = Buffer::CreateConstantBuffer(m_pd3dDevice, sizeof(cbLightData), false);
	m_constantBuffer = Buffer::CreateConstantBuffer(m_pd3dDevice, sizeof(ConstantBuffer), false);

	//Information lable
	m_infoLable = new InfoLable;

	m_infoHeight = m_height / 3.f;
	m_infoWidth = float(m_width) / float(m_height) * m_infoHeight;

	m_infoShader = new Shader(this);
	if (!m_infoLable->Init(m_pd3dDevice, m_infoShader, m_width, m_height, m_infoWidth, m_infoHeight))
		return false;

	// Шедеры
	for (int i = 0; i < m_lables.size(); i++)
	{
		m_shaders.push_back(new Shader(this));
		if (!m_shaders[i])
			return false;
	}

	for (int i = 0; i < m_pipes.size(); i++)
	{
		m_pipeShaders.push_back(new Shader(this));
		if (!m_pipeShaders[i])
			return false;
	}

	// Инициализируем все таблички
	for (int i = 0; i < m_lables.size(); i++)
		if (!m_lables[i]->Init(m_shaders[i], m_pd3dDevice))
			return false;

	// Инициализируем все соединения
	for (int i = 0; i < m_pipes.size(); i++)
		if (!m_pipes[i]->Init(m_pipeShaders[i], m_pd3dDevice))
			return false;

	// настраиваем камеру
	m_camPos = XMFLOAT3(0.f, -0.75f, -2.f);
	m_cam->Init(m_camPos);

	// Создаём все шрифты
	m_font = new BitmapFont(this);
	if (!m_font->Init("font.fnt"))
		return false;

	// Текст для каждой таблички
	// Вызывать по id
	for (int i = 0; i < m_lables.size(); i++)
	{

		m_initTextes(i, td[i]->shortname, td[i]->additionInf);

		std::wstring adress = L"photo//photo";
		adress.append(std::to_wstring(i + 1));
		adress.append(L".jpg");

		// Картинка для каждой таблички
		// Вызывать по id
		// запросом спрашивать имя
		if (!m_initPhotos(i, adress))
			return false;

		// рендер в текстуру
		m_drawLables(i);
	}

	return true;
}

void RenderTree::AddConnector(int parent, int son)
{
	m_pipes.push_back(new Pipe(parent, son));
}

void RenderTree::AddLabel(int id, XMFLOAT3 center)
{
	m_lables.push_back(new Label(id, center));
}

bool RenderTree::Draw()
{
	float color[4] = { 0.0f, 0.0f, 0.3f, 1.0f };
	m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, color);
	m_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	if (m_timer->GetCurOption() != 0)
	{
		if (!m_timer->Resault())
		{
			m_key = -XM_PI;
			m_lock = true;
			m_freedom = false;
		}
		else if (!m_lockInfo1)
		{
			m_lock = false;
		}
	}

	m_renderScene();

	return m_notClosed;
}

void RenderTree::m_renderScene()
{
	XMMATRIX m_world = m_update();
	m_cam->initMatrix();

	cbLightData cblgh;
	cblgh.vLightDir = XMFLOAT4(m_camPos.x, m_camPos.y + 0.1f, m_camPos.z, 1.f);
	cblgh.vLightColor = XMFLOAT4(1.f, 1.f, 1.f, 1.f);

	XMMATRIX camera = m_cam->GetViewMatrix();
	WVP = m_world * camera * m_Projection;

	cbMatrixData cbmd;
	cbmd.WVP = XMMatrixTranspose(WVP);
	cbmd.world = XMMatrixTranspose(m_world);


	m_pImmediateContext->UpdateSubresource(constLightBuffer, 0, NULL, &cblgh, 0, 0);
	m_pImmediateContext->PSSetConstantBuffers(0, 1, &constLightBuffer);
	m_pImmediateContext->UpdateSubresource(constMatrixBuffer, 0, NULL, &cbmd, 0, 0);
	m_pImmediateContext->VSSetConstantBuffers(0, 1, &constMatrixBuffer);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	// Рисуем таблички
	for (int i = 0; i < m_lables.size(); i++)
	{
		ID3D11Buffer* temp = m_lables[i]->GetVertexBuffer();
		m_pImmediateContext->IASetVertexBuffers(0, 1, &temp, &stride, &offset);
		m_pImmediateContext->IASetIndexBuffer(m_lables[i]->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

		m_shaders[i]->Draw();
		m_pImmediateContext->DrawIndexed(m_lables[i]->GetNrOfIndices(), 0, 0);
	}

	// Рисуем соединения
	for (int i = 0; i < m_pipes.size(); i++)
	{
		ID3D11Buffer* temp = m_pipes[i]->GetVertexBuffer();
		m_pImmediateContext->IASetVertexBuffers(0, 1, &temp, &stride, &offset);
		m_pImmediateContext->IASetIndexBuffer(m_pipes[i]->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

		m_pipeShaders[i]->Draw();
		m_pImmediateContext->DrawIndexed(m_pipes[i]->GetNrOfIndices(), 0, 0);
	}

	if (m_lockInfo1 && m_lockInfo2)
	{
		TurnZBufferOff();
		XMMATRIX objmatrix = XMMatrixTranslation(m_width - m_infoWidth, -(m_height / 3.f + m_infoHeight), 0.0f);
		ConstantBuffer cb;
		cb.WVP = XMMatrixTranspose(objmatrix*m_Ortho);

		m_pImmediateContext->UpdateSubresource(m_constantBuffer, 0, NULL, &cb, 0, 0);
		m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_constantBuffer);

		m_infoShader->Draw();
		m_infoLable->Render(m_pImmediateContext);
		TurnZBufferOn();
	}
}

XMMATRIX RenderTree::m_update()
{

	m_camKey.x = 0.0005f;
	m_camKey.y = 0.0005f;
	m_camKey.z = 0.0005f;

	if (m_lock)
	{
		if (m_camPos.x >= 1.5f)
			m_camKey.x = 0;
		if (m_camPos.y <= -2.f)
			m_camKey.y = 0;
		if (m_camPos.z >= -1.f)
			m_camKey.z = 0;

		m_cam->MoveX(m_camKey.x);
		m_camPos.x += m_camKey.x;
		m_cam->MoveY(-m_camKey.y);
		m_camPos.y -= m_camKey.y;
		m_cam->MoveZ(m_camKey.z);
		m_camPos.z += m_camKey.z;
	}
	else
	{
		m_freedom = true;
		if (m_camPos.x <= 0.f)
			m_camKey.x = 0;
		else
			m_freedom &= 0;

		if (m_camPos.y >= -0.75f)
			m_camKey.y = 0;
		else
			m_freedom &= 0;

		if (m_camPos.z <= -2.f)
			m_camKey.z = 0;
		else
			m_freedom &= 0;

		if (m_freedom)
		{
			m_key = XM_PI / 4.18879f;
			m_lockInfo2 = false;
		}

		m_cam->MoveX(-m_camKey.x);
		m_camPos.x -= m_camKey.x;
		m_cam->MoveY(m_camKey.y);
		m_camPos.y += m_camKey.y;
		m_cam->MoveZ(-m_camKey.z);
		m_camPos.z -= m_camKey.z;
	}

	if (m_key == -15.6f)
		m_key = XM_PI / 4.18879f;

	if (m_lock && static_cast<int>(m_radian * 57.296) % 360 == 0)
	{
		m_key = 0;
		m_lockInfo2 = true;
	}
	m_radian += m_key / 2500.f;

	XMMATRIX m_world = XMMatrixRotationY(m_radian);

	return m_world;
}

int RenderTree::m_labelSearch(float x, float y)
{
	XMFLOAT4 temp;
	for (int i = 0; i < m_lables.size(); i++)
	{
		temp = m_lables[i]->GetPosition();
		double FOV = static_cast<float>(m_width) / static_cast<float>(m_height);
		temp.x = temp.x / FOV + m_camPos.x / 2;
		temp.z = temp.z / FOV + m_camPos.x / 2;
		if (x >= temp.x && y <= temp.y && x <= temp.z && y >= temp.w)
		{
			//printf("Find lable in range:\nx = %f;\ty = %f;\tz = %f\tw = %f\n", temp.x, temp.y, temp.z, temp.w);
			return m_lables[i]->GetID();
		}
	}
	return 0;
}

std::vector<Text*> RenderTree::m_prepareInfoForLable(int id)
{
	//test Obj!--------------------------------------------
	std::vector<Text *> test;
	Text * txt1 = new Text(this, m_font);
	txt1->Init(td[id - 1]->name);
	Text * txt2 = new Text(this, m_font);
	txt2->Init(td[id - 1]->surname);
	Text * txt3 = new Text(this, m_font);
	txt3->Init(td[id - 1]->ffn);
	Text * txt4 = new Text(this, m_font);
	txt4->Init(td[id - 1]->cathedra);
	Text * txt5 = new Text(this, m_font);
	txt5->Init(td[id - 1]->scinDegr);
	Text * txt6 = new Text(this, m_font);
	txt6->Init(td[id - 1]->titul);
	test.push_back(txt1);
	test.push_back(txt2);
	test.push_back(txt3);
	test.push_back(txt4);
	test.push_back(txt5);
	test.push_back(txt6);
	//------------------------------------------------------
	return test;
}

bool RenderTree::m_LoadFromFile()
{
	std::wifstream input("data.txt");

	// Создаём все шрифты
	m_font = new BitmapFont(this);
	if (!m_font->Init("font.fnt"))
		return false;

	// настраиваем камеру
	m_camPos = XMFLOAT3(0.f, 0.5f, -1.f);
	m_cam->Init(m_camPos);

	bool type;
	XMFLOAT3 center;
	int id, i = 0;

	while (input.eof())
	{
		input >> type;//1 - lable, 0 - pipe
		if (type)
		{
			input >> id;//lable id
			input >> center.x >> center.y >> center.z;//position
			AddLabel(id, center);

			// Шедеры
			m_shaders.push_back(new Shader(this));
			if (!m_shaders[i])
				return false;

			// Инициализируем все таблички
			if (!m_lables[i]->Init(m_shaders[i], m_pd3dDevice))
				return false;

			std::wstring txt;
			input >> txt;//photo filename
			m_initPhotos(i, txt);
			input >> type;//is there second text
			if (type)
			{
				std::wstring txt2;
				input >> txt >> txt2;
				m_initTextes(i, txt, txt2);
			}
			else
			{
				input >> txt;
				m_initTextes(i, txt, L"");
			}

			m_drawLables(i);

			i++;
		}
		else
		{
			int id_to;
			input >> id >> id_to;
			AddConnector(id, id_to);

			m_pipeShaders.push_back(new Shader(this));
			if (!m_pipeShaders[i])
				return false;

			if (!m_pipes[i]->Init(m_pipeShaders[i], m_pd3dDevice))
				return false;
		}
	}

	// Константные буферы
	constMatrixBuffer = Buffer::CreateConstantBuffer(m_pd3dDevice, sizeof(cbMatrixData), false);
	constLightBuffer = Buffer::CreateConstantBuffer(m_pd3dDevice, sizeof(cbLightData), false);

	input.close();
}

void RenderTree::tempDataFilling()
{
	TD *t = new TD;
	t->name = L"Федоров ";
	t->surname = L"Михайло ";
	t->ffn = L"Михайлович";
	t->shortname = L"Федеров М.М.";
	t->additionInf = L"д.т.н.";
	t->cathedra = L"";
	t->scinDegr = L"доктор";
	t->titul = L"профессор";
	td.push_back(t);
	//2
	t = new TD;
	t->name = L"Iванченко ";
	t->surname = L"Eвтихiй ";
	t->ffn = L"Якович";
	t->shortname = L"Iванченко E.Я.";
	t->additionInf = L"д.т.н.";
	t->cathedra = L"ГiрЕл";
	t->scinDegr = L"доктор";
	t->titul = L"профессор";
	td.push_back(t);
	//3
	t = new TD;
	t->name = L"Огороднейчук ";
	t->surname = L"Iван ";
	t->ffn = L"Пилипович";
	t->shortname = L"Огороднейчук I.П.";
	t->additionInf = L"д.т.н.";
	t->cathedra = L"ТЕ";
	t->scinDegr = L"доктор";
	t->titul = L"профессор";
	td.push_back(t);
	//4
	t = new TD;
	t->name = L"Бих ";
	t->surname = L"АнатолIй ";
	t->ffn = L"Iванович";
	t->shortname = L"Бих А.I.";
	t->additionInf = L"д.фiз.-мат.н.";
	t->cathedra = L"БМI";
	t->scinDegr = L"доктор";
	t->titul = L"профессор";
	td.push_back(t);
	//5
	t = new TD;
	t->name = L"Семенець ";
	t->surname = L"Валерiй ";
	t->ffn = L"Васильович";
	t->shortname = L"Семенець В.В.";
	t->additionInf = L"д.т.н.";
	t->cathedra = L"БМI";
	t->scinDegr = L"доктор";
	t->titul = L"профессор";
	td.push_back(t);
	//6
	t = new TD;
	t->name = L"Висоцька ";
	t->surname = L"Олена ";
	t->ffn = L"Володимирiвна";
	t->shortname = L"Висоцька О.В.";
	t->additionInf = L"д.т.н.";
	t->cathedra = L"БМI";
	t->scinDegr = L"доктор";
	t->titul = L"профессор";
	td.push_back(t);
	//7
	t = new TD;
	t->name = L"Свiрь ";
	t->surname = L"Iрина ";
	t->ffn = L"Борисiвна";
	t->shortname = L"Свiрь I.Б.";
	t->additionInf = L"д.т.н.";
	t->cathedra = L"БМI";
	t->scinDegr = L"доктор";
	t->titul = L"доцент";
	td.push_back(t);
	//8
	t = new TD;
	t->name = L"Рожицький ";
	t->surname = L"Миколай ";
	t->ffn = L"Миколайович";
	t->shortname = L"Рожицький М.М.";
	t->additionInf = L"д.т.н.";
	t->cathedra = L"БМI";
	t->scinDegr = L"доктор";
	t->titul = L"профессор";
	td.push_back(t);
	//9
	t = new TD;
	t->name = L"Прасол ";
	t->surname = L"Iгор ";
	t->ffn = L"Вiкторович";
	t->shortname = L"Прасол I.В.";
	t->additionInf = L"д.т.н.";
	t->cathedra = L"БМI";
	t->scinDegr = L"доктор";
	t->titul = L"профессор";
	td.push_back(t);
	//10
	t = new TD;
	t->name = L"Сiнотiн ";
	t->surname = L"АнатолIй  ";
	t->ffn = L"Мєфодiйович";
	t->shortname = L"Сiнотiн А.М.";
	t->additionInf = L"д.т.н.";
	t->cathedra = L"КIТАМ";
	t->scinDegr = L"доктор";
	t->titul = L"профессор";
	td.push_back(t);
	//11
	t = new TD;
	t->name = L"Аврунiн ";
	t->surname = L"Олег ";
	t->ffn = L"Григорович";
	t->shortname = L"Аврунiн О.Г.";
	t->additionInf = L"д.т.н.";
	t->cathedra = L"БМI";
	t->scinDegr = L"доктор";
	t->titul = L"профессор";
	td.push_back(t);
}

void RenderTree::m_drawLables(int i)
{
	float rtKoef = (m_lables[i]->GetRTwidth() * 34.53f + m_lables[i]->GetImgWidth()) / 2;
	m_lables[i]->GetRT()->Init(m_pd3dDevice, rtKoef * 2, rtKoef);
	TurnZBufferOff();
	// Указываем текстуру для рисования (render target)
	m_lables[i]->GetRT()->SetRenderTarget(m_pImmediateContext, m_pDepthStencilView);
	// очищаем ее
	m_lables[i]->GetRT()->ClearRenderTarget(m_pImmediateContext, m_pDepthStencilView, 1.f, 1.f, 1.f, 1.0f);

	// снова рисуем сцену (теперь она рисуется не на экран, а текстуру)
	TurnOnAlphaBlending();
	std::vector<Text *> temp = m_lables[i]->GetTextes();
	float offsetPhoto = m_lables[i]->GetImgWidth() + 2;
	float secondOffset;

	if (m_lables[i]->GetID() == 4)
		secondOffset = 0;
	else
		secondOffset = rtKoef - offsetPhoto;

	for (int i = 0; i < temp.size(); i++)
		temp[i]->Draw(0.f, 0.f, 0.f, offsetPhoto + 2.f + secondOffset * i, 70.f * i + rtKoef / 2 - 60.f);
	TurnOffAlphaBlending();

	std::vector<Image *> temp1 = m_lables[i]->GetImgs();
	for (int i = 0; i < temp1.size(); i++)
		temp1[i]->Draw(0, rtKoef * i + (rtKoef - 300.f) / 2);

	// возвращаем render target view который рисует на экран (в back buffer)
	m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);
	m_shaders[i]->AddTexture(m_lables[i]->GetRT()->GetShaderResourceView());
	TurnZBufferOn();
}

void RenderTree::m_drawInfoLable()
{
	m_infoLable->GetRT()->Init(m_pd3dDevice, m_width, m_height);
	TurnZBufferOff();

	m_infoLable->GetRT()->SetRenderTarget(m_pImmediateContext, m_pDepthStencilView);

	m_infoLable->GetRT()->ClearRenderTarget(m_pImmediateContext, m_pDepthStencilView, 1.f, 1.f, 1.f, 1.f);

	//Text
	TurnOnAlphaBlending();
	std::vector<Text*> temp = m_infoLable->GetText();
	Text * txt = new Text(this, m_font);
	txt->Init(L"ФИО: ", false, 15);
	int i = 0, k = 0;
	float offset, horizontalPhotoOffset, verticalPhotoOffset;
	horizontalPhotoOffset = m_infoLable->GetImgWidth();
	verticalPhotoOffset = m_infoLable->GetImgHeight() - 10.f;

	//FIO
	txt->Draw(0.f, 0.f, 0.f, horizontalPhotoOffset + 4.f, 30.f);
	offset = txt->GetLength() + 30.f;
	for (i; i < 3; i++)
	{
		if (offset + horizontalPhotoOffset + temp[i]->GetLength() + 40.f > m_width)
		{
			k++;
			offset = 3.f;
		}
		temp[i]->Draw(0.f, 0.f, 0.f, (horizontalPhotoOffset + offset), 60.f * k + 30.f);
		offset += temp[i]->GetLength() + 40.f;
	}
	k++;

	//Kafedra
	txt->SetText(L"Кафедра: ");
	txt->Draw(0.f, 0.f, 0.f, horizontalPhotoOffset + 4.f, 60.f * k + 30.f);
	offset = txt->GetLength() + 30.f;
	temp[i++]->Draw(0.f, 0.f, 0.f, (horizontalPhotoOffset + offset), 60.f * k + 30.f);
	k++;

	//scin degree
	float begin = 0;

	txt->SetText(L"Уч.ст.: ");
	if (60.f * k + 30.f >= verticalPhotoOffset) //=
		begin = -horizontalPhotoOffset;
	txt->Draw(0.f, 0.f, 0.f, horizontalPhotoOffset + 4.f + begin, 60.f * k + 30.f);
	offset = txt->GetLength() + 40.f + begin;
	temp[i++]->Draw(0.f, 0.f, 0.f, (horizontalPhotoOffset + offset + begin), 60.f * k + 30.f);
	k++;

	//titul
	txt->SetText(L"Уч.звание: ");
	if (60.f * k + 30.f >= verticalPhotoOffset)
		begin = -horizontalPhotoOffset;
	txt->Draw(0.f, 0.f, 0.f, horizontalPhotoOffset + 4.f + begin, 60.f * k + 30.f);
	offset = txt->GetLength() + 40.f + begin;
	temp[i++]->Draw(0.f, 0.f, 0.f, (horizontalPhotoOffset + offset + begin), 60.f * k + 30.f);
	k++;

	//Candidate
	txt->SetText(L"Кандидатская: ");
	if (60.f * k + 30.f >= verticalPhotoOffset)
		begin = -horizontalPhotoOffset;
	txt->Draw(0.f, 0.f, 0.f, horizontalPhotoOffset + 4.f + begin, 60.f * k + 30.f);
	offset = txt->GetLength() + 40.f + begin;
	//!
	k++;

	//Doctor
	txt->SetText(L"Докторская: ");
	if (60.f * k + 30.f >= verticalPhotoOffset)
		begin = -horizontalPhotoOffset;
	txt->Draw(0.f, 0.f, 0.f, horizontalPhotoOffset + 4.f + begin, 60.f * k + 30.f);
	offset = txt->GetLength() + 40.f + begin;
	//!
	k++;

	TurnOffAlphaBlending();

	m_infoLable->GetPhoto()->Draw(0, 0);

	m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);
	m_infoShader->AddTexture(m_infoLable->GetRT()->GetShaderResourceView());
	TurnZBufferOn();
}

void RenderTree::Close()
{
	_RELEASE(constMatrixBuffer);
	_RELEASE(constLightBuffer);
	for (int i = 0; i < m_lables.size(); i++)
	{
		_CLOSE(m_shaders[i]);
		_CLOSE(m_lables[i]);
	}
	for (int i = 0; i < m_pipes.size(); i++)
	{
		_CLOSE(m_pipeShaders[i]);
		_CLOSE(m_pipes[i]);
	}
	_CLOSE(m_infoShader);
	_CLOSE(m_infoLable);
}

void RenderTree::SetTimer(Timer * timer)
{
	m_timer = timer;
}

void RenderTree::MouseTrap(float x, float y)
{
	XMFLOAT2 NMouse;
	XMFLOAT2 LineEnd;

	NMouse.x = (2.0*x / m_width - 1) * abs(m_camPos.z);
	NMouse.y = (2.0*y / m_height - 1) * abs(m_camPos.z);

	LineEnd.x = NMouse.x + m_camPos.x;
	LineEnd.y = -NMouse.y + m_camPos.y;

	//printf("Mouse coord: x = %f\ty = %f;\n", LineEnd.x, LineEnd.y);
	int t = m_labelSearch(LineEnd.x, LineEnd.y);
	if (t)
	{
		std::wstring adress = L"photo//photo";
		adress.append(std::to_wstring(t));
		adress.append(L".jpg");
		m_lockInfo1 = true;
		if (!m_infoLable->equilPhoto(adress))
		{
			m_infoLable->SetPhoto(adress, new Image(this), 2 * m_height / 3);
			m_infoLable->SetText(m_prepareInfoForLable(t));
			m_drawInfoLable();
		}
	}
	else
	{
		m_lockInfo1 = false;
		if (m_infoLable->isPhoto())
			m_infoLable->Clear();
	}
}

void RenderTree::MouseMove(float speed, const char axis)
{
	switch (axis)
	{
	case 'x':
		m_cam->MoveX(speed);
		m_camPos.x += speed;
		break;
	case 'y':
		m_cam->MoveY(speed);
		m_camPos.y += speed;
		break;
	case 'z':
		m_cam->MoveZ(speed);
		m_camPos.z += speed;
		break;
	}
}

void RenderTree::AngelFall()
{
	m_infoLable->Clear();
	m_notClosed = false;
}

bool RenderTree::m_initPhotos(int i, std::wstring fileName)
{
	Image *img = new Image(this);
	if (!img)
		return false;

	// запросом спрашивать имя
	m_lables[i]->AddImg(fileName.data(), img);

}

void RenderTree::m_initTextes(int i, std::wstring txt1, std::wstring txt2)
{
	Text *text = new Text(this, m_font);
	m_lables[i]->AddText(txt1, text);
	if (!txt2.empty())
	{
		Text *text2 = new Text(this, m_font);
		m_lables[i]->AddText(txt2, text2);
	}
}


