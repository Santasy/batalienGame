#include "tools.h"
#include "arma.h"

arma::arma(char* filename){
	this->filename = filename;
	this->position = glm::vec3(0.,0.,0.);
	this->rotation = glm::vec3(0.,1.,0.);
	this->angle = 0.0f;
	assert(load_mesh(filename, &vao, &numVertices));
}

//gets

GLuint arma::getVao(){
	return this->vao;
}

int arma::getNumVertices(){
	return this->numVertices;
}

glm::vec3 arma::getPosition(){
	return this->position;
}

glm::vec3 arma::getRotation(){
	return this->rotation;
}

char* arma::getFilename(){
	return this->filename;
}

//sets

void arma::setVao(GLuint vao){
	this->vao = vao;
}

void arma::setPosition(glm::vec3 pos){
	this->position = pos;
}

void arma::setRotation(float ang, glm::vec3 rot){
	this->rotation = rot;
	this->angle = ang;
}

void arma::setFilename(char* fi){
	this->filename = fi;
}

void arma::setModelMatrix(glm::mat4 model){
	this->modelMatrix = model;
}

// Draw

void arma::draw(int matloc){
	glUniformMatrix4fv(matloc, 1, GL_FALSE, &this->modelMatrix[0][0]);
	glBindVertexArray(this->getVao());
	glDrawArrays(GL_TRIANGLES, 0, this->getNumVertices());
}