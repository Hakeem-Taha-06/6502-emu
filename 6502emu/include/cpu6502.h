#pragma once
#include <cstdint>

#include <vector>
#include <map>
#include <string>

class Bus;

class cpu6502
{
public:
	cpu6502();
	~cpu6502();

	void connectBus(Bus* bus);

	// status flags that change how certain functions act
	enum FLAGS6502 {
		C = (1 << 0), // Carry
		Z = (1 << 1), // Zero
		I = (1 << 2), // Interrupt disable
		D = (1 << 3), // Decimal mode
		B = (1 << 4), // Break
		U = (1 << 5), // Unused
		V = (1 << 6), // Overflow
		N = (1 << 7), // Negative
	};

	uint16_t pc = 0x0000;  // Program Counter
	uint8_t stkp = 0x00;   // Stack Pointer
	uint8_t acc = 0x00;    // Accumulator
	uint8_t xreg = 0x00;   // X Register
	uint8_t yreg = 0x00;   // Y Register
	uint8_t stat = 0x00;   // Status Register

	int totalCycles = 0;

	
	void clock();
	void reset();
	void irq();  // interrupt request that can be masked using the I flag
	void nmi();  // non-maskable interrupt request that can't be masked with the I flag

	// fetches a byte of data based on the addressing mode 
	uint8_t fetch();
	
	inline bool complete() const { return cycles == 0; }

	uint8_t getFlag(FLAGS6502 f) const;

// switch to private later
public:
	Bus* bus = nullptr;

	// for the cpu to use when executing instructions
	uint8_t read(uint16_t address);
	void write(uint16_t address, uint8_t data);

	void push(uint8_t data);
	uint8_t pop();
	uint8_t peek();
	
	void setFlag(FLAGS6502 f, bool v);

	
	uint8_t fetched = 0x00;
	uint8_t opcode = 0x00;        // the instruction byte
	uint8_t cycles = 0;		      // number of cycles left for an instruction
	uint16_t addr_abs = 0x0000;   // this address is modified based on the addressing mode
	uint16_t addr_rel = 0x00;     // for jump instructions that use relative addresses


	//Addressing modes
	uint8_t IMP(); uint8_t IMM();
	uint8_t ZP0(); uint8_t IND();
	uint8_t REL(); uint8_t ABS();
	uint8_t ZPX(); uint8_t ZPY();
	uint8_t ABX(); uint8_t ABY();
	uint8_t IZX(); uint8_t IZY();

	// Opcodes (Mnemonics)
	uint8_t ADC(); uint8_t AND(); uint8_t ASL(); uint8_t BCC();
	uint8_t BCS(); uint8_t BEQ(); uint8_t BIT(); uint8_t BMI();
	uint8_t BNE(); uint8_t BPL(); uint8_t BRK(); uint8_t BVC();
	uint8_t BVS(); uint8_t CLC(); uint8_t CLD(); uint8_t CLI();
	uint8_t CLV(); uint8_t CMP(); uint8_t CPX(); uint8_t CPY();
	uint8_t DEC(); uint8_t DEX(); uint8_t DEY(); uint8_t EOR();
	uint8_t INC(); uint8_t INX(); uint8_t INY(); uint8_t JMP();
	uint8_t JSR(); uint8_t LDA(); uint8_t LDX(); uint8_t LDY();
	uint8_t LSR(); uint8_t NOP(); uint8_t ORA(); uint8_t PHA();
	uint8_t PHP(); uint8_t PLA(); uint8_t PLP(); uint8_t ROL();
	uint8_t ROR(); uint8_t RTI(); uint8_t RTS(); uint8_t SBC();
	uint8_t SEC(); uint8_t SED(); uint8_t SEI(); uint8_t STA();
	uint8_t STX(); uint8_t STY(); uint8_t TAX(); uint8_t TAY();
	uint8_t TSX(); uint8_t TXA(); uint8_t TXS(); uint8_t TYA();

	// Invalid/Illegal Opcodes
	uint8_t XXX();

	void _branch();

	struct Instruction {
		std::string name;                             // mnemonic for display in disassembler
		uint8_t(cpu6502::* operate)(void) = nullptr;  // the opcode mnemonic (aka the function containing the operation logic)
		uint8_t(cpu6502::* addrmode)(void) = nullptr; // the addressing mode of the instruction
		uint8_t cycles = 0;                           // number of cycles taken by the instruction
	};

	std::vector<Instruction> lookup;  // lookup table for the instructions given an opcode

	std::map<uint16_t, std::string> disassemble(uint16_t startAddr, uint16_t endAddr);
};

