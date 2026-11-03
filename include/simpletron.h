#pragma once

#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include <vector>
#include <cstring>
#include <stdint.h>
#include <span>
#include <optional>
#include <format>
#include "events.h"
#include "input_provider.h"

using namespace std;


#define MEM_SIZE 100


enum class State: uint8_t {
	PARSING,
	LOADING,
	READY,
	RUNNING,	
	HALTED,
	ERROR
};

enum class ErrorCode : uint8_t {
	/* No error */
	NONE = 0,

	INVALID_INSTRUCTION = 1,	

	DIVISION_BY_ZERO = 2,

	NOT_ENOUGTH_MEMORY = 3,
};

enum class OpCode : uint8_t {
	NOOP = 0,
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

struct Registers {
	uint16_t counter = 0;
	int16_t instruction = 0;
	OpCode opCode = OpCode::HALT;
	uint16_t operand = 0;
	int16_t accumulator = 0;
};

struct StateChangedEvent {
	State oldState;
	State newState;
};

struct ProgramLoadedEvent {
	size_t instructionCount;
};

struct ExectionEvent {
	ErrorCode errorCode;
};

struct InstructionExecutedEvent {
	uint16_t address;
	int16_t instruction;
	uint8_t opCode;
	uint16_t operand;
	ErrorCode result;
};



class Simpletron {
public:
	explicit Simpletron(EventBus& bus, IInputProvider& inputProvider) noexcept 
		: event_bus(bus), input_provider(inputProvider) {};	
	bool parse(const vector<short>& program);
	void dump() const;
	ErrorCode load(const vector<short>& program);
	ErrorCode run(const vector<short>& program);
	/**
	 * @brief Executes exactly one instruction cycle.
	 *
	 * A READ instruction may block while the configured input provider waits
	 * for input. Timing and scheduling are intentionally handled by callers.
	 */
	ErrorCode step();
	vector<short> readProgram();
	static vector<short> readFromFile(const std::string& fileName);

	ErrorCode raiseError(ErrorCode code) noexcept;

	/**
	* @brief Retrieves a readonly view of the Simpletron's memory.
	*/
	[[nodiscard]] span<const int16_t> getMemory() const noexcept {
		return span<const int16_t>(memory, MEM_SIZE);
	}

	[[nodiscard]] const Registers& getRegisters() const noexcept { return regs; }
	[[nodiscard]] const State& getState() const noexcept { return state; }
	[[nodiscard]] bool isHalted() const noexcept { return state == State::HALTED; }
	[[nodiscard]] bool hasError() const noexcept { return state == State::ERROR; }

private:
	EventBus& event_bus;
	IInputProvider& input_provider;

	int16_t memory[MEM_SIZE] = { 0 };
	Registers regs = { 0 };
	State state = State::READY;
	uint8_t errorCode = static_cast<uint8_t>(ErrorCode::NONE);
	bool executionStarted = false;
	bool executionEnded = false;
		
	ErrorCode opRead();
	ErrorCode opWrite();	
	ErrorCode opLoad();
	ErrorCode opStore();
	ErrorCode opAdd();
	ErrorCode opSubtract();
	ErrorCode opDivide();
	ErrorCode opMultiply();
	ErrorCode opBranch();
	ErrorCode opBranchNeg();
	ErrorCode opBranchZero();
	ErrorCode opHalt();	

	ErrorCode executeInstruction();
	void reset();
	void dumpRegisters() const;
	void dumpMemory() const;
	void printInteractiveMenu() const;	
	bool isValidOpCode(OpCode op) noexcept;
	void setState(State newState) noexcept;

	void LogInfo(std::string message) { event_bus.publish(LogEvent{ LogLevel::INFO, message }); }
	void LogError(std::string message) { event_bus.publish(LogEvent{ LogLevel::ERROR, message }); }
	void LogDebug(std::string message) { event_bus.publish(LogEvent{ LogLevel::DEBUG, message }); }
	void LogWarning(std::string message) { event_bus.publish(LogEvent{ LogLevel::WARNING, message }); }
};

/**
 * @brief Constructs a 4-digit encoded Simpletron instruction word.
 *
 * Combines an operation code (opCode) and a target memory address (operand)
 * into a single decimal machine instruction (e.g., OpCode 40 and operand 10 result in 4010).
 *
 * @param opCode The operation code representing the instruction to execute.
 * @param operand The target memory address (ranging from 0 to 99).
 * @return int16_t The fully encoded instruction ready to be stored in VM memory.
 */
[[nodiscard]] constexpr int16_t makeInstruction(OpCode opCode, uint16_t operand = 0) noexcept {
	return static_cast<int16_t>(static_cast<uint8_t>(opCode) * 100 + (operand % 100));
}