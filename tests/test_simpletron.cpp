#include <gtest/gtest.h>
#include "simpletron.h"
#include "mock_input_provider.h"

TEST(SimpletronCore, RegistersAreZeroInitializedByDefault) {
	EventBus bus;
	MockInputProvider input;
	Simpletron core(bus, input);
	const auto& registers = core.getRegisters();    
	const auto& state = core.getState();

	EXPECT_EQ(registers.accumulator, 0);
	EXPECT_EQ(registers.counter, 0);
	EXPECT_EQ(registers.instruction, 0);
	EXPECT_EQ(registers.opCode, OpCode::HALT);
	EXPECT_EQ(registers.operand, 0);    
	EXPECT_EQ(state, State::READY);    
}

TEST(SimpletronCore, StepExecutesOneInstructionPerCall) {
	EventBus bus;
	MockInputProvider input;
	Simpletron core(bus, input);

	int executedCycles = 0;
	bus.subscribe<InstructionExecutedEvent>([&executedCycles](const InstructionExecutedEvent&) {
		executedCycles++;
	});

	ASSERT_EQ(core.run({
		makeInstruction(OpCode::NOOP),
		makeInstruction(OpCode::HALT)
	}), ErrorCode::NONE);

	EXPECT_EQ(executedCycles, 2);
}

TEST(SimpletronCore, StepStopsAtHalt) {
	EventBus bus;
	MockInputProvider input;
	Simpletron core(bus, input);

	ASSERT_EQ(core.run({ makeInstruction(OpCode::HALT) }), ErrorCode::NONE);
	const auto counter = core.getRegisters().counter;

	EXPECT_EQ(core.step(), ErrorCode::NONE);
	EXPECT_EQ(core.getRegisters().counter, counter);
	EXPECT_TRUE(core.isHalted());
}

TEST(SimpletronCore, StepReturnsExecutionError) {
	EventBus bus;
	MockInputProvider input;
	Simpletron core(bus, input);

	const auto result = core.run({
		makeInstruction(OpCode::DIVIDE, 1),
		0
	});

	EXPECT_EQ(result, ErrorCode::DIVISION_BY_ZERO);
	EXPECT_TRUE(core.hasError());
	EXPECT_EQ(core.step(), ErrorCode::DIVISION_BY_ZERO);
}

TEST(SimpletronCore, NoopAdvancesCounterWithoutModifyingState) {
	EventBus bus;   
	MockInputProvider input;
	Simpletron core(bus, input);

	core.run(std::vector<short> {
		makeInstruction(OpCode::NOOP, 0),
		makeInstruction(OpCode::HALT, 0)
	});

	const auto& reg = core.getRegisters();
	const auto& state = core.getState();
	const auto& hasError = core.hasError();

	EXPECT_EQ(state, State::HALTED);
	EXPECT_FALSE(hasError);
	EXPECT_EQ(reg.accumulator, 0); 
	EXPECT_EQ(reg.counter, 2);     
}

TEST(SimpletronCore, BranchUnconditionalJumpsDirectlyToTarget) {
	EventBus bus;
	MockInputProvider input;
	Simpletron core(bus, input);

	auto program = vector<short>{
		makeInstruction(OpCode::BRANCH, 2),
		makeInstruction(OpCode::LOAD, 99),  //if this instruction is executed, the test will fail
		makeInstruction(OpCode::HALT)
	};

	core.run(program);

	const auto& reg = core.getRegisters();    	
	EXPECT_EQ(core.getState(), State::HALTED);
	EXPECT_FALSE(core.hasError());

	// The accumulator should remain 0 because the 
	// LOAD instruction at position 1 was skipped due to 
	// the unconditional branch to position 2.
	EXPECT_EQ(reg.accumulator, 0);

	// The instruction counter should be 3 after 
	// executing the HALT instruction at position 2.
	EXPECT_EQ(reg.counter, 3);
 
}
