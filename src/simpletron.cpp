#include "simpletron.h"
#include <fstream>
#include <format>


ErrorCode Simpletron::raiseError(ErrorCode code) noexcept {	
	errorCode = static_cast<uint8_t>(code);
	setState(State::ERROR);
	event_bus.publish(ExectionEvent{ code });
	return code;
}

ErrorCode Simpletron::load(const vector<short>& program) {	

	if (program.size() > MEM_SIZE)
		return raiseError(ErrorCode::NOT_ENOUGTH_MEMORY);

	setState(State::LOADING);	
	for (auto i = 0; i < program.size(); i++)
		memory[i] = program[i];

	setState(State::READY);		
	event_bus.publish(ProgramLoadedEvent{ program.size() });

	return ErrorCode::NONE;
}


void Simpletron::execute() {
	setState(State::RUNNING);
	while ( state == State::RUNNING && regs.counter < MEM_SIZE) {

		//1. fetch
		regs.instruction = memory[regs.counter++];		

		//2. decode
		regs.opCode = static_cast<OpCode>(regs.instruction / 100);
		regs.operand = static_cast<uint16_t>(regs.instruction % 100);

		auto result = executeInstruction();
		if (result != ErrorCode::NONE) {
			break;
		}
	}
}

ErrorCode Simpletron::executeInstruction() {
	switch (regs.opCode) {
		case OpCode::NOOP:      return ErrorCode::NONE;
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
			return raiseError(ErrorCode::INVALID_INSTRUCTION);
	}
}


ErrorCode Simpletron::run(const vector<short>& program) {
	
	auto result = load(program);
	if (result != ErrorCode::NONE) 
		return result;	

	execute();
	//event_bus.publish(ExectionEvent{ ErrorCode::SUCCESS });
	cout << endl << "*** Program finished ***" << endl;
	dump();

	return ErrorCode::NONE;
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

void Simpletron::setState(State newState) noexcept
{
	if (state != newState) {
		auto oldState = state;
		state = newState;
		event_bus.publish(StateChangedEvent{ oldState, newState });
	}
}



bool Simpletron::parse(const vector<short>& program) {		
		
	if (program.size() > MEM_SIZE) {
		raiseError(ErrorCode::NOT_ENOUGTH_MEMORY);		
		return false;
	}
		
	setState(State::PARSING);
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
	setState(State::READY);
}


ErrorCode Simpletron::opRead() {
	int value = 0;
	cout << " ? ";
	if (!(cin >> value)) {
		cin.clear();
		return ErrorCode::NONE;
	}
	memory[regs.operand] = static_cast<int16_t>(value);
	return ErrorCode::NONE;
}

ErrorCode Simpletron::opWrite() {
	cout << setw(5) << setfill('0') << showpos << internal << memory[regs.operand] << endl;
	return ErrorCode::NONE;
}

ErrorCode Simpletron::opLoad() {
	regs.accumulator = memory[regs.operand];
	return ErrorCode::NONE;
}

ErrorCode Simpletron::opStore() {
	memory[regs.operand] = regs.accumulator;
	return ErrorCode::NONE;
}

ErrorCode Simpletron::opAdd() {
	regs.accumulator += memory[regs.operand];
	return ErrorCode::NONE;
}

ErrorCode Simpletron::opSubtract() {
	regs.accumulator -= memory[regs.operand];
	return ErrorCode::NONE;
}

ErrorCode Simpletron::opDivide() {
	if (memory[regs.operand] == 0) {
		return raiseError(ErrorCode::DIVISION_BY_ZERO);
	}

	regs.accumulator /= memory[regs.operand];
	return ErrorCode::NONE;
}

ErrorCode Simpletron::opMultiply() {
	regs.accumulator *= memory[regs.operand];
	return ErrorCode::NONE;
}

ErrorCode Simpletron::opBranch() {
	regs.counter = regs.operand;
	return ErrorCode::NONE;
}

ErrorCode Simpletron::opBranchNeg() {
	if (regs.accumulator < 0)
		regs.counter = regs.operand;
	return ErrorCode::NONE;
}

ErrorCode Simpletron::opBranchZero() {
	if (regs.accumulator == 0)
		regs.counter = regs.operand;
	return ErrorCode::NONE;
}

ErrorCode Simpletron::opHalt() {	
	state = State::HALTED;
	return ErrorCode::NONE;
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