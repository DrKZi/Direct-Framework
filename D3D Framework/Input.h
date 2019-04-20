#pragma once

#include "RenderTree.h"


class Input : public InputListener
{
public:
	Input(RenderTree* render);

	bool MouseMove(const MouseEvent &arg);

	bool KeyPressed(const KeyEvent &arg);

private:
	Timer* timer;
	RenderTree *m_render;
	bool m_lock;
};

