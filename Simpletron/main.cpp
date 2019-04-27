#include "Simpletron.h"



int main(int argc, char *argv[]) {
	Simpletron simpletron;
	vector<short> program;

	if (argc <= 1) {				
		program = simpletron.readProgram();
		simpletron.run(program);
	}
	else {
		program = simpletron.readFromFile(argv[1]);
		simpletron.run(program);
	}


	/*
	vector<short> program = {
		+1007,  // READ A ON 7st MEM POSITION
		+1008,  // READ B ON 8st MEM POSITION
		+2007,  // LOAD A
		+3008,  // ADD B
		+2109,  // STORE C
		+1109,  // WRITE C
		+4300   // HALT
	};
	

	if (simpletron.parse(program)) {
		simpletron.run(program);
	} */
}