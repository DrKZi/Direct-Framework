#include "stdafx.h"
#include "Input.h"


Input::Input(RenderTree * render)
{
	timer = new Timer;
	m_render = render;
	m_render->SetTimer(timer);
	m_lock = true;
}

bool Input::MouseMove(const MouseEvent & arg)
{
	m_render->MouseTrap(arg.x, arg.y);
	if (m_lock)
		m_lock = false;
	else
	{
		if (timer->GetCurOption() == 0)
			timer->SetTimer(5.f);
		else
			timer->Restart();
	}
	return false;
}

bool Input::KeyPressed(const KeyEvent & arg)
{
	switch (arg.wc)
	{
	case L'd':
		m_render->MouseMove(0.005f, 'x');
		break;
	case L'a':
		m_render->MouseMove(-0.005f, 'x');
		break;
	case L'w':
		m_render->MouseMove(0.005f, 'y');
		break;
	case L's':
		m_render->MouseMove(-0.005f, 'y');
		break;
	case L' ':
		m_render->MouseMove(0.005f, 'z');
		break;
	case L'c':
		m_render->MouseMove(-0.005f, 'z');
		break;
	}

	if (arg.code == KEY_ESCAPE)
		m_render->AngelFall();

	return false;
}

