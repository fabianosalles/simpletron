// Simpletron.cpp : Este arquivo contém a função 'main'. A execução do programa começa e termina ali.
//
#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include <vector>
#include <cstring>


using namespace std;

#define MEM_SIZE 100

struct regs {	
	int counter;
	int instruction;
	short opCode;
	short operand;
	int accumulator;
};

#define SPT_SUCCESS 1
#define SPT_PROGRAM_TOO_LARGE 2
#define SPT_INVALID_INSTRUCTION 3

class Simpletron {
private:
	int memory[MEM_SIZE] = { 0 };
	regs reg = { 0 };
	map<char, bool(Simpletron::*)(short)> handlers;
	void initInstructionHandlers() {
		handlers[OpCode::NOOP] = &Simpletron::opNoOpHandler;
		handlers[OpCode::READ] = &Simpletron::opRead;
		handlers[OpCode::WRITE] = &Simpletron::opWrite;
		handlers[OpCode::LOAD] = &Simpletron::opLoadHandler;
		handlers[OpCode::STORE] = &Simpletron::opStoreHandler;
		handlers[OpCode::ADD] = &Simpletron::opAddHandler;
		handlers[OpCode::SUBTRACT] = &Simpletron::opSubtractHandler;
		handlers[OpCode::DIVIDE] = &Simpletron::opDivideHandler;
		handlers[OpCode::MULTIPLY] = &Simpletron::opMultiplyHandler;
		handlers[OpCode::BRANCH] = &Simpletron::opBranchHandler;
		handlers[OpCode::BRANCHNEG] = &Simpletron::opBranchNegHandler;
		handlers[OpCode::BRACHZERO] = &Simpletron::opBranchZeroHandler;
		handlers[OpCode::HALT] = &Simpletron::opHaltHandler;
	}

	bool opRead(const short operand) {
		cin >> memory[operand];
		return true;
	}

	bool opWrite(const short operand) {
		cout << memory[operand];
		return true;
	}

	bool opLoadHandler(const short operand) {
		reg.accumulator = memory[operand];
		return true;
	}

	bool opStoreHandler(const short operand) {
		memory[operand] = reg.accumulator;
		return true;
	}

	bool opAddHandler(const short operand) {
		reg.accumulator += memory[operand];
		return true;
	}

	bool opSubtractHandler(const short operand) {
		reg.accumulator -= memory[operand];
		return true;
	}

	bool opDivideHandler(const short operand) {
		reg.accumulator /= memory[operand];
		return true;
	}

	bool opMultiplyHandler(const short operand) {
		reg.accumulator *= memory[operand];
		return true;
	}

	bool opBranchHandler(const short operand) {
		reg.counter = operand;
		return true;
	}

	bool opBranchNegHandler(const short operand) {
		if (reg.accumulator < 0)
			reg.counter = operand;
		return true;
	}

	bool opBranchZeroHandler(const short operand) {
		if (reg.accumulator == 0)
			reg.counter = operand;
		return true;
	}

	bool opHaltHandler(const short memPosition) {		
		reg.counter = MEM_SIZE - 1;
		return false;
	}

	bool opNoOpHandler(const short memPosition) {		
		return false;
	}

	void load(const vector<short> program) {
		for (auto i = 0; i < (int)program.size(); i++)
			memory[i] = program[i];		
	}

	void reset() {
		reg.accumulator = 0;
		reg.counter = -1;
		reg.instruction = 0;
		reg.operand = 0;
		reg.opCode = 0;
	}

	void execute() {
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

public:	

	enum OpCode {
		NOOP		= 00,
		READ		= 10,
		WRITE		= 11,
		LOAD		= 20,
		STORE		= 21,
		ADD			= 30,
		SUBTRACT	= 31,
		DIVIDE		= 32,
		MULTIPLY	= 33,
		BRANCH		= 40,
		BRANCHNEG	= 41,
		BRACHZERO	= 42,
		HALT		= 43
	};

	Simpletron() {
		initInstructionHandlers();		
	}

	void run(const vector<short> program) {
		cout << "Loading program..." << endl;
		load(program);		
		cout << "Running..." << endl;
		execute();
		dump();
	}

	bool parse(const vector<short> program) {
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

	void dump() {
		cout << "\nREGISTRADORES:" << endl;
		cout << setw(6) << reg.accumulator << " accumulator" << endl;		
		cout << setw(6) << reg.counter << setfill(' ') <<" counter" << endl;
		cout << setw(6) << reg.instruction << " instructionRegister" << endl;
		cout << setw(6) << reg.opCode << " operationCode" <<  endl;
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
};

int main()
{
	Simpletron simpletron;	
	vector<short> program = {
	
		+1007,  // READ A ON 7st MEM POSITION
		+1008,  // READ B ON 8st MEM POSITION
		+2007,  // LOAD A
		+3008,  // ADD B
		+2109,  // STORE C
		+1109,  // WRITE C
		+4300   // HALT
	};
	
	if (simpletron.parse(program)) {
		simpletron.run(program);
	}
}
