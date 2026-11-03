#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>

#include "simpletron.h"

struct ClockRunResult {
	ErrorCode errorCode;
	std::uint64_t cycles;
};

class ClockRunner {
public:
	[[nodiscard]] static ClockRunResult runForCycles(
		Simpletron& simpletron,
		std::size_t cycleCount);

	[[nodiscard]] static ClockRunResult runAtPeriod(
		Simpletron& simpletron,
		std::chrono::nanoseconds period,
		std::size_t maxCycles);
};
