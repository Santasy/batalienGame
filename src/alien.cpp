#include "tools.h"
#include "alien.h"
using namespace std;

alien::alien(char *filename){
	this->filename = filename;
	this->position = glm::vec3(0, 0, 0);
	this->rotation = glm::vec3(0, 1, 0);
	this->angle = 0.0f;
	assert(load_mesh(filename, &vao, &numVertices));
}
/* Gets */
GLuint alien::getVao(){
	return this->vao;
}

int alien::getNumVertices(){
	return this->numVertices;
}

glm::vec3 alien::getPosition(){
	return this->position;
}

glm::vec3 alien::getRotation(){
	return this->rotation;
}

char* alien::getFilename(){
	return this->filename;
}

/*---Sets---*/

void alien::setPosition(glm::vec3 pos){
	this->position = pos;
}

void alien::setRotation(float ang, glm::vec3 rot){
	this->rotation = rot;
	this->angle = ang;
}

void alien::setFilename(char *f){
	this->filename = f;
}

void alien::setModelMatrix(glm::mat4 model){
	this->modelMatrix = model;
}

/*---Others---*/
void alien::draw(int matloc){
	glUniformMatrix4fv(matloc, 1, GL_FALSE, &this->modelMatrix[0][0]);
	glBindVertexArray(this->getVao());
	glDrawArrays(GL_TRIANGLES, 0, this->getNumVertices());
}