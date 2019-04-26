// Simpletron.cpp : Este arquivo contém a função 'main'. A execução do programa começa e termina ali.
//
#include "Simpletron.h"


void Simpletron::iniInstructionHandlers() {
	handlers[OpCode::READ] = &Simpletron::opRead;
	handlers[OpCode::WRITE] = &Simpletron::opWrite;
	handlers[OpCode::LOAD] = &Simpletron::opLoad;
	handlers[OpCode::STORE] = &Simpletron::opStore;
	handlers[OpCode::ADD] = &Simpletron::opAdd;
	handlers[OpCode::SUBTRACT] = &Simpletron::opSubtract;
	handlers[OpCode::DIVIDE] = &Simpletron::opDivide;
	handlers[OpCode::MULTIPLY] = &Simpletron::opMultiply;
	handlers[OpCode::BRANCH] = &Simpletron::opBranch;
	handlers[OpCode::BRANCHNEG] = &Simpletron::opBranchNeg;
	handlers[OpCode::BRACHZERO] = &Simpletron::opBranchZero;
	handlers[OpCode::HALT] = &Simpletron::opHalt;
}

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

Simpletron::Simpletron() {
	iniInstructionHandlers();
}


void Simpletron::dump() {
	cout << "\nREGISTRADORES:" << endl;
	cout << setw(6) << reg.accumulator << " accumulator" << endl;
	cout << setw(6) << reg.counter << setfill(' ') << " counter" << endl;
	cout << setw(6) << reg.instruction << " instructionRegister" << endl;
	cout << setw(6) << reg.opCode << " operationCode" << endl;
	cout << setw(6) << reg.operand << " operand" << endl << endl;
	cout << "MEMORIA:" << endl << endl;

	cout << setw(2) << "";
	for (int i = 0; i < 10; i++)
		cout << setw(7) << i;
	cout << endl << endl;

	for (int row = 0; row < 10; row++) {
		cout << setw(2) << row * 10;
		for (int col = 0; col < 10; col++) {
			cout << setw(7) << memory[(row * 10) + col];
		}
		cout << endl;
	}

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
	cout << memory[operand];
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