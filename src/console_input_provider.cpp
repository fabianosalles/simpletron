#include "input_provider.h"
#include <iostream>

/**
 * @brief Concrete implementation of IInputProvider that uses std::cin
 * 
 * Encapsulates all std::cin dependency in one place,
 * allowing Simpletron to be input-agnostic.
 */
class ConsoleInputProvider : public IInputProvider {
public:
	[[nodiscard]] int16_t readValue() override {
		int value = 0;
		if (!(std::cin >> value)) {
			lastReadSuccess = false;
			std::cin.clear();
			std::cin.ignore(10000, '\n');
			return 0;
		}
		lastReadSuccess = true;
		return static_cast<int16_t>(value);
	}

	[[nodiscard]] bool lastReadSucceeded() const noexcept override {
		return lastReadSuccess;
	}

private:
	bool lastReadSuccess = true;
};
