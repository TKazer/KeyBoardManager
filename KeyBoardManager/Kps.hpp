#pragma once
#include "KeyBoard_Async.hpp"

class KpsApp
{
public:
	struct KeyData
	{
		int MaxKps = 0;
		int Counts = 0;
		std::deque<TimePoint> EventTimeDeque;
	};
private:
	KeyBoardManager::KeyBoard_Async KeyBoard;
	std::mutex m;
	std::vector<KeyData> Keys;
	std::vector<int> VKeyList;
	int TotalMaxKps = 0;
public:
	KpsApp()
	{
		this->Keys.resize(256);
		this->KeyBoard._set_callback(
			std::bind(&KpsApp::AddKeyEvent, this,
				std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		this->KeyBoard.StartListening();
	}

	~KpsApp()
	{
		this->KeyBoard.StopListening();
	}

	void AddKey(std::vector<int> VKeys)
	{
		if (VKeys.size() <= 0)
			return;
		std::lock_guard<std::mutex> Lock(this->m);
		for (const auto& VKey : VKeys)
		{
			if (!this->IsInKeyList(VKey))
			{
				this->VKeyList.push_back(VKey);
			}
		}
	}

	void RemoveKey(int VKey)
	{
		std::lock_guard<std::mutex> Lock(this->m);
		auto iter = std::find(this->VKeyList.begin(), this->VKeyList.end(), VKey);
		if (iter != this->VKeyList.end())
		{
			this->VKeyList.erase(iter);
		}
	}

	int GetCurrentKps(int VKey = -1)
	{
		std::lock_guard<std::mutex> Lock(this->m);

		this->UpdateKeysDeque();
		if (VKey == -1)
			return this->GetKeysDequeSize();
		if (!this->IsInKeyList(VKey))
			return 0;
		return this->Keys[VKey].EventTimeDeque.size();
	}

	int GetMaxKps(int VKey = -1)
	{
		std::lock_guard<std::mutex> Lock(this->m);

		if (VKey == -1)
			return this->TotalMaxKps;
		if (!this->IsInKeyList(VKey))
			return 0;
		return this->Keys[VKey].MaxKps;
	}

	int GetCounts(int VKey = -1)
	{
		std::lock_guard<std::mutex> Lock(this->m);

		if (VKey == -1)
			return this->GetKeysCounts();
		if (!this->IsInKeyList(VKey))
			return 0;
		return this->Keys[VKey].Counts;
	}

	void ResetMaxKps(int VKey = -1)
	{
		std::lock_guard<std::mutex> Lock(this->m);

		if (VKey == -1)
		{
			for (auto& Key : this->Keys)
			{
				Key.MaxKps = 0;
			}
			this->TotalMaxKps = 0;
		}
		if (!this->IsInKeyList(VKey))
			return;
		this->Keys[VKey].MaxKps = 0;
	}

	void ResetCounts(int VKey = -1)
	{
		std::lock_guard<std::mutex> Lock(this->m);

		if (VKey == -1)
		{
			for (auto& Key : this->Keys)
			{
				Key.Counts = 0;
			}
		}
		if (!this->IsInKeyList(VKey))
			return;
		this->Keys[VKey].Counts = 0;
	}

private:
	void AddKeyEvent(int VKey, TimePoint Time, bool down)
	{
		std::lock_guard<std::mutex> Lock(this->m);
		if (!IsInKeyList(VKey))
			return;

		this->Keys[VKey].EventTimeDeque.push_back(Time);
		UpdateKeysDeque();

		this->Keys[VKey].Counts++;
		this->Keys[VKey].MaxKps = std::max(this->Keys[VKey].MaxKps, (int)this->Keys[VKey].EventTimeDeque.size());
		this->TotalMaxKps = std::max(this->TotalMaxKps, this->GetKeysDequeSize());
	}

	bool IsInKeyList(int VKey)
	{
		auto iter = std::find(this->VKeyList.begin(), this->VKeyList.end(), VKey);
		return iter != this->VKeyList.end();
	}

	int GetKeysDequeSize()
	{
		return std::accumulate(this->Keys.begin(), this->Keys.end(), 0,
			[&](int Result, KeyData Key)
			{
				return Result + Key.EventTimeDeque.size();
			});
	}

	int GetKeysCounts()
	{
		return std::accumulate(this->Keys.begin(), this->Keys.end(), 0,
			[&](int Result, KeyData Key)
			{
				return Result + Key.Counts;
			});
	}

	void UpdateKeysDeque()
	{
		TimePoint Now = std::chrono::steady_clock::now();
		for (auto& Key : this->Keys)
		{
			for (int i = 0; i < Key.EventTimeDeque.size();)
			{
				if (Now - Key.EventTimeDeque[i] > std::chrono::seconds(1))
				{
					Key.EventTimeDeque.pop_front();
					continue;
				}
				i++;
			}
		}
	}
};
