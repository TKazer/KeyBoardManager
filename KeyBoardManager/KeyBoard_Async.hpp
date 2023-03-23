#pragma once
#include "KeyBoard_Base.hpp"

namespace KeyBoardManager
{
	class KeyBoard_Async : public _keyboard_base
	{
	private:
		std::thread ListenThread;
		bool ExitThread = false;
	public:
		void StartListening()
		{
			this->ListenThread = std::thread{ &KeyBoard_Async::_listen_thread, this };
		}
		void StopListening()
		{
			this->ExitThread = true;
		}
		void _listen_thread()
		{
			while (true)
			{
				if (ExitThread)
					break;
				for (int i = 0; i < 256; i++)
				{
					if (GetAsyncKeyState(i))
					{
						this->_key_down(i, std::chrono::steady_clock::now());
					}
					else
					{
						this->_key_up(i, std::chrono::steady_clock::now());
					}
				}
			}
		}
	};
}
