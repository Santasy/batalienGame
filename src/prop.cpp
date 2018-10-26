#include "tools.h"
#include "prop.h"

prop::prop(char* filename){
	this->filename = filename;
	this->position = glm::vec3(0.,0.,0.);
	this->rotation = glm::vec3(0.,1.,0.);
	this->angle = 0.0f;
	assert(load_mesh(filename, &vao, &numVertices));
}

//gets

GLuint prop::getVao(){
	return this->vao;
}

int prop::getNumVertices(){
	return this->numVertices;
}

glm::vec3 prop::getPosition(){
	return this->position;
}

glm::vec3 prop::getRotation(){
	return this->rotation;
}

char* prop::getFilename(){
	return this->filename;
}

//sets

void prop::setVao(GLuint vao){
	this->vao = vao;
}

void prop::setPosition(glm::vec3 pos){
	this->position = pos;
}

void prop::setRotation(float ang, glm::vec3 rot){
	this->rotation = rot;
	this->angle = ang;
}

void prop::setFilename(char* f){
	this->filename = f;
}

void prop::setModelMatrix(glm::mat4 model){
	this->modelMatrix = model;
}

//Draw

void prop::draw(int matloc){
	glUniformMatrix4fv(matloc, 1, GL_FALSE, &this->modelMatrix[0][0]);
	glBindVertexArray(this->getVao());
	glDrawArrays(GL_TRIANGLES, 0, this->getNumVertices());
}