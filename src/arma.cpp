#include "tools.h"
#include "arma.h"

arma::arma(char* filename) : mesh(filename){
	std::cout << "Created arma\n";
}