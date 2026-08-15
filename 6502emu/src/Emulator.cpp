#include "Emulator.h"

Emulator::Emulator() {
	
}

Emulator::~Emulator() {
	
}

void Emulator::reset(){
	bus.reset();
}


void Emulator::load(std::string path) {


	std::ifstream file( path, std::ios::binary | std::ios::ate);
	if (!file)
		throw std::runtime_error("Couldn't read file: "+path);

	size_t size = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<uint8_t> buffer(size);
	if (!file.read(reinterpret_cast<char*>(buffer.data()), size))
		throw std::runtime_error("Failed to read file: " + path);
		
	for (int i = 0; i < size; ++i)
		bus.cpuWrite(0x0600 + (uint16_t)i, buffer[i]); 
	
	// reset vector
	bus.cpuWrite(0xFFFC, 0x00);
	bus.cpuWrite(0xFFFD, 0x06);
	reset();
}

void Emulator::clock() {
	bus.clock();
}
