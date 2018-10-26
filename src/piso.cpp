#include "tools.h"
#include "piso.h"

piso::piso(char* filename){
	this->filename = filename;
	this->position = glm::vec3(0.,0.,0.);
	this->rotation = glm::vec3(0.,1.,0.);
	this->angle = 0.0f;
	assert(load_mesh(filename, &vao, &numVertices));
}

//gets

GLuint piso::getVao(){
	return this->vao;
}

int piso::getNumVertices(){
	return this->numVertices;
}

glm::vec3 piso::getPosition(){
	return this->position;
}

glm::vec3 piso::getRotation(){
	return this->rotation;
}

char* piso::getFilename(){
	return this->filename;
}

//sets

void piso::setVao(GLuint vao){
	this->vao = vao;
}

void piso::setPosition(glm::vec3 pos){
	this->position = pos;
}

void piso::setRotation(float ang, glm::vec3 rot){
	this->rotation = rot;
	this->angle = ang;
}

void piso::setFilename(char* f){
	this->filename = f;
}

void piso::setModelMatrix(glm::mat4 model){
	this->modelMatrix = model;
}

//Draw

void piso::draw(int matloc){
	glUniformMatrix4fv(matloc, 1, GL_FALSE, &this->modelMatrix[0][0]);
	glBindVertexArray(this->getVao());
	glDrawArrays(GL_TRIANGLES, 0, this->getNumVertices());
}