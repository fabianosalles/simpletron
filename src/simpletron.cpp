#include "simpletron.h"
#include <fstream>


ErrorCode 
Simpletron::load(const vector<short>& program) {

	if (program.size() > MEM_SIZE)
		return raiseError(ErrorCode::NOT_ENOUGTH_MEMORY);

	state = State::LOADING;
	for (auto i = 0; i < program.size(); i++)
		memory[i] = program[i];
	state = State::READY;
	return ErrorCode::SUCCESS;
}


void Simpletron::execute() {
	state = State::RUNNING;
	while ( state == State::RUNNING && regs.counter < MEM_SIZE) {

		//1. fetch
		regs.instruction = memory[regs.counter++];		

		//2. decode
		regs.opCode = static_cast<OpCode>(regs.instruction / 100);
		regs.operand = static_cast<uint16_t>(regs.instruction % 100);

		if (!executeInstruction()) {
			break;
		}
	}
}

bool Simpletron::executeInstruction() {
    switch (regs.opCode) {
		case OpCode::NOOP:      return true;
        case OpCode::READ:      return opRead();
        case OpCode::WRITE:     return opWrite();
        case OpCode::LOAD:      return opLoad();
        case OpCode::STORE:     return opStore();
        case OpCode::ADD:       return opAdd();
        case OpCode::SUBTRACT:  return opSubtract();
        case OpCode::DIVIDE:    return opDivide();
        case OpCode::MULTIPLY:  return opMultiply();
        case OpCode::BRANCH:    return opBranch();
        case OpCode::BRANCHNEG: return opBranchNeg();
        case OpCode::BRACHZERO: return opBranchZero();
        case OpCode::HALT:      return opHalt();
        default:
            raiseError(ErrorCode::INVALID_INSTRUCTION);
            return false;
    }
}

void Simpletron::run(const vector<short>& program) {
	cout << "Loading program into memory... ";
	switch (load(program) ) {
		case ErrorCode::NOT_ENOUGTH_MEMORY:
			cout << "Error: Not enough memory to load program." << endl;
			return;
		case ErrorCode::INVALID_INSTRUCTION:
			cout << "Error: Invalid instruction in program." << endl;
			return;
		case ErrorCode::SUCCESS:
			cout << "Success." << endl;
			break;
		default:
			cout << "Error." << endl;
			return;
	}

	cout << "Running..." << endl;

	execute();
	cout << endl << "*** Program finished ***" << endl;
	dump();
}

bool Simpletron::isValidOpCode(OpCode op) noexcept {
	switch (op) {
	case OpCode::NOOP:
	case OpCode::READ:
	case OpCode::WRITE:
	case OpCode::LOAD:
	case OpCode::STORE:
	case OpCode::ADD:
	case OpCode::SUBTRACT:
	case OpCode::DIVIDE:
	case OpCode::MULTIPLY:
	case OpCode::BRANCH:
	case OpCode::BRANCHNEG:
	case OpCode::BRACHZERO:
	case OpCode::HALT:
		return true;
	default:
		return false;
	}
}

bool Simpletron::parse(const vector<short>& program) {
	cout << "Parsing... " << endl;
	if (program.size() > MEM_SIZE) {
		cout << "Program too large to fit in memory." << endl;
		return false;
	}
		
	for (size_t i = 0; i < program.size(); i++) {
		const int16_t instruction = program[i];
		const auto opCode = static_cast<OpCode>(instruction / 100);
		const auto operand = static_cast<uint16_t>(instruction % 100);

		if (operand >= MEM_SIZE) {
			std::cout << "Invalid operand address at index " << i << ": " << operand << "\n";
			return false;
		}

		if (!isValidOpCode(opCode)) {
			cout << "Invalid instruction : " << instruction << endl;
			return false;
		}		
	}

	cout << "Program is valid." << endl;;
	return true;
}

