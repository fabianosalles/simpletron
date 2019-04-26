#include "Simpletron.h"



int main(){
	Simpletron simpletron;
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
	}
}