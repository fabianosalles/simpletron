#include <vector>

#include "simpletron.h"


const auto LogToStdOut = ([](const LogEvent& event) {
	std::string level;
	switch (event.level) {
	case LogLevel::DEBUG: level = "DEBUG"; break;
	case LogLevel::INFO: level = "INFO"; break;
	case LogLevel::WARNING: level = "WARNING"; break;
	case LogLevel::ERROR: level = "ERROR"; break;
	}
	std::cout << "[" << level << "] " << event.message << std::endl;
});

int main(int argc, char *argv[]) {
	EventBus event_bus;	
	Simpletron simpletron(event_bus);
	vector<short> program;

	if (argc <= 1) {
		event_bus.subscribe<LogEvent>([](const LogEvent& event) {
			LogToStdOut(event);
		});

		program = simpletron.readProgram();
		simpletron.run(program);
	}
	else {
		program = Simpletron::readFromFile(argv[1]);
		simpletron.run(program);
	}

}