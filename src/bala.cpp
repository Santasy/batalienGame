#include "tools.h"
#include "bala.h"

bala::bala(char* filename) : mesh(filename){
	std::cout << "Created bala\n";
}

// Draw
//GLFW_KEY_BACKSPACE
void bala::draw(int matloc){
	glUniformMatrix4fv(matloc, 1, GL_FALSE, &this->modelMatrix[0][0]);
	glBindVertexArray(this->getVao());
	glDrawArrays(GL_TRIANGLES, 0, this->getNumVertices());
}
 
/*void handleKeypress (int x, int y) {
    static int animate = 0;                                 
 
    switch (key) {
       case 27:   exit(0);   break;
       case 32:
          animate = !animate;                               
          if (animate)  glutIdleFunc (bullet);
          else          glutIdleFunc ( NULL ); 
       break;
    }
 
    glutPostRedisplay();
}*/