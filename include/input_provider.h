#pragma once

#include <cstdint>

/**
 * @brief Abstraction for input provision (I/O)
 * 
 * Decouples Simpletron from std::cin, allowing different
 * input sources (console, file, mock in tests, etc)
 */
class IInputProvider {
public:
	virtual ~IInputProvider() = default;

	/**
	 * @brief Reads an int16_t value from the input provider
	 * @return Value read
	 * @note Concrete implementations should handle input errors
	 */
	[[nodiscard]] virtual int16_t readValue() = 0;

	/**
	 * @brief Checks if the last read was successful
	 * @return true if last read succeeded, false otherwise
	 */
	[[nodiscard]] virtual bool lastReadSucceeded() const noexcept = 0;
};
