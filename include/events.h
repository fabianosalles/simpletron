#pragma once

#include<string>
#include<typeindex>
#include<unordered_map>
#include<functional>
#include <any>
#include <cstdint>

enum class LogLevel {
	DEBUG,
	INFO,
	WARNING,
	ERROR,	
};

struct LogEvent {
	LogLevel level;
	std::string message;
};

// Output Events - Pure data, memory optimized
struct RegistersDumpEvent {
	int16_t accumulator;
	uint16_t counter;
	int16_t instruction;
	uint8_t opCode;
	uint16_t operand;
};

struct MemoryDumpEvent {
	uint16_t startAddress;
	uint16_t endAddress;
	// Main is responsible for accessing memory via simpletron.getMemory()
};

struct OutputValueEvent {
	int16_t value;
};

struct InputPromptEvent {
	// Event indicates that input is required
	// Main responds by publishing InputProvidedEvent
};

struct InputProvidedEvent {
	int16_t value;
	bool success;
};

enum class ValidationStatus : uint8_t {
	VALID,
	INVALID_OPERAND,
	INVALID_INSTRUCTION
};

struct ValidationEvent {
	ValidationStatus status;
	uint16_t instructionIndex;  // Index of instruction with problem
	uint16_t faultyValue;       // Invalid value
};

struct ProgramStartEvent {
	// Marks beginning of execution
};

struct ProgramEndEvent {
	// Marks end of execution
};

struct ErrorMessageEvent {
	enum class ErrorType : uint8_t {
		PARSE_INVALID_OPERAND,
		PARSE_INVALID_INSTRUCTION,
		DIVISION_BY_ZERO,
		INVALID_INSTRUCTION_CODE,
		FILE_NOT_FOUND
	} errorType;
	uint16_t detail;  // Can be index, address, etc
};


class EventBus {
public:    
	// Subcribes to any type of event T
    template <typename EventType>
    void subscribe(std::function<void(const EventType&)> handler) {
        auto type = std::type_index(typeid(EventType));

        handlers[type].push_back([handler = std::move(handler)](const std::any& eventAny) {        
            handler(std::any_cast<const EventType&>(eventAny));
        });
    }

    // Publishes any type of event T
    template <typename EventType>
    void publish(const EventType& event) const {
        auto type = std::type_index(typeid(EventType));
        auto it = handlers.find(type);

        if (it != handlers.end()) {
            for (const auto& handler : it->second) {
                handler(event);
            }
        }
    }

private:
    using AnyHandler = std::function<void(const std::any&)>;
    std::unordered_map<std::type_index, std::vector<AnyHandler>> handlers;
};