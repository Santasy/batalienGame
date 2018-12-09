#ifndef _ALIEN_H
#define _ALIEN_H

#include <glm/glm.hpp>
#include "mesh.h"

class alien : public mesh{
	
	public:
		alien(char *filename);
		int cooldown;
};

#endif
