#include "tools.h"
#include "mesh.h"

/*---Constructor---*/
mesh::mesh(char *filename){
	this->filename = filename;
	this->position = glm::vec3(0, 0, 0);
	this->rotation = glm::vec3(0, 1, 0);
	this->angle = 0.0f;
	assert(load_mesh(filename, &vao, &numVertices));
}

/*---Gets---*/
GLuint mesh::getVao(){
	return this->vao;
}

int mesh::getNumVertices(){
	return this->numVertices;
}

glm::vec3 mesh::getPosition(){
	return this->position;
}

glm::vec3 mesh::getRotation(){
	return this->rotation;
}

char* mesh::getFilename(){
	return this->filename;
}

/*---Sets---*/

void mesh::setPosition(glm::vec3 pos){
	this->position = pos;
}

void mesh::setRotation(float ang, glm::vec3 rot){
	this->rotation = rot;
	this->angle = ang;
}

void mesh::setFilename(char *f){
	this->filename = f;
}

void mesh::setModelMatrix(glm::mat4 model){
	this->modelMatrix = model;
}

/*---Others---*/
void mesh::draw(int matloc){
	glUniformMatrix4fv(matloc, 1, GL_FALSE, &this->modelMatrix[0][0]);
	glBindVertexArray(this->getVao());
	glDrawArrays(GL_TRIANGLES, 0, this->getNumVertices());
}