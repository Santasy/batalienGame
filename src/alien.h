#ifndef _ALIEN_H
#define _ALIEN_H

#include <glm/glm.hpp>

class alien{
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
		alien(char *filename);

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