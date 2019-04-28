#include "Simpletron.h"


int main(int argc, char *argv[]) {
	Simpletron simpletron;
	vector<short> *program;

	if (argc <= 1) {				
		program = simpletron.readProgram();
		simpletron.run(*program);
		delete program;
	}
	else {
		program = simpletron.readFromFile(argv[1]);
		simpletron.run(*program);
		delete program;
	}

}