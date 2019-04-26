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


class Simpletron {
private:
	int memory[MEM_SIZE] = { 0 };
	regs reg = { 0 };
	map<short, bool(Simpletron::*)(short)> handlers = {
		{OpCode::READ, &Simpletron::opRead},
		{OpCode::READ, &Simpletron::opRead},
		{OpCode::WRITE, &Simpletron::opWrite},
		{OpCode::LOAD, &Simpletron::opLoad},
		{OpCode::STORE, &Simpletron::opStore},
		{OpCode::ADD, &Simpletron::opAdd},
		{OpCode::SUBTRACT, &Simpletron::opSubtract},
		{OpCode::DIVIDE, &Simpletron::opDivide},
		{OpCode::MULTIPLY, &Simpletron::opMultiply},
		{OpCode::BRANCH, &Simpletron::opBranch},
		{OpCode::BRANCHNEG, &Simpletron::opBranchNeg},
		{OpCode::BRACHZERO, &Simpletron::opBranchZero},
		{OpCode::HALT, &Simpletron::opHalt}
	};
	
	bool opRead(const short operand);
	bool opWrite(const short operand);	
	bool opLoad(const short operand);
	bool opStore(const short operand);
	bool opAdd(const short operand);
	bool opSubtract(const short operand);
	bool opDivide(const short operand);
	bool opMultiply(const short operand);
	bool opBranch(const short operand);
	bool opBranchNeg(const short operand);
	bool opBranchZero(const short operand);
	bool opHalt(const short memPosition);		

	void load(const vector<short> program);
	void reset();
	void execute();
	void dumpRegisters() const;
	void dumpMemory() const;

public:

	enum OpCode {	
		READ = 10,
		WRITE = 11,
		LOAD = 20,
		STORE = 21,
		ADD = 30,
		SUBTRACT = 31,
		DIVIDE = 32,
		MULTIPLY = 33,
		BRANCH = 40,
		BRANCHNEG = 41,
		BRACHZERO = 42,
		HALT = 43
	};	
	
	bool parse(const vector<short> program);
	void dump() const;
	void run(const vector<short> program);
};

