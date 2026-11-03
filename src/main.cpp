#include <vector>
#include <iostream>
#include <iomanip>
#include "simpletron.h"
#include "clock_runner.h"
#include "console_input_provider.cpp"

using namespace std;

const auto LogToStdOut = ([](const LogEvent& event) {
	string level;
	switch (event.level) {
	case LogLevel::DEBUG: level = "DEBUG"; break;
	case LogLevel::INFO: level = "INFO"; break;
	case LogLevel::WARNING: level = "WARNING"; break;
	case LogLevel::ERROR: level = "ERROR"; break;
	}
	cout << "[" << level << "] " << event.message << endl;
});

int main(int argc, char *argv[]) {
	EventBus event_bus;
	ConsoleInputProvider input_provider;
	Simpletron simpletron(event_bus, input_provider);
	vector<short> program;

	// Subscribe to LogEvent
	event_bus.subscribe<LogEvent>([](const LogEvent& event) {
		LogToStdOut(event);
	});

	// Subscribe to RegistersDumpEvent
	event_bus.subscribe<RegistersDumpEvent>([](const RegistersDumpEvent& event) {
		cout << "Registers:" << endl;
		cout << "  accumulator         : " << showpos << setfill('0') << setw(5) << internal << event.accumulator << endl;
		cout << "  instructionCounter  : " << noshowpos << setfill(' ') << setw(5) << internal << event.counter << endl;
		cout << "  instructionRegister : " << showpos << setfill('0') << setw(5) << internal << event.instruction << endl;
		cout << "  operationCode       : " << noshowpos << setfill(' ') << setw(5) << internal << static_cast<unsigned>(event.opCode) << endl;
		cout << "  operand             : " << noshowpos << setfill(' ') << setw(5) << internal << event.operand << endl;
	});

	// Subscribe to MemoryDumpEvent
	event_bus.subscribe<MemoryDumpEvent>([&simpletron](const MemoryDumpEvent& event) {
		auto memory = simpletron.getMemory();

		// Memory grid header (only on first line)
		if (event.startAddress == 0) {
			cout << "Memory\n       0     1     2     3     4     5     6     7     8     9" << endl;
		}

		// Print memory line
		cout << noshowpos << setfill(' ') << setw(4) << event.startAddress << " ";
		for (auto i = event.startAddress; i <= event.endAddress && i < memory.size(); i++) {
			cout << showpos << setfill('0') << setw(5) << internal << memory[i] << " ";
		}
		cout << endl;
	});

	// Subscribe to OutputValueEvent (WRITE operation)
	event_bus.subscribe<OutputValueEvent>([](const OutputValueEvent& event) {
		cout << setw(5) << setfill('0') << showpos << internal << event.value << endl;
	});

	// Subscribe to InputPromptEvent
	event_bus.subscribe<InputPromptEvent>([](const InputPromptEvent& event) {
		cout << " ? ";
		cout.flush();
	});

	// Subscribe to ValidationEvent
	event_bus.subscribe<ValidationEvent>([](const ValidationEvent& event) {
		switch (event.status) {
		case ValidationStatus::VALID:
			cout << "Program is valid." << endl;
			break;
		case ValidationStatus::INVALID_OPERAND:
			cout << "Invalid operand address at index " << event.instructionIndex 
				 << ": " << event.faultyValue << "\n";
			break;
		case ValidationStatus::INVALID_INSTRUCTION:
			cout << "Invalid instruction : " << event.faultyValue << endl;
			break;
		}
	});

	// Subscribe to ProgramStartEvent
	event_bus.subscribe<ProgramStartEvent>([](const ProgramStartEvent& event) {
		// Can add logging if needed
	});

	// Subscribe to ProgramEndEvent
	event_bus.subscribe<ProgramEndEvent>([](const ProgramEndEvent& event) {
		cout << endl << "*** Program finished ***" << endl;
	});

	// Subscribe to ErrorMessageEvent (optional)
	event_bus.subscribe<ErrorMessageEvent>([](const ErrorMessageEvent& event) {
		cout << "ERROR: ";
		switch (event.errorType) {
		case ErrorMessageEvent::ErrorType::FILE_NOT_FOUND:
			cout << "Could not open file" << endl;
			break;
		case ErrorMessageEvent::ErrorType::DIVISION_BY_ZERO:
			cout << "Division by zero" << endl;
			break;
		default:
			cout << "Unknown error" << endl;
		}
	});

	const bool cycleMode = argc >= 4 && string(argv[1]) == "--cycles";
	if (argc <= 1) {
		program = simpletron.readProgram();
	}
	else if (!cycleMode) {
		program = Simpletron::readFromFile(argv[1]);
	}

	if (cycleMode) {
		program = Simpletron::readFromFile(argv[2]);
	}

	if (cycleMode) {
		const auto loadResult = simpletron.load(program);
		if (loadResult == ErrorCode::NONE) {
			const auto cycleCount = static_cast<size_t>(stoull(argv[3]));
			const auto result = ClockRunner::runForCycles(simpletron, cycleCount);
			cout << "Executed cycles: " << result.cycles << endl;
			simpletron.dump();
		}
	}
	else {
		simpletron.run(program);
	}

	return 0;
}
