#ifndef _BALA_H
#define _BALA_H

#include <glm/glm.hpp>
#include "mesh.h"

class bala : public mesh{

	public:
		bala(char *filename);

		/*---Others---*/
		void draw(int matloc);
};

#endif