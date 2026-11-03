#include <gtest/gtest.h>

#include "clock_runner.h"
#include "mock_input_provider.h"

TEST(ClockRunner, RunsConfiguredNumberOfCycles) {
	EventBus bus;
	MockInputProvider input;
	Simpletron core(bus, input);

	const auto result = ClockRunner::runForCycles(core, 2);

	EXPECT_EQ(result.errorCode, ErrorCode::NONE);
	EXPECT_EQ(result.cycles, 2);
	EXPECT_EQ(core.getRegisters().counter, 2);
}

TEST(ClockRunner, StopsWhenProgramHalts) {
	EventBus bus;
	MockInputProvider input;
	Simpletron core(bus, input);

	ASSERT_EQ(core.run({ makeInstruction(OpCode::HALT) }), ErrorCode::NONE);
	const auto result = ClockRunner::runForCycles(core, 10);

	EXPECT_EQ(result.errorCode, ErrorCode::NONE);
	EXPECT_EQ(result.cycles, 0);
}