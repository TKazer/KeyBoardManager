#include "Kps.hpp"

int main()
{
	KpsApp Kps;
	Kps.AddKey({ VK_HOME,VK_END,VK_DELETE });

	while (true)
	{
		int CurrentKps = Kps.GetCurrentKps();
		int MaxKps = Kps.GetMaxKps();
		int Counts = Kps.GetCounts();

		if (GetAsyncKeyState(VK_INSERT))
		{
			Kps.ResetCounts();
			Kps.ResetMaxKps();
		}

		if (GetAsyncKeyState(VK_F12))
		{
			Kps.RemoveKey(VK_DELETE);
		}

		std::cout << "Kps:" << CurrentKps << ",Max:" << MaxKps << ",Counts=" << Counts << std::endl;
		std::cout << "[Home] Kps:" << Kps.GetCurrentKps(VK_HOME) 
			<< ",Max:" << Kps.GetMaxKps(VK_HOME) 
			<< ",Counts=" << Kps.GetCounts(VK_HOME) << std::endl;

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		system("cls");
	};

	system("pause");
	return 0;
}