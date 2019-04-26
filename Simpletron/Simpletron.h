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
	map<short, bool(Simpletron::*)(short)> handlers;
	
	void iniInstructionHandlers();
	void load(const vector<short> program);
	void reset();
	void execute();

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

	Simpletron();
	
	bool parse(const vector<short> program);
	void dump();
	void run(const vector<short> program);
};

