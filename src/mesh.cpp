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