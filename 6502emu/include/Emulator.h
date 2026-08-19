#pragma once

#include <fstream>
#include <vector>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <memory>

#include "Bus.h"

class Emulator {
public:
	Emulator();
	~Emulator();

	void reset();
	void clock();
	void load(std::string path, uint16_t writeAddr);
	void clearMem();

	TestBus bus;
	
	int emulationSpeed = 1;
};