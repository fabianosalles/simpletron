#include <gtest/gtest.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "simpletron.h"

template <typename StreamType>
class StreamRedirect {
public:
    StreamRedirect(StreamType& stream, std::streambuf* newBuffer)
        : stream_(stream), oldBuffer_(stream.rdbuf(newBuffer)) {}

    ~StreamRedirect() {
        stream_.rdbuf(oldBuffer_);
    }

    StreamRedirect(const StreamRedirect&) = delete;
    StreamRedirect& operator=(const StreamRedirect&) = delete;

private:
    StreamType& stream_;
    std::streambuf* oldBuffer_;
};

// ==========================================
// 1. INPUT / OUTPUT (READ / WRITE)
// ==========================================

TEST(SimpletronOpcodes, ReadStoresInputIntoMemory) {
    Simpletron core;

    std::cin.clear();
    std::stringstream input("42\n");
    {
        StreamRedirect<std::istream> cinRedirect(std::cin, input.rdbuf());
        core.run({
            makeInstruction(OpCode::READ, 10),
            makeInstruction(OpCode::HALT, 0)
            });
    }

    auto memory = core.getMemory();
    EXPECT_FALSE(core.hasError());
    EXPECT_EQ(memory[10], 42);
    EXPECT_EQ(core.getState(), State::HALTED);
}

TEST(SimpletronOpcodes, WriteOutputsMemoryValue) {
    Simpletron core;
    std::stringstream output;
    const std::vector<short> program = {
        makeInstruction(OpCode::WRITE, 2),
        makeInstruction(OpCode::HALT, 0),
        42  // This is the value that will be written to the output
    };

    {
        StreamRedirect<std::ostream> coutRedirect(std::cout, output.rdbuf());
        core.run(program);
    }

    const std::string outStr = output.str();
    EXPECT_FALSE(core.hasError());
    EXPECT_EQ(core.getState(), State::HALTED);
    EXPECT_NE(outStr.find("42"), std::string::npos);
}

// ==========================================
// 2. DATA TRANSFER (LOAD / STORE)
// ==========================================

TEST(SimpletronOpcodes, LoadTransfersMemoryValueToAccumulator) {
    Simpletron core;

    // Program:
    // 0: LOAD 02 (load value at address 2 into accumulator)
    // 1: HALT 00
    // 2: 1234    (raw data word in memory)
    const std::vector<short> program = {
        makeInstruction(OpCode::LOAD, 2),
        makeInstruction(OpCode::HALT, 0),
        1234
    };

    core.run(program);

    const auto& regs = core.getRegisters();
    EXPECT_FALSE(core.hasError());
    EXPECT_EQ(core.getState(), State::HALTED);
    EXPECT_EQ(regs.accumulator, 1234);
}

TEST(SimpletronOpcodes, StoreTransfersAccumulatorToMemory) {
    Simpletron core;

    // Program:
    // 0: LOAD 03  (load 55 into accumulator)
    // 1: STORE 10 (store accumulator into memory address 10)
    // 2: HALT 00
    // 3: 55       (raw data word)
    const std::vector<short> program = {
        makeInstruction(OpCode::LOAD, 3),
        makeInstruction(OpCode::STORE, 10),
        makeInstruction(OpCode::HALT, 0),
        55
    };

    core.run(program);

    auto memory = core.getMemory();
    EXPECT_FALSE(core.hasError());
    EXPECT_EQ(core.getState(), State::HALTED);
    EXPECT_EQ(memory[10], 55);
}

// ==========================================
// 3. ARITHMETIC OPERATIONS (ADD, SUBTRACT, MULTIPLY, DIVIDE)
// ==========================================

TEST(SimpletronOpcodes, AddSumsMemoryValueToAccumulator) {
    Simpletron core;

    // 0: LOAD 04 (acc = 20)
    // 1: ADD 05  (acc = 20 + 15 = 35)
    // 2: HALT 00
    // 3: NOOP 00 (data alignment separator)
    // 4: 20
    // 5: 15
    const std::vector<short> program = {
        makeInstruction(OpCode::LOAD, 4),
        makeInstruction(OpCode::ADD, 5),
        makeInstruction(OpCode::HALT, 0),
        makeInstruction(OpCode::NOOP, 0),
        20,
        15
    };

    core.run(program);

    const auto& regs = core.getRegisters();
    EXPECT_FALSE(core.hasError());
    EXPECT_EQ(regs.accumulator, 35);
}

TEST(SimpletronOpcodes, SubtractDeductsMemoryValueFromAccumulator) {
    Simpletron core;

    // 0: LOAD 03     (acc = 50)
    // 1: SUBTRACT 04 (acc = 50 - 18 = 32)
    // 2: HALT 00
    // 3: 50
    // 4: 18
    const std::vector<short> program = {
        makeInstruction(OpCode::LOAD, 3),
        makeInstruction(OpCode::SUBTRACT, 4),
        makeInstruction(OpCode::HALT, 0),
        50,
        18
    };

    core.run(program);

    const auto& regs = core.getRegisters();
    EXPECT_FALSE(core.hasError());
    EXPECT_EQ(regs.accumulator, 32);
}

