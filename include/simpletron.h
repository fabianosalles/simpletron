#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include <vector>
#include <cstring>
#include <stdint.h>
#include <span>
#include <optional>

using namespace std;


#define MEM_SIZE 100


enum class State: uint8_t {
	LOADING,
	READY,
	RUNNING,	
	HALTED,
	ERROR
};

enum class ErrorCode : uint8_t {
	SUCCESS = 0,
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


class Simpletron {
private:
	using Handler = bool (Simpletron::*)();

	int16_t memory[MEM_SIZE] = { 0 };
	Registers regs = { 0 };
	State state = State::READY;
	uint8_t errorCode = static_cast<uint8_t>(ErrorCode::SUCCESS);
		
	bool opRead();
	bool opWrite();	
	bool opLoad();
	bool opStore();
	bool opAdd();
	bool opSubtract();
	bool opDivide();
	bool opMultiply();
	bool opBranch();
	bool opBranchNeg();
	bool opBranchZero();
	bool opHalt();		

	ErrorCode load(const vector<short>& program);
	bool executeInstruction();
	void reset();
	void execute();
	void dumpRegisters() const;
	void dumpMemory() const;
	void printInteractiveMenu() const;	
	bool isValidOpCode(OpCode op) noexcept;

public:		
	bool parse(const vector<short>& program);
	void dump() const;
	void run(const vector<short>& program);
	vector<short> readProgram() const;
	static vector<short> readFromFile(const std::string& fileName);

	ErrorCode raiseError(ErrorCode code) noexcept {
		state = State::ERROR;
		errorCode = static_cast<uint8_t>(code);
		return code;
	}

	/**
	* @bref Retrieves a readonly view of the Simpletron's memory.
	*/
	[[nodiscard]] span<const int16_t> getMemory() const noexcept {
		return span<const int16_t>(memory, MEM_SIZE);
	}
	
	[[nodiscard]] const Registers& getRegisters() const noexcept { return regs; }
	[[nodiscard]] const State& getState() const noexcept { return state; }
	[[nodiscard]] bool isHalted() const noexcept { return state == State::HALTED; }
	[[nodiscard]] bool hasError() const noexcept { return state == State::ERROR; }
	
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