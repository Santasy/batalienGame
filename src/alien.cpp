#include "tools.h"
#include "alien.h"
using namespace std;

alien::alien(char *filename) : mesh(filename){
	std::cout << "Created alien\n";

	this->alive = true;
	this->cooldown = 0;
	this->hp = 3;
	this->esalien = true;
}
