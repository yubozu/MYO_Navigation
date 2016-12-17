#ifndef MYO_NAVIGATION_OS
#define MYO_NAVIGATION_OS
#include <iostream>
#include <iomanip>
#include <process.h>
#include <myo/myo.hpp>
#include "DataCollector.h"
void cooperationWork()
{
	try {
		DataCollector dataCollector;
		myo::Hub hub("com.example.hello-myo");
		std::cout << "Attempting to find a Myo..." << std::endl;
		myo::Myo* myo = hub.waitForMyo(10000);
		if (!myo) {
			throw std::runtime_error("Unable to find a Myo!");
		}
		std::cout << "Connected to a Myo armband!" << std::endl << std::endl;
		DataCollector collector;
		hub.addListener(&collector);
		hub.setLockingPolicy(hub.LockingPolicy::lockingPolicyNone);

		while (1) {
			hub.run(1000 / 20);
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		std::cerr << "Press enter to continue.";
		std::cin.ignore();
	}
	system("pause");
}
#endif