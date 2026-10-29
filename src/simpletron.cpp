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


ErrorCode Simpletron::execute() {
	setState(State::RUNNING);
	while ( state == State::RUNNING && regs.counter < MEM_SIZE) {

		//1. fetch
		regs.instruction = memory[regs.counter++];		

		//2. decode
		regs.opCode = static_cast<OpCode>(regs.instruction / 100);
		regs.operand = static_cast<uint16_t>(regs.instruction % 100);

		auto result = executeInstruction();
		if (result != ErrorCode::NONE) {
			return result;
		}
	}
	return ErrorCode::NONE;
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

	event_bus.publish(ProgramStartEvent{});
	auto executionResult = execute();
	event_bus.publish(ProgramEndEvent{});
	dump();

	return executionResult;
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
			event_bus.publish(ValidationEvent{ ValidationStatus::INVALID_OPERAND, static_cast<uint16_t>(i), operand });
			return false;
		}

		if (!isValidOpCode(opCode)) {
				event_bus.publish(ValidationEvent{ ValidationStatus::INVALID_INSTRUCTION, static_cast<uint16_t>(i), static_cast<uint16_t>(instruction) });
				return false;
			}
	}

	// Programa válido
	return true;
}

void Simpletron::dumpRegisters() const {
	event_bus.publish(RegistersDumpEvent{
		regs.accumulator,
		regs.counter,
		regs.instruction,
		static_cast<uint8_t>(regs.opCode),
		regs.operand
	});
}

void Simpletron::dumpMemory() const {
	// Publicar evento para cada linha de memória (10 em 10)
	for (int start = 0; start < MEM_SIZE; start += 10) {
		event_bus.publish(MemoryDumpEvent{ 
			static_cast<uint16_t>(start),
			static_cast<uint16_t>(start + 9)
		});
	}
}

void Simpletron::printInteractiveMenu() const {
	// Menu permanece com cout já que é apenas interface de entrada
	std::cout <<"* -------------------------------------------------------------------- *" << std::endl
		<< "|                      Welcome to Simpletron!                          |" << std::endl
		<< "* -------------------------------------------------------------------- *" << std::endl
		<< "| Please enter your program one instruction (or data word) at a time.  |" << std::endl
		<< "| I will type the location number and a question mark(?).              |" << std::endl
		<< "| You then type the word for that location.                            |" << std::endl
		<< "| Type the sentinel -9999 to stop entering your program.               | " << std::endl
		<< "* -------------------------------------------------------------------- *" << std::endl;
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
	event_bus.publish(InputPromptEvent{});
	int16_t value = input_provider.readValue();
	if (!input_provider.lastReadSucceeded()) {
		return ErrorCode::NONE;
	}
	memory[regs.operand] = value;
	return ErrorCode::NONE;
}

ErrorCode Simpletron::opWrite() {
	event_bus.publish(OutputValueEvent{ memory[regs.operand] });
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


vector<short> Simpletron::readProgram() {
	printInteractiveMenu();
	short i = 0;
	short instruction;
	auto *program = new vector<short>();
	do {
		std::cout << "   " << std::setw(4) << std::setfill('0') << std::noshowpos << i << " ? ";
		instruction = input_provider.readValue();
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