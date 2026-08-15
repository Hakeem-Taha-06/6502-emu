#pragma once

#include <cstdint>
#include <array>
#include <memory>

#include "cpu6502.h"

class Bus
{
public:
	virtual ~Bus() = default;

	virtual uint8_t cpuRead(uint16_t address) = 0;
	virtual void cpuWrite(uint16_t address, uint8_t data) = 0;

	virtual void clock() = 0;
	virtual void reset() = 0;
};

class TestBus : public Bus
{
public:
	TestBus();
	~TestBus();

	uint8_t cpuRead(uint16_t address) override;
	void cpuWrite(uint16_t address, uint8_t data) override;

	void clock() override;
	void reset() override;

	inline const uint8_t* getRam()const { return ram.data(); }

	cpu6502 cpu;

private:
	// temporary ram
	std::array<uint8_t, 64 * 1024> ram;
};