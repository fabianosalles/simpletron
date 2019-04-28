// Simpletron.cpp : Este arquivo contém a função 'main'. A execução do programa começa e termina ali.
//
#include "Simpletron.h"
#include <fstream>


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
		bool(Simpletron::*handler)() = handlers[reg.opCode];

		//3. execute
		if (!(this->*handler)())
			break;
	}
}

void Simpletron::run(const vector<short> program) {
	cout << "Loading program into memory..." << endl;
	load(program);
	cout << "Running..." << endl;
	execute();
	cout << endl << "*** Program finished ***" << endl;
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

void Simpletron::printInteractiveMenu() const {
	cout << "*** ------------------------------------------------------------------- ***" << endl
		<< "***                      Welcome to Simpletron!                         ***" << endl
		<< "*** ------------------------------------------------------------------- ***" << endl
		<< "*** Please enter your program one instruction (or data word) at a time. ***" << endl
		<< "*** I will type the location number and a question mark(?).             ***" << endl
		<< "*** You then type the word for that location.                           ***" << endl
		<< "*** Type the sentinel -99999 to stop entering your program.             ***" << endl
		<< "*** ------------------------------------------------------------------- ***" << endl;
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


bool Simpletron::opRead() {
	cout << " ? ";
	cin >> memory[reg.operand];
	return true;
}

bool Simpletron::opWrite() {
	cout << setw(5) << setfill('0') << showpos << internal << memory[reg.operand] << endl;
	return true;
}

bool Simpletron::opLoad() {
	reg.accumulator = memory[reg.operand];
	return true;
}

bool Simpletron::opStore() {
	memory[reg.operand] = reg.accumulator;
	return true;
}

bool Simpletron::opAdd() {
	reg.accumulator += memory[reg.operand];
	return true;
}

bool Simpletron::opSubtract() {
	reg.accumulator -= memory[reg.operand];
	return true;
}

bool Simpletron::opDivide() {
	reg.accumulator /= memory[reg.operand];
	return true;
}

bool Simpletron::opMultiply() {
	reg.accumulator *= memory[reg.operand];
	return true;
}

bool Simpletron::opBranch() {
	reg.counter = reg.operand;
	return true;
}

bool Simpletron::opBranchNeg() {
	if (reg.accumulator < 0)
		reg.counter = reg.operand;
	return true;
}

bool Simpletron::opBranchZero() {
	if (reg.accumulator == 0)
		reg.counter = reg.operand;
	return true;
}

bool Simpletron::opHalt() {
	reg.counter = MEM_SIZE;
	return false;
}


vector<short> *Simpletron::readProgram() {	
	printInteractiveMenu();
	short i = 0;
	short instruction;
	vector<short> *program = new vector<short>();
	do {
		cout << "   " << setw(4) << setfill('0') << noshowpos << i << " ? ";
		cin >> instruction;
		if (instruction != -9999) {
			program->push_back(instruction);
			i++;
		}		
	} while (instruction != -9999);
	return program;
}


vector<short> *Simpletron::readFromFile(string fileName) {

	ifstream file(fileName, ifstream::in);
	if (!file) {
		cout << "Could not open file '" << fileName << "'" << endl;
		return nullptr;
	}

	vector<short> *program = new vector<short>;
	short instruction;
	while (file >> instruction){
		program->push_back(instruction);
	}
	file.close();
	return program;
}