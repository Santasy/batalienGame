#include "tools.h"
#include "piso.h"

piso::piso(char* filename): mesh(filename){
	std::cout << "Created piso\n";
}