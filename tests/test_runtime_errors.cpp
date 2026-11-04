#include <gtest/gtest.h>

#include <limits>
#include <vector>

#include "mock_input_provider.h"
#include "simpletron.h"

TEST(SimpletronRuntimeErrors, RejectsProgramsLargerThanMemory) {
	EventBus bus;
	MockInputProvider input;
	Simpletron core(bus, input);
	const std::vector<short> program(MEM_SIZE + 1, makeInstruction(OpCode::NOOP));

	EXPECT_EQ(core.run(program), ErrorCode::NOT_ENOUGTH_MEMORY);
	EXPECT_TRUE(core.hasError());
}

TEST(SimpletronRuntimeErrors, ReportsProgramCounterPastMemory) {
	EventBus bus;
	MockInputProvider input;
	Simpletron core(bus, input);
	const std::vector<short> program(MEM_SIZE, makeInstruction(OpCode::NOOP));

	EXPECT_EQ(core.run(program), ErrorCode::NOT_ENOUGTH_MEMORY);
	EXPECT_TRUE(core.hasError());
}

TEST(SimpletronRuntimeErrors, RejectsInvalidInstructionAtRuntime) {
	EventBus bus;
	MockInputProvider input;
	Simpletron core(bus, input);

	EXPECT_EQ(core.run({ 9999 }), ErrorCode::INVALID_INSTRUCTION);
	EXPECT_TRUE(core.hasError());
}

TEST(SimpletronRuntimeErrors, RejectsNegativeInstructionAtRuntime) {
	EventBus bus;
	MockInputProvider input;
	Simpletron core(bus, input);

	EXPECT_EQ(core.run({ -1 }), ErrorCode::INVALID_INSTRUCTION);
	EXPECT_TRUE(core.hasError());
}

TEST(SimpletronRuntimeErrors, ReportsInputFailure) {
	EventBus bus;
	MockInputProvider input;
	Simpletron core(bus, input);

	EXPECT_EQ(core.run({ makeInstruction(OpCode::READ, 0) }), ErrorCode::INPUT_FAILURE);
	EXPECT_TRUE(core.hasError());
}

TEST(SimpletronRuntimeErrors, ReportsAddOverflow) {
	EventBus bus;
	MockInputProvider input;
	Simpletron core(bus, input);

	EXPECT_EQ(core.run({
		makeInstruction(OpCode::LOAD, 3),
		makeInstruction(OpCode::ADD, 4),
		makeInstruction(OpCode::HALT),
		std::numeric_limits<int16_t>::max(),
		1
	}), ErrorCode::ARITHMETIC_OVERFLOW);
}

TEST(SimpletronRuntimeErrors, ReportsSubtractOverflow) {
	EventBus bus;
	MockInputProvider input;
	Simpletron core(bus, input);

	EXPECT_EQ(core.run({
		makeInstruction(OpCode::LOAD, 3),
		makeInstruction(OpCode::SUBTRACT, 4),
		makeInstruction(OpCode::HALT),
		std::numeric_limits<int16_t>::min(),
		1
	}), ErrorCode::ARITHMETIC_OVERFLOW);
}

TEST(SimpletronRuntimeErrors, ReportsMultiplyOverflow) {
	EventBus bus;
	MockInputProvider input;
	Simpletron core(bus, input);

	EXPECT_EQ(core.run({
		makeInstruction(OpCode::LOAD, 3),
		makeInstruction(OpCode::MULTIPLY, 4),
		makeInstruction(OpCode::HALT),
		std::numeric_limits<int16_t>::max(),
		2
	}), ErrorCode::ARITHMETIC_OVERFLOW);
}

TEST(SimpletronRuntimeErrors, ReportsDivisionOverflow) {
	EventBus bus;
	MockInputProvider input;
	Simpletron core(bus, input);

	EXPECT_EQ(core.run({
		makeInstruction(OpCode::LOAD, 3),
		makeInstruction(OpCode::DIVIDE, 4),
		makeInstruction(OpCode::HALT),
		std::numeric_limits<int16_t>::min(),
		-1
	}), ErrorCode::ARITHMETIC_OVERFLOW);
}

TEST(SimpletronRuntimeErrors, StopsAfterTheFirstRuntimeError) {
	EventBus bus;
	MockInputProvider input;
	Simpletron core(bus, input);
	int executedInstructions = 0;
	bus.subscribe<InstructionExecutedEvent>([&executedInstructions](const InstructionExecutedEvent&) {
		++executedInstructions;
	});

	EXPECT_EQ(core.run({ makeInstruction(OpCode::DIVIDE, 1), 0, makeInstruction(OpCode::HALT) }),
		ErrorCode::DIVISION_BY_ZERO);
	EXPECT_EQ(executedInstructions, 1);
	EXPECT_TRUE(core.hasError());
}
