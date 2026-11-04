#include <gtest/gtest.h>

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "assembler.h"
#include "simpletron.h"
#include "mock_input_provider.h"

namespace {
std::string readSourceFile(const std::string& fileName) {
	std::ifstream file(std::string(SIMPLETRON_SAMPLE_DIR) + "/" + fileName);
	EXPECT_TRUE(file.is_open()) << "Could not open sample file: " << fileName;
	std::ostringstream source;
	source << file.rdbuf();
	return source.str();
}
}

TEST(Assembler, EncodesMnemonicsAndResolvesLabels) {
	const auto program = Assembler::assemble(
		"start: LOAD value\n"
		"        ADD one\n"
		"        STORE result\n"
		"        HALT\n"
		"value: .word 40\n"
		"one:   .data 2\n"
		"result: .word 0\n");

	ASSERT_EQ(program.size(), 7);
	EXPECT_EQ(program[0], makeInstruction(OpCode::LOAD, 4));
	EXPECT_EQ(program[1], makeInstruction(OpCode::ADD, 5));
	EXPECT_EQ(program[2], makeInstruction(OpCode::STORE, 6));
	EXPECT_EQ(program[3], makeInstruction(OpCode::HALT));
	EXPECT_EQ(program[4], 40);
	EXPECT_EQ(program[5], 2);
	EXPECT_EQ(program[6], 0);
}

TEST(Assembler, AcceptsCommentsAndLegacyNumericWords) {
	const auto program = Assembler::assemble(
		"; a complete comment line\n"
		"+2002  ; load the data\n"
		"5000 # halt\n"
		"42 // data word\n");

	EXPECT_EQ(program, (std::vector<int16_t>{ 2002, 5000, 42 }));
}

TEST(Assembler, ReportsInvalidSyntaxWithLineNumber) {
	try {
		static_cast<void>(Assembler::assemble("LOAD\n"));
		FAIL() << "Expected an AssemblyError.";
	} catch (const AssemblyError& error) {
		EXPECT_EQ(error.line(), 1);
		EXPECT_NE(std::string(error.what()).find("requires an operand"), std::string::npos);
	}
}

TEST(Assembler, AssembledProgramRunsOnSimpletron) {
	EventBus bus;
	MockInputProvider input;
	Simpletron core(bus, input);
	const auto program = Assembler::assemble(
		"LOAD value\n"
		"WRITE value\n"
		"HALT\n"
		"value: .word 7\n");

	EXPECT_EQ(core.run(program), ErrorCode::NONE);
	EXPECT_EQ(core.getState(), State::HALTED);
	EXPECT_EQ(core.getRegisters().accumulator, 7);
}

TEST(AssemblerSamples, AllAssemblySamplesAssembleAndRun) {
	struct SampleCase {
		const char* fileName;
		std::vector<int16_t> input;
		int16_t expectedOutput;
	};

	const std::vector<SampleCase> samples = {
		{ "add.asm", { 2, 3 }, 5 },
		{ "divide.asm", { 84, 2 }, 42 },
		{ "multiply.asm", { 7, 6 }, 42 },
		{ "read.asm", { 12 }, 12 },
		{ "subtract.asm", { 50, 18 }, 32 }
	};

	for (const auto& sample : samples) {
		SCOPED_TRACE(sample.fileName);
		EventBus bus;
		MockInputProvider input;
		for (const auto value : sample.input)
			input.enqueueValue(value);

		int16_t output = 0;
		int outputCount = 0;
		bus.subscribe<OutputValueEvent>([&output, &outputCount](const OutputValueEvent& event) {
			output = event.value;
			++outputCount;
		});

		Simpletron core(bus, input);
		const auto program = Assembler::assemble(readSourceFile(sample.fileName));

		EXPECT_EQ(core.run(program), ErrorCode::NONE);
		EXPECT_TRUE(core.isHalted());
		EXPECT_EQ(outputCount, 1);
		EXPECT_EQ(output, sample.expectedOutput);
	}
}
