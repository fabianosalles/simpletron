#include "assembler.h"
#include "simpletron.h"

#include <charconv>
#include <cctype>
#include <format>
#include <limits>
#include <map>
#include <sstream>
#include <string_view>

namespace {
struct Statement {
	size_t line;
	std::string operation;
	std::string operand;
};

std::string upper(std::string value) {
	for (char& character : value)
		character = static_cast<char>(std::toupper(static_cast<unsigned char>(character)));
	return value;
}

std::string removeComments(std::string line) {
	const auto firstComment = line.find_first_of(";#");
	const auto slashComment = line.find("//");
	const auto end = std::min(firstComment, slashComment);
	if (end != std::string::npos)
		line.erase(end);
	return line;
}

int parseInteger(const std::string& value, size_t line) {
	int result = 0;
	const auto* begin = value.data();
	const auto* end = begin + value.size();
	if (begin != end && *begin == '+')
		++begin;
	const auto parsed = std::from_chars(begin, end, result);
	if (parsed.ec != std::errc{} || parsed.ptr != end)
		throw AssemblyError(line, std::format("invalid integer '{}'.", value));
	return result;
}

const std::map<std::string, OpCode> mnemonics = {
	{"NOOP", OpCode::NOOP}, {"READ", OpCode::READ}, {"WRITE", OpCode::WRITE},
	{"LOAD", OpCode::LOAD}, {"STORE", OpCode::STORE}, {"ADD", OpCode::ADD},
	{"SUBTRACT", OpCode::SUBTRACT}, {"DIVIDE", OpCode::DIVIDE}, {"MULTIPLY", OpCode::MULTIPLY},
	{"BRANCH", OpCode::BRANCH}, {"BRANCHNEG", OpCode::BRANCHNEG},
	{"BRANCHZERO", OpCode::BRANCHZERO}, {"BRANCHPOS", OpCode::BRANCHPOS}, {"HALT", OpCode::HALT}
};

bool takesOperand(OpCode opcode) {
	return opcode != OpCode::NOOP && opcode != OpCode::HALT;
}
}

AssemblyError::AssemblyError(size_t line, const std::string& message)
	: std::runtime_error(std::format("Assembly error on line {}: {}", line, message)), line_(line) {}

std::vector<int16_t> Assembler::assemble(const std::string& source) {
	std::vector<Statement> statements;
	std::map<std::string, int> labels;
	std::istringstream input(source);
	std::string lineText;
	size_t lineNumber = 0;
	int address = 0;

	while (std::getline(input, lineText)) {
		++lineNumber;
		auto line = removeComments(lineText);
		std::istringstream tokens(line);
		std::string first;
		if (!(tokens >> first))
			continue;

		if (!first.empty() && first.back() == ':') {
			first.pop_back();
			const auto label = upper(first);
			if (label.empty() || labels.contains(label))
				throw AssemblyError(lineNumber, "invalid or duplicate label.");
			labels[label] = address;
			if (!(tokens >> first))
				continue;
		}

		std::string operand;
		tokens >> operand;
		std::string extra;
		if (tokens >> extra)
			throw AssemblyError(lineNumber, "too many tokens.");

		const auto operation = upper(first);
		if (operation == ".WORD" || operation == ".DATA") {
			if (operand.empty())
				throw AssemblyError(lineNumber, "data directives require a value.");
		} else if (!mnemonics.contains(operation)) {
			int value = parseInteger(first, lineNumber);
			if (!operand.empty())
				throw AssemblyError(lineNumber, "numeric words cannot have operands.");
			if (value < std::numeric_limits<int16_t>::min() || value > std::numeric_limits<int16_t>::max())
				throw AssemblyError(lineNumber, "data value is outside the int16 range.");
		} else {
			const auto opcode = mnemonics.at(operation);
			if (takesOperand(opcode) && operand.empty())
				throw AssemblyError(lineNumber, "this instruction requires an operand.");
			if (!takesOperand(opcode) && !operand.empty())
				throw AssemblyError(lineNumber, "this instruction does not accept an operand.");
		}

		if (address >= MEM_SIZE)
			throw AssemblyError(lineNumber, "program exceeds Simpletron memory.");
		statements.push_back({ lineNumber, operation, operand });
		++address;
	}

	std::vector<int16_t> program;
	program.reserve(statements.size());
	for (const auto& statement : statements) {
		if (statement.operation == ".WORD" || statement.operation == ".DATA") {
			const int value = parseInteger(statement.operand, statement.line);
			if (value < std::numeric_limits<int16_t>::min() || value > std::numeric_limits<int16_t>::max())
				throw AssemblyError(statement.line, "data value is outside the int16 range.");
			program.push_back(static_cast<int16_t>(value));
			continue;
		}

		if (!mnemonics.contains(statement.operation)) {
			const int value = parseInteger(statement.operation, statement.line);
			program.push_back(static_cast<int16_t>(value));
			continue;
		}

		const auto opcode = mnemonics.at(statement.operation);
		int operand = 0;
		if (!statement.operand.empty()) {
			const auto label = upper(statement.operand);
			if (labels.contains(label))
				operand = labels.at(label);
			else
				operand = parseInteger(statement.operand, statement.line);
		}
		if (operand < 0 || operand >= MEM_SIZE)
			throw AssemblyError(statement.line, "operand must be a memory address from 0 to 99.");
		program.push_back(makeInstruction(opcode, static_cast<uint16_t>(operand)));
	}
	return program;
}
