#pragma once

#include "input_provider.h"
#include <queue>

/**
 * @brief Mock InputProvider for tests
 * 
 * Allows simulating user input in tests without using std::cin
 */
class MockInputProvider : public IInputProvider {
public:
	explicit MockInputProvider() = default;

	/**
	 * @brief Enqueues a value to be read
	 */
	void enqueueValue(int16_t value) {
		values.push(value);
	}

	[[nodiscard]] int16_t readValue() override {
		if (values.empty()) {
			lastReadSuccess = false;
			return 0;
		}
		int16_t value = values.front();
		values.pop();
		lastReadSuccess = true;
		return value;
	}

	[[nodiscard]] bool lastReadSucceeded() const noexcept override {
		return lastReadSuccess;
	}

	void reset() {
		lastReadSuccess = true;
		while (!values.empty()) {
			values.pop();
		}
	}

private:
	std::queue<int16_t> values;
	bool lastReadSuccess = true;
};