TEST(SimpletronOpcodes, MultiplyMultipliesAccumulatorByMemoryValue) {
    Simpletron core;

    // 0: LOAD 03     (acc = 7)
    // 1: MULTIPLY 04 (acc = 7 * 6 = 42)
    // 2: HALT 00
    // 3: 7
    // 4: 6
    const std::vector<short> program = {
        makeInstruction(OpCode::LOAD, 3),
        makeInstruction(OpCode::MULTIPLY, 4),
        makeInstruction(OpCode::HALT, 0),
        7,
        6
    };

    core.run(program);

    const auto& regs = core.getRegisters();
    EXPECT_FALSE(core.hasError());
    EXPECT_EQ(regs.accumulator, 42);
}

TEST(SimpletronOpcodes, DivideDividesAccumulatorByMemoryValue) {
    Simpletron core;

    // 0: LOAD 03   (acc = 84)
    // 1: DIVIDE 04 (acc = 84 / 2 = 42)
    // 2: HALT 00
    // 3: 84
    // 4: 2
    const std::vector<short> program = {
        makeInstruction(OpCode::LOAD, 3),
        makeInstruction(OpCode::DIVIDE, 4),
        makeInstruction(OpCode::HALT, 0),
        84,
        2
    };

    core.run(program);

    const auto& regs = core.getRegisters();
    EXPECT_FALSE(core.hasError());
    EXPECT_EQ(regs.accumulator, 42);
}

TEST(SimpletronOpcodes, DivideByZeroSetsErrorState) {
    Simpletron core;

    // 0: LOAD 03   (acc = 10)
    // 1: DIVIDE 04 (division by zero -> runtime error)
    // 2: HALT 00
    // 3: 10
    // 4: 0
    const std::vector<short> program = {
        makeInstruction(OpCode::LOAD, 3),
        makeInstruction(OpCode::DIVIDE, 4),
        makeInstruction(OpCode::HALT, 0),
        10,
        0
    };

    core.run(program);

    EXPECT_TRUE(core.hasError());
    EXPECT_NE(core.getState(), State::HALTED);
}

// ==========================================
// 4. CONDITIONAL BRANCHING (BRANCHNEG / BRACHZERO)
// ==========================================

TEST(SimpletronOpcodes, BranchNegBranchesWhenAccumulatorIsNegative) {
    Simpletron core;

    // 0: LOAD 04       (acc = -5)
    // 1: BRANCHNEG 03  (branches directly to HALT at index 3)
    // 2: LOAD 05       (trap: must be skipped if branch succeeds)
    // 3: HALT 00
    // 4: -5
    // 5: 999
    const std::vector<short> program = {
        makeInstruction(OpCode::LOAD, 4),
        makeInstruction(OpCode::BRANCHNEG, 3),
        makeInstruction(OpCode::LOAD, 5),
        makeInstruction(OpCode::HALT, 0),
        -5,
        999
    };

    core.run(program);

    const auto& regs = core.getRegisters();
    EXPECT_FALSE(core.hasError());
    EXPECT_EQ(regs.accumulator, -5);
}

TEST(SimpletronOpcodes, BranchNegDoesNotBranchWhenAccumulatorIsPositiveOrZero) {
    Simpletron core;

    // 0: LOAD 04       (acc = 10, positive)
    // 1: BRANCHNEG 03  (should not branch)
    // 2: LOAD 05       (executes sequentially)
    // 3: HALT 00
    // 4: 10
    // 5: 25
    const std::vector<short> program = {
        makeInstruction(OpCode::LOAD, 4),
        makeInstruction(OpCode::BRANCHNEG, 3),
        makeInstruction(OpCode::LOAD, 5),
        makeInstruction(OpCode::HALT, 0),
        10,
        25
    };

    core.run(program);

    const auto& regs = core.getRegisters();
    EXPECT_FALSE(core.hasError());
    EXPECT_EQ(regs.accumulator, 25);
}

TEST(SimpletronOpcodes, BranchZeroBranchesWhenAccumulatorIsZero) {
    Simpletron core;

    // 0: BRACHZERO 02  (acc is 0 by default, jumps directly to index 2)
    // 1: LOAD 03       (trap: must not execute)
    // 2: HALT 00
    // 3: 500
    const std::vector<short> program = {
        makeInstruction(OpCode::BRACHZERO, 2),
        makeInstruction(OpCode::LOAD, 3),
        makeInstruction(OpCode::HALT, 0),
        500
    };

    core.run(program);

    const auto& regs = core.getRegisters();
    EXPECT_FALSE(core.hasError());
    EXPECT_EQ(regs.accumulator, 0);
}

TEST(SimpletronOpcodes, BranchZeroDoesNotBranchWhenAccumulatorIsNonZero) {
    Simpletron core;

    // 0: LOAD 04       (acc = 7)
    // 1: BRACHZERO 03  (should not branch)
    // 2: LOAD 05       (executes sequentially)
    // 3: HALT 00
    // 4: 7
    // 5: 99
    const std::vector<short> program = {
        makeInstruction(OpCode::LOAD, 4),
        makeInstruction(OpCode::BRACHZERO, 3),
        makeInstruction(OpCode::LOAD, 5),
        makeInstruction(OpCode::HALT, 0),
        7,
        99
    };

    core.run(program);

    const auto& regs = core.getRegisters();
    EXPECT_FALSE(core.hasError());
    EXPECT_EQ(regs.accumulator, 99);
}
