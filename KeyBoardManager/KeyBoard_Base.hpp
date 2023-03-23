#pragma once
#include <iostream>
#include <windows.h>
#include <chrono>
#include <vector>
#include <thread>
#include <deque>
#include <numeric>
#include <mutex>
#include <functional>

#undef max

using TimePoint = std::chrono::steady_clock::time_point;

namespace KeyBoardManager
{
	class key_info
	{
	public:
		bool is_pressed = false;
		TimePoint lastest_down_time{};
		TimePoint lastest_up_time{};
	};

	class _keyboard_base
	{
	public:
		using CallBack = std::function<void(int, TimePoint, bool)>;
	private:
		CallBack _key_down_callback = nullptr;
		CallBack _key_up_callback = nullptr;
		std::mutex m;
		std::vector<key_info> keys;
	public:
		_keyboard_base()
		{
			keys.resize(256);
		}
		void _set_callback(CallBack down, CallBack up = nullptr)
		{
			this->_key_down_callback = down;
			this->_key_up_callback = up;
		}
		void _key_down(int vkey, TimePoint time)
		{
			std::lock_guard<std::mutex> _(m);
			if (vkey < 0 || vkey > 256)
				return;
			if (!this->keys[vkey].is_pressed)
			{
				this->keys[vkey].is_pressed = true;
				this->keys[vkey].lastest_down_time = time;
				if (this->_key_down_callback != nullptr)
					this->_key_down_callback(vkey, time, true);
			}
		}
		void _key_up(int vkey, TimePoint time)
		{
			std::lock_guard<std::mutex> _(m);
			if (vkey < 0 || vkey > 256)
				return;
			if (this->keys[vkey].is_pressed)
			{
				this->keys[vkey].is_pressed = false;
				this->keys[vkey].lastest_up_time = time;
				if (this->_key_up_callback != nullptr)
					this->_key_up_callback(vkey, time, true);
			}
		}
		key_info at(int index)
		{
			std::lock_guard<std::mutex> _(m);
			return this->keys[index];
		}
		virtual void _listen_thread() = 0;
	};
}
