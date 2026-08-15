#include "cpu6502.h"
#include "Bus.h"

cpu6502::cpu6502() {

	//lookup table by javidx9 containing all the legal instructions on the 6502
	using a = cpu6502;
	lookup =
	{
		{ "BRK", &a::BRK, &a::IMM, 7 },{ "ORA", &a::ORA, &a::IZX, 6 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 3 },{ "ORA", &a::ORA, &a::ZP0, 3 },{ "ASL", &a::ASL, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "PHP", &a::PHP, &a::IMP, 3 },{ "ORA", &a::ORA, &a::IMM, 2 },{ "ASL", &a::ASL, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::NOP, &a::IMP, 4 },{ "ORA", &a::ORA, &a::ABS, 4 },{ "ASL", &a::ASL, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
		{ "BPL", &a::BPL, &a::REL, 2 },{ "ORA", &a::ORA, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "ORA", &a::ORA, &a::ZPX, 4 },{ "ASL", &a::ASL, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "CLC", &a::CLC, &a::IMP, 2 },{ "ORA", &a::ORA, &a::ABY, 4 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "ORA", &a::ORA, &a::ABX, 4 },{ "ASL", &a::ASL, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
		{ "JSR", &a::JSR, &a::ABS, 6 },{ "AND", &a::AND, &a::IZX, 6 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "BIT", &a::BIT, &a::ZP0, 3 },{ "AND", &a::AND, &a::ZP0, 3 },{ "ROL", &a::ROL, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "PLP", &a::PLP, &a::IMP, 4 },{ "AND", &a::AND, &a::IMM, 2 },{ "ROL", &a::ROL, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "BIT", &a::BIT, &a::ABS, 4 },{ "AND", &a::AND, &a::ABS, 4 },{ "ROL", &a::ROL, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
		{ "BMI", &a::BMI, &a::REL, 2 },{ "AND", &a::AND, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "AND", &a::AND, &a::ZPX, 4 },{ "ROL", &a::ROL, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "SEC", &a::SEC, &a::IMP, 2 },{ "AND", &a::AND, &a::ABY, 4 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "AND", &a::AND, &a::ABX, 4 },{ "ROL", &a::ROL, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
		{ "RTI", &a::RTI, &a::IMP, 6 },{ "EOR", &a::EOR, &a::IZX, 6 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 3 },{ "EOR", &a::EOR, &a::ZP0, 3 },{ "LSR", &a::LSR, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "PHA", &a::PHA, &a::IMP, 3 },{ "EOR", &a::EOR, &a::IMM, 2 },{ "LSR", &a::LSR, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "JMP", &a::JMP, &a::ABS, 3 },{ "EOR", &a::EOR, &a::ABS, 4 },{ "LSR", &a::LSR, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
		{ "BVC", &a::BVC, &a::REL, 2 },{ "EOR", &a::EOR, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "EOR", &a::EOR, &a::ZPX, 4 },{ "LSR", &a::LSR, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "CLI", &a::CLI, &a::IMP, 2 },{ "EOR", &a::EOR, &a::ABY, 4 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "EOR", &a::EOR, &a::ABX, 4 },{ "LSR", &a::LSR, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
		{ "RTS", &a::RTS, &a::IMP, 6 },{ "ADC", &a::ADC, &a::IZX, 6 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 3 },{ "ADC", &a::ADC, &a::ZP0, 3 },{ "ROR", &a::ROR, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "PLA", &a::PLA, &a::IMP, 4 },{ "ADC", &a::ADC, &a::IMM, 2 },{ "ROR", &a::ROR, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "JMP", &a::JMP, &a::IND, 5 },{ "ADC", &a::ADC, &a::ABS, 4 },{ "ROR", &a::ROR, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
		{ "BVS", &a::BVS, &a::REL, 2 },{ "ADC", &a::ADC, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "ADC", &a::ADC, &a::ZPX, 4 },{ "ROR", &a::ROR, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "SEI", &a::SEI, &a::IMP, 2 },{ "ADC", &a::ADC, &a::ABY, 4 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "ADC", &a::ADC, &a::ABX, 4 },{ "ROR", &a::ROR, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
		{ "???", &a::NOP, &a::IMP, 2 },{ "STA", &a::STA, &a::IZX, 6 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 6 },{ "STY", &a::STY, &a::ZP0, 3 },{ "STA", &a::STA, &a::ZP0, 3 },{ "STX", &a::STX, &a::ZP0, 3 },{ "???", &a::XXX, &a::IMP, 3 },{ "DEY", &a::DEY, &a::IMP, 2 },{ "???", &a::NOP, &a::IMP, 2 },{ "TXA", &a::TXA, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "STY", &a::STY, &a::ABS, 4 },{ "STA", &a::STA, &a::ABS, 4 },{ "STX", &a::STX, &a::ABS, 4 },{ "???", &a::XXX, &a::IMP, 4 },
		{ "BCC", &a::BCC, &a::REL, 2 },{ "STA", &a::STA, &a::IZY, 6 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 6 },{ "STY", &a::STY, &a::ZPX, 4 },{ "STA", &a::STA, &a::ZPX, 4 },{ "STX", &a::STX, &a::ZPY, 4 },{ "???", &a::XXX, &a::IMP, 4 },{ "TYA", &a::TYA, &a::IMP, 2 },{ "STA", &a::STA, &a::ABY, 5 },{ "TXS", &a::TXS, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 5 },{ "???", &a::NOP, &a::IMP, 5 },{ "STA", &a::STA, &a::ABX, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "???", &a::XXX, &a::IMP, 5 },
		{ "LDY", &a::LDY, &a::IMM, 2 },{ "LDA", &a::LDA, &a::IZX, 6 },{ "LDX", &a::LDX, &a::IMM, 2 },{ "???", &a::XXX, &a::IMP, 6 },{ "LDY", &a::LDY, &a::ZP0, 3 },{ "LDA", &a::LDA, &a::ZP0, 3 },{ "LDX", &a::LDX, &a::ZP0, 3 },{ "???", &a::XXX, &a::IMP, 3 },{ "TAY", &a::TAY, &a::IMP, 2 },{ "LDA", &a::LDA, &a::IMM, 2 },{ "TAX", &a::TAX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "LDY", &a::LDY, &a::ABS, 4 },{ "LDA", &a::LDA, &a::ABS, 4 },{ "LDX", &a::LDX, &a::ABS, 4 },{ "???", &a::XXX, &a::IMP, 4 },
		{ "BCS", &a::BCS, &a::REL, 2 },{ "LDA", &a::LDA, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 5 },{ "LDY", &a::LDY, &a::ZPX, 4 },{ "LDA", &a::LDA, &a::ZPX, 4 },{ "LDX", &a::LDX, &a::ZPY, 4 },{ "???", &a::XXX, &a::IMP, 4 },{ "CLV", &a::CLV, &a::IMP, 2 },{ "LDA", &a::LDA, &a::ABY, 4 },{ "TSX", &a::TSX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 4 },{ "LDY", &a::LDY, &a::ABX, 4 },{ "LDA", &a::LDA, &a::ABX, 4 },{ "LDX", &a::LDX, &a::ABY, 4 },{ "???", &a::XXX, &a::IMP, 4 },
		{ "CPY", &a::CPY, &a::IMM, 2 },{ "CMP", &a::CMP, &a::IZX, 6 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "CPY", &a::CPY, &a::ZP0, 3 },{ "CMP", &a::CMP, &a::ZP0, 3 },{ "DEC", &a::DEC, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "INY", &a::INY, &a::IMP, 2 },{ "CMP", &a::CMP, &a::IMM, 2 },{ "DEX", &a::DEX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "CPY", &a::CPY, &a::ABS, 4 },{ "CMP", &a::CMP, &a::ABS, 4 },{ "DEC", &a::DEC, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
		{ "BNE", &a::BNE, &a::REL, 2 },{ "CMP", &a::CMP, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "CMP", &a::CMP, &a::ZPX, 4 },{ "DEC", &a::DEC, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "CLD", &a::CLD, &a::IMP, 2 },{ "CMP", &a::CMP, &a::ABY, 4 },{ "NOP", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "CMP", &a::CMP, &a::ABX, 4 },{ "DEC", &a::DEC, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
		{ "CPX", &a::CPX, &a::IMM, 2 },{ "SBC", &a::SBC, &a::IZX, 6 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "CPX", &a::CPX, &a::ZP0, 3 },{ "SBC", &a::SBC, &a::ZP0, 3 },{ "INC", &a::INC, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "INX", &a::INX, &a::IMP, 2 },{ "SBC", &a::SBC, &a::IMM, 2 },{ "NOP", &a::NOP, &a::IMP, 2 },{ "???", &a::SBC, &a::IMP, 2 },{ "CPX", &a::CPX, &a::ABS, 4 },{ "SBC", &a::SBC, &a::ABS, 4 },{ "INC", &a::INC, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
		{ "BEQ", &a::BEQ, &a::REL, 2 },{ "SBC", &a::SBC, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "SBC", &a::SBC, &a::ZPX, 4 },{ "INC", &a::INC, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "SED", &a::SED, &a::IMP, 2 },{ "SBC", &a::SBC, &a::ABY, 4 },{ "NOP", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "SBC", &a::SBC, &a::ABX, 4 },{ "INC", &a::INC, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
	};
}

cpu6502::~cpu6502() {

}

void cpu6502::connectBus(Bus* b) {
	bus = b;
}

uint8_t cpu6502::read(uint16_t address) {
	return bus->cpuRead(address);
}
void cpu6502::write(uint16_t address, uint8_t data) {
	bus->cpuWrite(address, data);
}

// stack operations
void cpu6502::push(uint8_t data) {
	write(0x100 + stkp, data);
	stkp--;
}
uint8_t cpu6502::pop() {
	stkp++;
	return read(0x100 + stkp);
}
uint8_t cpu6502::peek() {
	return read(0x100 + stkp + 1);
}

void cpu6502::reset() {
	// Set program counter to reset vector stored at 0xFFFC/D
	uint8_t lo = read(0xFFFC);
	uint8_t hi = read(0xFFFD);
	pc = (hi << 8) | lo;

	stkp = 0xFD;
	acc = 0x00;
	xreg = 0x00;
	yreg = 0x00;
	stat = 0x00;

	cycles = 0x00;
	addr_abs = 0x0000;
	addr_rel = 0x0000;
	fetched = 0x00;

	cycles = 8;
	totalCycles = 0;
}

void cpu6502::irq() {
	if (getFlag(I) == 0) {

		// Write the current pc to the stack
		push((pc >> 8) & 0x00FF);   // the pc is in the stack in this order
		push(pc & 0x00FF);		    //            -> LL HH

		// Write the current status register, and set I
		setFlag(B, 0);
		setFlag(I, 1);
		setFlag(U, 1);
		push(stat);

		// pc = Interrupt Request handler address
		uint16_t lo = read(0xFFFE);
		uint16_t hi = read(0xFFFF);
		pc = (hi << 8) | lo;

		cycles = 7;
	}
}

void cpu6502::nmi() {
	// Write the current pc to the stack
	push((pc >> 8) & 0x00FF);   // the pc is in the stack in this order
	push(pc & 0x00FF);		    //            -> LL HH

	// Write the current status register, and set I
	setFlag(B, 0);
	setFlag(I, 1);
	setFlag(U, 1);
	push(stat);

	// pc = Non-Maskable Interrupt Request handler address
	uint16_t lo = read(0xFFFA);
	uint16_t hi = read(0xFFFB);
	pc = (hi << 8) | lo;

	cycles = 7;
}

void cpu6502::clock() {
	
	if (cycles == 0) {
		//read the data at the program counter
		opcode = read(pc);
		pc++;

		cycles += lookup[opcode].cycles;

		//run the instruction
		uint8_t additionalCycle1 = (this->*lookup[opcode].addrmode)();
		uint8_t additionalCycle2 = (this->*lookup[opcode].operate)();

		cycles += (additionalCycle1 & additionalCycle2);
	}

	totalCycles++;
	cycles--;
}

uint8_t cpu6502::fetch(){
	if (lookup[opcode].addrmode != &cpu6502::IMP)
		fetched = read(addr_abs);
	return fetched;
}

uint8_t cpu6502::getFlag(FLAGS6502 f) const {
	return (stat & f) > 0 ? 1 : 0;
}

void cpu6502::setFlag(FLAGS6502 f, bool v) {
	if (v)
		stat |= f;
	else
		stat &= ~f;
}

// ---------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Addressing modes ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ---------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------
// Implicit: no data needed, but some instructions operate on the value in the accumulator
uint8_t cpu6502::IMP() {
	fetched = acc; // set the fetched data as the data in the accumulator
	return 0;
}

// Immediate: the next byte represents the target address 
uint8_t cpu6502::IMM() {
	addr_abs = pc; // set the pc as the address then increment it
	pc++;
	return 0;
}

// Zero Page: the next byte represents the offset in the zero page, set the address to the result
uint8_t cpu6502::ZP0() {
	uint8_t zeroPageOffset = read(pc);    // first we read the data at the program counter 
	pc++;                                 // increment the program counter
	addr_abs = zeroPageOffset & 0x00FF;   // the read data represents the offset into the zero page, set the result as the address
	return 0;
}

// Zero Page X: The address to be accessed by an instruction using indexed zero page addressing is calculated by taking the 
//             8 bit zero page address from the instruction and adding the current value of the X register to it
uint8_t cpu6502::ZPX() {
	uint8_t zeroPageOffset = read(pc);             // read the data at the program counter
	pc++;                                          // increment the program counter
	addr_abs = ((zeroPageOffset + xreg) & 0x00FF); // the address is the LBO of the offset into the zero page + the contents of the x register
	return 0;
}

// Zero Page X: The address to be accessed by an instruction using indexed zero page addressing is calculated by taking the 
//             8 bit zero page address from the instruction and adding the current value of the Y register to it
uint8_t cpu6502::ZPY() {
	uint8_t zeroPageOffset = read(pc);             // read the data at the program counter
	pc++;                                          // increment the program counter
	addr_abs = ((zeroPageOffset + yreg) & 0x00FF); // the address is the LBO of the offset into the zero page + the contents of the y register
	return 0;
}

// Absolute: the next two bytes represent the target location
uint8_t cpu6502::ABS() {
	//assumes little endian order of low byte followed by high byte
	uint16_t lo = read(pc); // read the low byte of the address and increment pc
	pc++;
	uint16_t hi = read(pc); // read the high byte of the address and increment pc
	pc++;

	addr_abs = (hi << 8) | lo; // HHHH HHHH LLLL LLLL
	return 0;
}

uint8_t cpu6502::ABX() { 
	//assumes little endian order of low byte followed by high byte
	uint16_t lo = read(pc); // read the low byte of the address and increment pc
	pc++;
	uint16_t hi = read(pc); // read the high byte of the address and increment pc
	pc++;

	addr_abs = (hi << 8) | lo; // HHHH HHHH LLLL LLLL
	addr_abs += xreg;

	// If adding xreg results in moving to another page then add another cycle for the instruction
	// this can be checked by checking for the high byte of the address and comparing it to the high byte before adding xreg
	if ((addr_abs & 0xFF00) != (hi << 8))
		return 1;

	return 0; 
}

uint8_t cpu6502::ABY() {
	//assumes little endian order of low byte followed by high byte
	uint16_t lo = read(pc); // read the low byte of the address and increment pc
	pc++;
	uint16_t hi = read(pc); // read the high byte of the address and increment pc
	pc++;

	addr_abs = (hi << 8) | lo; // HHHH HHHH LLLL LLLL
	addr_abs += yreg;

	// If adding yreg results in moving to another page then add another cycle for the instruction
	// this can be checked by checking for the high byte of the address and comparing it to the high byte before adding yreg
	if ((addr_abs & 0xFF00) != (hi << 8))
		return 1;

	return 0;
}

// Indirect: the next two byte represent the memory location holding the value of the target location (pointers)
uint8_t cpu6502::IND() { 

	uint16_t ptr_lo = read(pc);
	pc++;
	uint16_t ptr_hi = read(pc);
	pc++;

	uint16_t ptr = (ptr_hi << 8) | ptr_lo;

	if (ptr_lo == 0x00FF)  // Simulate page boundary hardware bug (the high byte failing to increment at the end of a page)
		addr_abs = (read(ptr & 0xFF00) << 8) | read(ptr);
	else 
		// Normal behaviour
		addr_abs = (read(ptr + 1) << 8) | read(ptr);
	
	return 0; 
}

// Indexed Indirect: the instruction specifies a zero page location and then the xreg is added to it,
//                   the resulting value represents the address where the target location value lives
uint8_t cpu6502::IZX() {
	// the first operand represents the zero page index
	uint16_t zp = read(pc);
	pc++;

	// add the xreg to it and wrap around, this value represents the address of the target location
	uint16_t ptr = (zp + xreg) & 0x00FF;

	// read the two bytes at the location specified, this is the target location
	uint16_t lo = read(ptr);
	uint16_t hi = read(ptr + 1);

	addr_abs = (hi << 8) | lo;

	return 0;
}

// Indirect Indexed: the instruction specifies a zero page location which represents where 
//                   the unmodified target location lives, the yreg is then added to it, 
//                   the resulting value is the target location.
//                   if the addition of the yreg results in changing pages, add 1 extra cycle
uint8_t cpu6502::IZY() {
	uint16_t zp = read(pc);
	pc++;

	uint16_t lo = read(zp & 0x00FF);      // read two bytes for the initial target location  
	uint16_t hi = read((zp + 1) & 0x00FF);

	uint16_t ptr = (hi << 8) | lo;
	addr_abs = ptr + yreg;                // add the yreg to get the final target location 

	// if this results in incrementing the page, add an extra cycle to the instruction
	if ((addr_abs & 0xFF00)!= hi << 8) 
		return 1;
	else 
		return 0;
}

// Relative: specifies a signed offset from -128 to 127 from the current program counter
uint8_t cpu6502::REL() { 
	addr_rel = read(pc);
	pc++;
	if (addr_rel & 0x80)     // if the value is negative, set its high byte to all 1's 
		addr_rel |= 0xFF00;
	return 0;
}

// ---------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Operations ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ---------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------

// LOAD INSTRUCTIONS

// Load Accumulator: Loads a byte of memory into the accumulator setting the zero and negative flags as appropriate
uint8_t cpu6502::LDA() {
	acc = fetch();                     // reads a byte of memory into the accumulator
	setFlag(Z, acc == 0x00);           // set the zero flag if the accumulator is zero
	setFlag(N, acc & 0x80);            // set the negative flag if the MSB is high
	return 0;
}

// Load X: Loads a byte of memory into the X register setting the zero and negative flags as appropriate
uint8_t cpu6502::LDX() {
	xreg = fetch();                       // reads a byte of memory into the x register
	setFlag(Z, xreg == 0x00);             // set the zero flag if the x register is zero
	setFlag(N, xreg & 0x80);              // set the overflow flag if the MSB is high
	return 0;
}

// Load Y: Loads a byte of memory into the Y register setting the zero and negative flags as appropriate
uint8_t cpu6502::LDY() {
	yreg = fetch();                    // reads a byte of memory into the y register
	setFlag(Z, yreg == 0);             // set the zero flag if the y register is zero
	setFlag(N, yreg & 0x80);           // set the overflow flag if the MSB is high
	return 0;
}

// STORE INSTURCTIONS

// Store Accumulator: stores accumulator contents into a memory address
uint8_t cpu6502::STA() {
	write(addr_abs, acc);
	return 0;
}

// Store X: stores xreg contents into a memory address
uint8_t cpu6502::STX() {
	write(addr_abs, xreg);
	return 0;
}

// Store Y: stores yreg contents into a memory address
uint8_t cpu6502::STY() {
	write(addr_abs, yreg);
	return 0;
}

// TRANSFER INSTURCTIONS

// Transfer Accumulator to X: transfers the contents of the accumulator to the xreg
uint8_t cpu6502::TAX() {
	xreg = acc;
	setFlag(Z, xreg == 0);             // set the zero flag if the x register is zero
	setFlag(N, xreg & 0x80);           // set the overflow flag if the MSB is high
	return 0;
}

// Transfer Accumulator to Y: transfers the contents of the accumulator to the yreg
uint8_t cpu6502::TAY() {
	yreg = acc;
	setFlag(Z, yreg == 0);             // set the zero flag if the y register is zero
	setFlag(N, yreg & 0x80);           // set the overflow flag if the MSB is high
	return 0;
}

// Transfer X to Accumulator: transfers the contents of the xreg to the accumulator
uint8_t cpu6502::TXA() {
	acc = xreg;
	setFlag(Z, acc == 0);             // set the zero flag if the accumulator is zero
	setFlag(N, acc & 0x80);           // set the overflow flag if the MSB is high
	return 0;
}

// Transfer Y to Accumulator: transfers the contents of the yreg to the accumulator
uint8_t cpu6502::TYA() {
	acc = yreg;
	setFlag(Z, acc == 0);             // set the zero flag if the accumulator is zero
	setFlag(N, acc & 0x80);           // set the overflow flag if the MSB is high
	return 0;
}

// Transfer Stack Pointer to X: transfers the contents of the stack pointer to the xreg
uint8_t cpu6502::TSX() {
	xreg = stkp;
	setFlag(Z, xreg == 0);
	setFlag(N, xreg & 0x80);
	return 0;
}

// Transfer X to Stack Pointer: transfers the contents of the accumulator to the xreg
uint8_t cpu6502::TXS() {
	stkp = xreg;
	return 0;
}

// BRANCH INSTURCTIONS

void cpu6502::_branch() {
	cycles++;
	addr_abs = pc + addr_rel;

	if ((addr_abs & 0xFF00) != (pc & 0xFF00))
		cycles++;

	pc = addr_abs;
}

// Branch if Carry Set 
uint8_t cpu6502::BCS() {
	if (getFlag(C) == 1) 
		_branch();
	return 0;
}

// Branch if Carry Clear
uint8_t cpu6502::BCC() {
	if (getFlag(C) == 0) 
		_branch();
	return 0;
}

// Branch if Equal
uint8_t cpu6502::BEQ() {
	if (getFlag(Z) == 1) 
		_branch();
	return 0;
}

// Branch if Not Equal
uint8_t cpu6502::BNE() {
	if (getFlag(Z) == 0)
		_branch();
	return 0;
}

// Branch if Overflow Set
uint8_t cpu6502::BVS() {
	if (getFlag(V) == 1)
		_branch();
	return 0;
}

// Branch if Overflow Clear
uint8_t cpu6502::BVC() {
	if (getFlag(V) == 0)
		_branch();
	return 0;
}

// Branch if Minus
uint8_t cpu6502::BMI() {
	if (getFlag(N) == 1)
		_branch();
	return 0;
}

// Branch if Positive
uint8_t cpu6502::BPL() {
	if (getFlag(N) == 0)
		_branch();
	return 0;
}

// Break
uint8_t cpu6502::BRK() {
	// I commented this out because reading the opcode + IMM addressing mode already advance the pc once each,
	// so there shouldn't be a need to advance it again in the instruction itself 
	// (this was tested against https://github.com/Klaus2m5/6502_65C02_functional_tests/blob/master/6502_functional_test.a65)
	// (commenting this line causes the test it would otherwise fail in to pass, checking manually verifies the pushed address to be 1 higher than it should be)
	
	//pc++;                     // from the wiki "Although BRK only uses 1 byte, its return address skips the following byte."
	                            // "Because BRK skips the following byte, it is often considered a 2-byte instruction."
	
	push((pc >> 8) & 0x00FF);
	push(pc & 0x00FF);

	push(stat | B | U);
	setFlag(I, 1);

	uint16_t lo = read(0xFFFE);
	uint16_t hi = read(0xFFFF);
	pc = (hi << 8) | lo;

	return 0;
}

// Return From Interrupt
uint8_t cpu6502::RTI() {

	// Retrieve the Status Register
	stat = pop();
	setFlag(U, 1);
	setFlag(B, 0);

	// Retrieve the Program Counter
	uint16_t lo = pop();
	uint16_t hi = pop();

	pc = (hi << 8) | lo;
	return 0;
}

// Jump: Sets the program counter to the address specified by the operand
uint8_t cpu6502::JMP() {
	pc = addr_abs; // set the program counter as the address
	return 0;
}

// Jump to Subroutine
uint8_t cpu6502::JSR() {
	pc--;                    // ABS already advances the pc twice,
	                         // but JSR should push the address of its last byte since RTS advances it by one thus landing on the next instruction 
	push((pc >> 8) & 0x00FF);
	push(pc & 0x00FF);

	pc = addr_abs;
	return 0;
}

// Return from Subroutine
uint8_t cpu6502::RTS() {
	uint16_t lo = pop();
	uint16_t hi = pop();

	pc = (hi << 8) | lo;
	pc++;
	return 0;
}

// FLAG SET/CLEAR INSTRUCTIONS

uint8_t cpu6502::SEC() {
	setFlag(C, 1);
	return 0;
}

// Clear Carry Flag
uint8_t cpu6502::CLC() {
	setFlag(C, 0);
	return 0;
}

uint8_t cpu6502::SEI() {
	setFlag(I, 1);
	return 0;
}

// Clear Interrupt Disable Flag
uint8_t cpu6502::CLI() {
	setFlag(I, 0);
	return 0;
}

uint8_t cpu6502::SED() {
	setFlag(D, 1);
	return 0;
}

// Clear Decimal Mode Flag
uint8_t cpu6502::CLD() {
	setFlag(D, 0);
	return 0;
}

// Clear Overflow Flag
uint8_t cpu6502::CLV() {
	setFlag(V, 0);
	return 0;
}

// MATH OPERATIONS

// Add with Carry
uint8_t cpu6502::ADC() {
	uint16_t result = (uint16_t)acc + (uint16_t)fetch() + (uint16_t)getFlag(C);

	setFlag(C, result > 255);
	setFlag(Z, (result & 0x00FF) == 0);
	setFlag(N, result & 0x80);

	// (V = A'M'R + AMR'), could also be (V = (A^R) & (A^M)')
	setFlag(V, ((~((uint16_t)acc ^ (uint16_t)fetched) & ((uint16_t)acc ^ result))) & 0x0080);
	acc = result & 0x00FF;
	return 1;
}


// Subtract with Carry
uint8_t cpu6502::SBC() {
	uint16_t inverted = (uint16_t)fetch() ^ 0x00FF;

	uint16_t result = (uint16_t)acc + inverted + (uint16_t)getFlag(C);

	setFlag(C, result > 255);
	setFlag(Z, (result & 0x00FF) == 0);
	setFlag(N, result & 0x80);

	// (V = A'MR + AM'R'), could also be (V = (A^R) & (A^M')')
	setFlag(V, ((~((uint16_t)acc ^ (uint16_t)inverted) & ((uint16_t)acc ^ result))) & 0x0080);
	acc = result & 0x00FF;
	return 1;
}

// Compare Accumulator
uint8_t cpu6502::CMP() {
	uint8_t result = acc - fetch();
	setFlag(C, acc >= fetched);
	setFlag(Z, result == 0x00);
	setFlag(N, result & 0x80);
	return 1;
}

// Compare X 
uint8_t cpu6502::CPX() {
	uint8_t result = xreg - fetch();
	setFlag(C, xreg >= fetched);
	setFlag(Z, result == 0x00);
	setFlag(N, result & 0x80);
	return 1;
}

// Compare Y
uint8_t cpu6502::CPY() {
	uint8_t result = yreg - fetch();
	setFlag(C, yreg >= fetched);
	setFlag(Z, result == 0x00);
	setFlag(N, result & 0x80);
	return 1;
}

// Decrement Memory
uint8_t cpu6502::DEC() {
	uint8_t result = fetch() - 1;
	setFlag(Z, result == 0x00);
	setFlag(N, result & 0x80);
	write(addr_abs, result);
	return 0;
}

// Decrement X
uint8_t cpu6502::DEX() {
	xreg--;
	setFlag(Z, xreg == 0x00);
	setFlag(N, xreg & 0x80);
	return 0;
}

// Decrement Y
uint8_t cpu6502::DEY() {
	yreg--;
	setFlag(Z, yreg == 0x00);
	setFlag(N, yreg & 0x80);
	return 0;
}

// Increment Memory
uint8_t cpu6502::INC() {
	fetch();
	fetched++;
	setFlag(Z, fetched == 0x00);
	setFlag(N, fetched & 0x80);
	write(addr_abs, fetched);
	return 0;
}

// Increment X
uint8_t cpu6502::INX() {
	xreg++;
	setFlag(Z, xreg == 0x00);
	setFlag(N, xreg & 0x80);
	return 0;
}

// Increment Y
uint8_t cpu6502::INY() {
	yreg++;
	setFlag(Z, yreg == 0x00);
	setFlag(N, yreg & 0x80);
	return 0;
}

// LOGICAL OPERATIONS

// Bitwise AND
uint8_t cpu6502::AND() {
	acc = acc & fetch();
	setFlag(Z, acc == 0x00);
	setFlag(N, acc & 0x80);
	return 1;
}

// Bitwise OR
uint8_t cpu6502::ORA() {
	acc = acc | fetch();
	setFlag(Z, acc == 0x00);
	setFlag(N, acc & 0x80);
	return 1;
}

// Bitwise XOR
uint8_t cpu6502::EOR() {
	acc = acc ^ fetch();
	setFlag(Z, acc == 0x00);
	setFlag(N, acc & 0x80);
	return 1;
}

// Arithmetic Shift Left
uint8_t cpu6502::ASL() {

	uint8_t result = fetch() << 1;
	setFlag(C, fetched & 0x80);
	setFlag(Z, result == 0x00);
	setFlag(N, result & 0x80);
	if (lookup[opcode].addrmode == &cpu6502::IMP) {
		acc = result;
	}
	else {
		write(addr_abs, result);
	}
	return 0;
}

// Logical Shift Right
uint8_t cpu6502::LSR() {
	uint8_t initial;
	uint8_t result;
	if (lookup[opcode].addrmode == &cpu6502::IMP) {
		initial = acc;
		result = initial >> 1;
		acc = result;
	}
	else {
		initial = fetch();
		result = initial >> 1;
		write(addr_abs, result);
	}
	setFlag(C, initial & 0x01);
	setFlag(Z, result == 0x00);
	setFlag(N, result & 0x80);
	return 0;
}

uint8_t cpu6502::ROL() {
	uint8_t initial;
	uint8_t result;
	if (lookup[opcode].addrmode == &cpu6502::IMP) {
		initial = acc;
		result = (acc << 1) + (getFlag(C) << 0);
		acc = result;
	}
	else {
		initial = fetch();
		result = (fetched << 1) + (getFlag(C) << 0);
		write(addr_abs, result);
	}
	setFlag(C, initial & 0x80);
	setFlag(Z, result == 0x00);
	setFlag(N, result & 0x80);
	return 0;
}

uint8_t cpu6502::ROR() {
	uint8_t initial;
	uint8_t result;
	if (lookup[opcode].addrmode == &cpu6502::IMP) {
		initial = acc;
		result = (acc >> 1) + (getFlag(C) << 7);
		acc = result;
	}
	else {
		initial = fetch();
		result = (fetched >> 1) + (getFlag(C) << 7);
		write(addr_abs, result);
	}
	setFlag(C, initial & 0x01);
	setFlag(Z, result == 0x00);
	setFlag(N, result & 0x80);
	return 0;
}

// Bit Compare
uint8_t cpu6502::BIT() {
	uint8_t result = acc & fetch();
	setFlag(Z, result == 0x00);
	setFlag(V, fetched & 0x40);
	setFlag(N, fetched & 0x80);
	return 0;
}

// STACK OPERATIONS

// Push Accumulator
uint8_t cpu6502::PHA() {
	push(acc);
	return 0;
}

// Push Processor Status
uint8_t cpu6502::PHP() {
	push(stat | B | U);
	return 0;
}

// Pull Accumulator
uint8_t cpu6502::PLA() {
	acc = pop();
	setFlag(Z, acc == 0x00);
	setFlag(N, acc & 0x80);
	return 0;
}

// Pull Processor Status
uint8_t cpu6502::PLP() {
	uint8_t newStat = pop();
	setFlag(C, newStat & C);
	setFlag(Z, newStat & Z);
	setFlag(I, newStat & I); // TODO: should be delayed until one instruction later
	setFlag(D, newStat & D);
	setFlag(V, newStat & V);
	setFlag(N, newStat & N);
	return 0;
}

// No Operation
uint8_t cpu6502::NOP() {
	return 0;
}

// Placeholder for illegal instructions
uint8_t cpu6502::XXX() { 
	return 0; 
}


std::map<uint16_t, std::string> cpu6502::disassemble(uint16_t startAddr, uint16_t endAddr) {
	std::map<uint16_t, std::string> lines;
	
	uint16_t currentAddr = startAddr;
	std::string instr = "";

	auto hex = [](int num) {
		return "0123456789ABCDEF"[num];
	};

	while (currentAddr <= endAddr) {
		uint16_t lineAddr = currentAddr;

		instr = "#";
		instr += hex(lineAddr >> 12 & 0x000F);
		instr += hex(lineAddr >> 8 & 0x000F);
		instr += hex(lineAddr >> 4 & 0x000F);
		instr += hex(lineAddr >> 0 & 0x000F);
	
		uint8_t readOpcode = bus->cpuRead(currentAddr);
		currentAddr++;
		instr += " " + lookup[readOpcode].name;
		if (lookup[readOpcode].addrmode == &cpu6502::IMP) {
			instr += " {IMP}";
		}
		else if (lookup[readOpcode].addrmode == &cpu6502::IMM) {
			instr += " #$";
			uint8_t operand = bus->cpuRead(currentAddr);
			currentAddr++;
			instr += hex(operand>>4 & 0x000F);
			instr += hex(operand>>0 & 0x000F);
			instr += " {IMM}";
		}
		else if (lookup[readOpcode].addrmode == &cpu6502::ZP0) {
			uint8_t operand = bus->cpuRead(currentAddr);
			currentAddr++;
			instr += " $";
			instr += hex(operand >> 4 & 0x000F);
			instr += hex(operand >> 0 & 0x000F);
			instr += " {ZP0}";
		}
		else if (lookup[readOpcode].addrmode == &cpu6502::ZPX) {
			uint8_t operand = bus->cpuRead(currentAddr);
			currentAddr++;
			instr += " $";
			instr += hex(operand >> 4 & 0x000F);
			instr += hex(operand >> 0 & 0x000F);
			instr += ", X {ZPX}";
		}
		else if (lookup[readOpcode].addrmode == &cpu6502::ZPY) {
			uint8_t operand = bus->cpuRead(currentAddr);
			currentAddr++;
			instr += " $";
			instr += hex(operand >> 4 & 0x000F);
			instr += hex(operand >> 0 & 0x000F);
			instr += ", Y {ZPY}";
		}
		else if (lookup[readOpcode].addrmode == &cpu6502::IND) {
			uint16_t lo = bus->cpuRead(currentAddr);
			currentAddr++;
			uint16_t hi = bus->cpuRead(currentAddr);
			currentAddr++;
			uint16_t addr = (hi << 8) | lo;
			instr += " ($";
			instr += hex(addr >> 12 & 0x000F);
			instr += hex(addr >> 8 & 0x000F);
			instr += hex(addr >> 4 & 0x000F);
			instr += hex(addr >> 0 & 0x000F);
			instr += ") {IND}";
		}
		else if (lookup[readOpcode].addrmode == &cpu6502::IZX) {
			uint8_t operand = bus->cpuRead(currentAddr);
			currentAddr++;
			instr += " ($";
			instr += hex(operand >> 4 & 0x000F);
			instr += hex(operand >> 0 & 0x000F);
			instr += ", X) {IZX}";
		}
		else if (lookup[readOpcode].addrmode == &cpu6502::IZY) {
			uint8_t operand = bus->cpuRead(currentAddr);
			currentAddr++;
			instr += " ($";
			instr += hex(operand >> 4 & 0x000F);
			instr += hex(operand >> 0 & 0x000F);
			instr += "), Y {IZY}";
		}
		else if (lookup[readOpcode].addrmode == &cpu6502::ABS) {
			uint16_t lo = bus->cpuRead(currentAddr);
			currentAddr++;
			uint16_t hi = bus->cpuRead(currentAddr);
			currentAddr++;
			uint16_t addr = (hi << 8) | lo;
			instr += " $";
			instr += hex(addr >> 12 & 0x000F);
			instr += hex(addr >> 8 & 0x000F);
			instr += hex(addr >> 4 & 0x000F);
			instr += hex(addr >> 0 & 0x000F);
			instr += " {ABS}";
		}
		else if (lookup[readOpcode].addrmode == &cpu6502::ABX) {
			uint16_t lo = bus->cpuRead(currentAddr);
			currentAddr++;
			uint16_t hi = bus->cpuRead(currentAddr);
			currentAddr++;
			uint16_t addr = (hi << 8) | lo;
			instr += " $";
			instr += hex(addr >> 12 & 0x000F);
			instr += hex(addr >> 8 & 0x000F);
			instr += hex(addr >> 4 & 0x000F);
			instr += hex(addr >> 0 & 0x000F);
			instr += ", X {ABX}";
		}
		else if (lookup[readOpcode].addrmode == &cpu6502::ABY) {
			uint16_t lo = bus->cpuRead(currentAddr);
			currentAddr++;
			uint16_t hi = bus->cpuRead(currentAddr);
			currentAddr++;
			uint16_t addr = (hi << 8) | lo;
			instr += " $";
			instr += hex(addr >> 12 & 0x000F);
			instr += hex(addr >> 8 & 0x000F);
			instr += hex(addr >> 4 & 0x000F);
			instr += hex(addr >> 0 & 0x000F);
			instr += ", Y {ABY}";
		}
		else if (lookup[readOpcode].addrmode == &cpu6502::REL) {
			uint8_t operand = bus->cpuRead(currentAddr);
			currentAddr++;
			instr += " $";
			instr += hex(operand >> 4 & 0x000F);
			instr += hex(operand >> 0 & 0x000F);
			uint16_t addr = currentAddr + (int8_t)operand; // cast to int8_t for negative offsets
			instr += " [$";
			instr += hex(addr >> 12 & 0x000F);
			instr += hex(addr >> 8 & 0x000F);
			instr += hex(addr >> 4 & 0x000F);
			instr += hex(addr >> 0 & 0x000F);
			instr += "] {REL}";
		}

		lines[lineAddr] = instr;
	}
	return lines;
}