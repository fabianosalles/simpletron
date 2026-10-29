#include <gtest/gtest.h>
#include "simpletron.h"

TEST(SimpletronCore, RegistersAreZeroInitializedByDefault) {
    EventBus bus;
    Simpletron core(bus);
    const auto& registers = core.getRegisters();    
	const auto& state = core.getState();

    EXPECT_EQ(registers.accumulator, 0);
    EXPECT_EQ(registers.counter, 0);
    EXPECT_EQ(registers.instruction, 0);
    EXPECT_EQ(registers.opCode, OpCode::HALT);
    EXPECT_EQ(registers.operand, 0);    
    EXPECT_EQ(state, State::READY);    
}

TEST(SimpletronCore, NoopAdvancesCounterWithoutModifyingState) {
    EventBus bus;   
    Simpletron core(bus);
    
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
    Simpletron core(bus);

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
