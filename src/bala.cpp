#include "tools.h"
#include "bala.h"

bala::bala(char* filename){
	this->filename = filename;
	this->position = glm::vec3(0.,0.,0.);
	this->rotation = glm::vec3(0.,1.,0.);
	this->angle = 0.0f;
	assert(load_mesh(filename, &vao, &numVertices));
}

//gets

GLuint bala::getVao(){
	return this->vao;
}

int bala::getNumVertices(){
	return this->numVertices;
}

glm::vec3 bala::getPosition(){
	return this->position;
}

glm::vec3 bala::getRotation(){
	return this->rotation;
}

char* bala::getFilename(){
	return this->filename;
}

//sets

void bala::setVao(GLuint vao){
	this->vao = vao;
}

void bala::setPosition(glm::vec3 pos){
	this->position = pos;
}

void bala::setRotation(float ang, glm::vec3 rot){
	this->rotation = rot;
	this->angle = ang;
}

void bala::setFilename(char* fi){
	this->filename = fi;
}

void bala::setModelMatrix(glm::mat4 model){
	this->modelMatrix = model;
}

// Draw
//GLFW_KEY_BACKSPACE
void bala::draw(int matloc){
	glUniformMatrix4fv(matloc, 1, GL_FALSE, &this->modelMatrix[0][0]);
	glBindVertexArray(this->getVao());
	glDrawArrays(GL_TRIANGLES, 0, this->getNumVertices());
	
 
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
}