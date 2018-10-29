#include "tools.h"
#include "alien.h"
using namespace std;

alien::alien(char *filename) : mesh(filename){
	std::cout << "Created alien\n";
}