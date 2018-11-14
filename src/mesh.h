#ifndef _MESH_H
#define _MESH_H

#include <glm/glm.hpp>
#include <bullet/btBulletDynamicsCommon.h>


class mesh{
	protected:
		GLuint vao, vbo, tex;
		int numVertices;
		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;
		glm::mat4 modelMatrix;
		char* filename;
		float angle;

		/*---Collision---*/
		btCollisionShape *btCS; // ->calculateLocalIntertia(mass, localInertia);
		btTransform *btT;
		btScalar mass;
		bool isDynamic;
		btVector3 localInertia;
		btDefaultMotionState *motionState;
		btRigidBody *body;

	public:
		mesh(char *filename);

		/*---Gets---*/
		GLuint getVao();
		int getNumVertices();
		glm::vec3 getPosition();
		glm::vec3 getRotation();
		char* getFilename();

		/*---Sets---*/
		void setVao(GLuint vao);
		void setNumVertices(int num);
		void setRotation(float ang, glm::vec3 rot);
		void setFilename(char *f);
		void setModelMatrix(glm::mat4 model);

		/*---Others---*/
		void draw(int matloc); //Integrar setModelMatrix dentro de la funcion
		bool load_texture(const char* file_name);

		/*---TODO---*/
		void setCollisionShape(btCollisionShape *btCS);
		void createRigidBody();
		void setMass(double m);
		void setPosition(glm::vec3 pos); //Rehacer con setOrigin
		void getTrans(btTransform *trans);
};

#endif