#ifndef _PISO_H
#define _PISO_H

#include <glm/glm.hpp>

class piso{
	private:
		GLuint vao, vbo;
		int numVertices;
		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;
		glm::mat4 modelMatrix;
		char* filename;
		float angle;
	public:
		piso(char *filename);

		/*---Gets---*/
		GLuint getVao();
		int getNumVertices();
		glm::vec3 getPosition();
		glm::vec3 getRotation();
		char* getFilename();

		/*---Sets---*/
		void setVao(GLuint vao);
		void setNumVertices(int num);
		void setPosition(glm::vec3 pos);
		void setRotation(float ang, glm::vec3 rot);
		void setFilename(char *f);
		void setModelMatrix(glm::mat4 model);

		/*---Others---*/
		void draw(int matloc);
};
#endif