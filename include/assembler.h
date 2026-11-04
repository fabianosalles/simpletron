#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

class AssemblyError : public std::runtime_error {
public:
	AssemblyError(size_t line, const std::string& message);

	[[nodiscard]] size_t line() const noexcept { return line_; }

private:
	size_t line_;
};

class Assembler {
public:
	[[nodiscard]] static std::vector<int16_t> assemble(const std::string& source);
};