void Simpletron::dumpRegisters() const {
	cout << "Registers:" << endl;
	cout << "  accumulator         : " << showpos << setfill('0') << setw(5) << internal << regs.accumulator << endl;
	cout << "  instructionCounter  : " << noshowpos << setfill(' ') << setw(5) << internal << regs.counter << endl;
	cout << "  instructionRegister : " << showpos << setfill('0') << setw(5) << internal << regs.instruction << endl;
	cout << "  operationCode       : " << noshowpos << setfill(' ') << setw(5) << internal << static_cast<uint8_t>(regs.opCode) << endl;
	cout << "  operand             : " << noshowpos << setfill(' ') << setw(5) << internal << regs.operand << endl;	
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
	cout <<"* -------------------------------------------------------------------- *" << endl
		<< "|                      Welcome to Simpletron!                          |" << endl
		<< "* -------------------------------------------------------------------- *" << endl
		<< "| Please enter your program one instruction (or data word) at a time.  |" << endl
		<< "| I will type the location number and a question mark(?).              |" << endl
		<< "| You then type the word for that location.                            |" << endl
		<< "| Type the sentinel -9999 to stop entering your program.               | " << endl
		<< "* -------------------------------------------------------------------- *" << endl;
}

void Simpletron::dump() const {
	dumpRegisters();
	dumpMemory();
}


void Simpletron::reset() {
	regs.accumulator = 0;
	regs.counter = 0;
	regs.instruction = 0;
	regs.operand = 0;
	regs.opCode = OpCode::HALT;
	state = State::READY;
}


bool Simpletron::opRead() {
	int value = 0;
	cout << " ? ";
	if (!(cin >> value)) {
		cin.clear();
		return false;
	}
	memory[regs.operand] = static_cast<int16_t>(value);
	return true;
}

bool Simpletron::opWrite() {
	cout << setw(5) << setfill('0') << showpos << internal << memory[regs.operand] << endl;
	return true;
}

bool Simpletron::opLoad() {
	regs.accumulator = memory[regs.operand];
	return true;
}

bool Simpletron::opStore() {
	memory[regs.operand] = regs.accumulator;
	return true;
}

bool Simpletron::opAdd() {
	regs.accumulator += memory[regs.operand];
	return true;
}

bool Simpletron::opSubtract() {
	regs.accumulator -= memory[regs.operand];
	return true;
}

bool Simpletron::opDivide() {
	if (memory[regs.operand] == 0) {
		raiseError(ErrorCode::DIVISION_BY_ZERO);		
		return false;
	}

	regs.accumulator /= memory[regs.operand];
	return true;
}

bool Simpletron::opMultiply() {
	regs.accumulator *= memory[regs.operand];
	return true;
}

bool Simpletron::opBranch() {
	regs.counter = regs.operand;
	return true;
}

bool Simpletron::opBranchNeg() {
	if (regs.accumulator < 0)
		regs.counter = regs.operand;
	return true;
}

bool Simpletron::opBranchZero() {
	if (regs.accumulator == 0)
		regs.counter = regs.operand;
	return true;
}

bool Simpletron::opHalt() {	
	state = State::HALTED;
	return true;
}


vector<short> Simpletron::readProgram() const {
	printInteractiveMenu();
	short i = 0;
	short instruction;
	auto *program = new vector<short>();
	do {
		cout << "   " << setw(4) << setfill('0') << noshowpos << i << " ? ";
		cin >> instruction;
		if (instruction != -9999) {
			program->push_back(instruction);
			i++;
		}		
	} while (instruction != -9999);
	return *program;
}


vector<short> Simpletron::readFromFile(const string& fileName) {

	ifstream file(fileName, ifstream::in);
	if (!file) {
		cout << "Could not open file '" << fileName << "'" << endl;
		return {};
	}

	vector<short> program;
	short instruction;
	while (file >> instruction){
		program.push_back(instruction);
	}
	file.close();
	return program;
}