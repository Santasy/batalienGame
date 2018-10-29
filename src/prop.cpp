#include "tools.h"
#include "prop.h"

prop::prop(char* filename): mesh(filename){
	std::cout << "Created prop\n";
}