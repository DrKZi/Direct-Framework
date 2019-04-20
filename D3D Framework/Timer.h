#pragma once

namespace D3D11Framework
{
    //------------------------------------------------------------------

    class Timer
    {
	public:
		Timer();

		void SetTimer(float time);
		void Restart();

		float GetCurOption();

		bool Resault();

		void Stop();

	private:
		DWORD m_TimerStart;
		DWORD m_TimerGoal;

		float m_curSet;
    };

    //------------------------------------------------------------------
}

