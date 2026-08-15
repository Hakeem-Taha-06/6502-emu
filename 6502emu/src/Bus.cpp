#include "Bus.h"

// ----------------------------- TESTBUS
TestBus::TestBus() {
	cpu.connectBus(this);
	ram.fill(0x00);
}

TestBus::~TestBus() {

}

uint8_t TestBus::cpuRead(uint16_t address) {
	if(address >= 0x0000 && address <= 0xFFFF)
		return ram[address];

	return 0x00;  
}

void TestBus::cpuWrite(uint16_t address, uint8_t data) {
	if (address >= 0x0000 && address <= 0xFFFF)
		ram[address] = data;
}

void TestBus::clock() {
	cpu.clock();
}

void TestBus::reset() {
	cpu.reset();
}