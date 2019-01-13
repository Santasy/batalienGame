#include "tools.h"
#include "mesh.h"
#include "stb_image.h"

/*---Constructor---*/
mesh::mesh(char *filename){
	this->filename = filename;
	this->position = glm::vec3(0, 0, 0);
	this->rotation = glm::vec3(0, 1, 0);
	this->angle = 0.0f;
	assert(load_mesh(filename, &vao, &numVertices));
}

/*---Gets---*/
btRigidBody* mesh::getBody(){
	return this->body;
}

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
void mesh::setMass(double m){
	this->mass = *(new btScalar(m));
}

void mesh::setCollisionShape(btCollisionShape *b){
	this->btCS = b;
}

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
void mesh::createRigidBody(btCollisionShape *b, btVector3 origin, float m){
	this->btCS = b;
	this->btT.setIdentity();
	this->btT.setOrigin(origin);
	this->mass = *(new btScalar(m));
	this->localInertia = *(new btVector3(-1, -1, -1));
	this->isDynamic = (m != 0.0f);
	if(this->isDynamic) this->btCS->calculateLocalInertia(this->mass, this->localInertia);
	this->motionState = new btDefaultMotionState(this->btT);
	btRigidBody::btRigidBodyConstructionInfo info(this->mass, this->motionState, this->btCS, this->localInertia);
	this->body = new btRigidBody(info);
	this->body->setActivationState(DISABLE_DEACTIVATION);
}

void mesh::draw(int matloc, glm::mat4 &aux, btTransform &trans){
	this->body->getMotionState()->getWorldTransform(trans);
	trans.getOpenGLMatrix(&aux[0][0]);
	this->setModelMatrix(aux);

	glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(this->getVao());
    glBindTexture(GL_TEXTURE_2D, tex);
    glPointSize(3.0);

	glUniformMatrix4fv(matloc, 1, GL_FALSE, &this->modelMatrix[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, this->getNumVertices());
	glBindVertexArray(this->getVao());
}

bool mesh::load_texture (const char* file_name) {
	int x, y, n;
	int force_channels = 4;
	unsigned char* image_data = stbi_load (file_name, &x, &y, &n, force_channels);
    //printf("x = %i    y = %i\n", x, y);
	if (!image_data) {
		fprintf (stderr, "ERROR: could not load %s\n", file_name);
		return false;
	}
	// NPOT check
	if ((x & (x - 1)) != 0 || (y & (y - 1)) != 0) {
		fprintf (
			stderr, "WARNING: texture %s is not power-of-2 dimensions\n", file_name
		);
	}
	int width_in_bytes = x * 4;
	unsigned char *top = NULL;
	unsigned char *bottom = NULL;
	unsigned char temp = 0;
	int half_height = y / 2;

	for (int row = 0; row < half_height; row++) {
		top = image_data + row * width_in_bytes;
		bottom = image_data + (y - row - 1) * width_in_bytes;
		for (int col = 0; col < width_in_bytes; col++) {
			temp = *top;
			*top = *bottom;
			*bottom = temp;
			top++;
			bottom++;
		}
	}
	glGenTextures (1, &tex);
	glActiveTexture (GL_TEXTURE0);
	glBindTexture (GL_TEXTURE_2D, tex);
	glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	glGenerateMipmap (GL_TEXTURE_2D);
    // probar cambiar GL_CLAMP_TO_EDGE por GL_REPEAT
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	GLfloat max_aniso = 16.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_aniso);
	// set the maximum!
	glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_aniso);
	return true;
}
