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
		btTransform btT;
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
		btRigidBody* getBody(); 

		/*---Sets---*/
		void setVao(GLuint vao);
		void setMass(double m);
		void setNumVertices(int num);
		void setRotation(float ang, glm::vec3 rot);
		void setFilename(char *f);
		void setModelMatrix(glm::mat4 model);
		void setCollisionShape(btCollisionShape *b);

		/*---Others---*/
		void createRigidBody(btCollisionShape *b, btVector3 origin, float m);
		void draw(int matloc); //Integrar setModelMatrix dentro de la funcion
		bool load_texture(const char* file_name);
		void getTrans(btTransform trans);

		/*---TODO---*/		
		void setPosition(glm::vec3 pos); //Rehacer con setOrigin
		
};

#endif