#include "clock_runner.h"

#include <thread>

ClockRunResult ClockRunner::runForCycles(Simpletron& simpletron, std::size_t cycleCount) {
	std::uint64_t cycles = 0;
	while (cycles < cycleCount && !simpletron.isHalted() && !simpletron.hasError()) {
		const auto result = simpletron.step();
		++cycles;
		if (result != ErrorCode::NONE)
			return { result, cycles };
	}

	return { ErrorCode::NONE, cycles };
}

ClockRunResult ClockRunner::runAtPeriod(
	Simpletron& simpletron,
	std::chrono::nanoseconds period,
	std::size_t maxCycles) {
	const auto start = std::chrono::steady_clock::now();
	std::uint64_t cycles = 0;

	while (cycles < maxCycles && !simpletron.isHalted() && !simpletron.hasError()) {
		const auto result = simpletron.step();
		++cycles;
		if (result != ErrorCode::NONE)
			return { result, cycles };

		std::this_thread::sleep_until(start + period * cycles);
	}

	return { ErrorCode::NONE, cycles };
}
