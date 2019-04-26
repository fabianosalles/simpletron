// Simpletron.cpp : Este arquivo contém a função 'main'. A execução do programa começa e termina ali.
//
#include "Simpletron.h"



void Simpletron::load(const vector<short> program) {
	for (auto i = 0; i < (int)program.size(); i++)
		memory[i] = program[i];
}

void Simpletron::execute() {
	reset();
	while (++reg.counter < MEM_SIZE) {

		//1. fetch
		reg.instruction = memory[reg.counter];

		//2. decode
		reg.opCode = reg.instruction / 100;
		reg.operand = reg.instruction % 100;
		bool(Simpletron::*handler)(short) = handlers[reg.opCode];

		//3. execute
		if ((this->*handler)(reg.operand)) {


		}
		else
			break;
	}
}

void Simpletron::run(const vector<short> program) {
	cout << "Loading program..." << endl;
	load(program);
	cout << "Running..." << endl;
	execute();
	cout << endl;
	dump();
}

bool Simpletron::parse(const vector<short> program) {
	cout << "Parsing... " << endl;
	if (program.size() > MEM_SIZE) {
		cout << "Program too large to fit in memory." << endl;
		return false;
	}

	char opCode, operand;
	short instruction;

	for (char i = 0; i < (char)program.size(); i++) {
		instruction = program[i];
		opCode = instruction / 100;
		operand = instruction % 100;

		if (instruction == 0) {
			cout << "Invalid instruction." << endl;
			return false;
		}

		auto handler = handlers.find(opCode);
		if (handler == handlers.end()) {
			cout << "Invalid instruction : " << instruction << endl;
			return false;
		}
		else if (handlers[opCode] == nullptr) {
			cout << "Instruction not implemented yet: " << (int)opCode << endl;
			return false;
		}
	}
	cout << "Program is valid." << endl;;
	return true;
}

void Simpletron::dumpRegisters() const {
	cout << "Registers:" << endl;
	cout << "  accumulator         : " << showpos << setfill('0') << setw(5) << internal << reg.accumulator << endl;
	cout << "  instructionCounter  : " << noshowpos << setfill(' ') << setw(5) << internal << reg.counter << endl;
	cout << "  instructionRegister : " << showpos << setfill('0') << setw(5) << internal << reg.instruction << endl;
	cout << "  operationCode       : " << noshowpos << setfill(' ') << setw(5) << internal << reg.opCode << endl;
	cout << "  operand             : " << noshowpos << setfill(' ') << setw(5) << internal << reg.operand << endl;	
}

void Simpletron::dumpMemory() const {
	cout << "Memory\n       0     1     2     3     4     5     6     7     8     9" << endl;
	for (int i = 0; i < 100; ++i){
		if (i % 10 == 0){
			if (i == 0)
				cout << " ";			
			cout << noshowpos << i << " ";
		}
		cout << showpos << setfill('0') << setw(5) << internal << memory[i] << " ";
		if ((i + 1) % 10 == 0) 
			cout << endl;
	}
}

void Simpletron::dump() const {
	dumpRegisters();
	dumpMemory();
}


void Simpletron::reset() {
	reg.accumulator = 0;
	reg.counter = -1;
	reg.instruction = 0;
	reg.operand = 0;
	reg.opCode = 0;
}


bool Simpletron::opRead(const short operand) {
	cin >> memory[operand];
	return true;
}

bool Simpletron::opWrite(const short operand) {
	cout << memory[operand] << endl;
	return true;
}

bool Simpletron::opLoad(const short operand) {
	reg.accumulator = memory[operand];
	return true;
}

bool Simpletron::opStore(const short operand) {
	memory[operand] = reg.accumulator;
	return true;
}

bool Simpletron::opAdd(const short operand) {
	reg.accumulator += memory[operand];
	return true;
}

bool Simpletron::opSubtract(const short operand) {
	reg.accumulator -= memory[operand];
	return true;
}

bool Simpletron::opDivide(const short operand) {
	reg.accumulator /= memory[operand];
	return true;
}

bool Simpletron::opMultiply(const short operand) {
	reg.accumulator *= memory[operand];
	return true;
}

bool Simpletron::opBranch(const short operand) {
	reg.counter = operand;
	return true;
}

bool Simpletron::opBranchNeg(const short operand) {
	if (reg.accumulator < 0)
		reg.counter = operand;
	return true;
}

bool Simpletron::opBranchZero(const short operand) {
	if (reg.accumulator == 0)
		reg.counter = operand;
	return true;
}

bool Simpletron::opHalt(const short memPosition) {
	reg.counter = MEM_SIZE - 1;
	return false;
}