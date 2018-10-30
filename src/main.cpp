#include <bits/stdc++.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <bullet/btBulletDynamicsCommon.h>

/* Nuestros archivos */
#include "tools.h"
#include "gl_utils.h"
#include "mesh.h"
#include "alien.h"
#include "arma.h"
#include "prop.h"
#include "piso.h"
#include "bala.h"
#include "GLDebugDrawer.hpp"

#define GL_LOG_FILE "log/gl.log"
#define VERTEX_SHADER_FILE "shaders/vShader.glsl"
#define FRAGMENT_SHADER_FILE "shaders/fShader.glsl"

/*---Interaction Functions---*/
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window, btRigidBody *player);

/*---Window Properties---*/
int g_gl_width  =  1080;
int g_gl_height =  800;
GLFWwindow *g_window = NULL;

/*---Camera Properties---*/
glm::vec3 cameraPos   = glm::vec3(0.0f, 12.0f, 15.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, -2.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw   = -90.0f;
float pitch =  0.0f;
float lastX =  g_gl_width / 2.0;
float lastY =  g_gl_height / 2.0;
float fov   =  90.0f;

/*---Time Calculation---*/
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main(){
	/*---Window---*/
	init();
	glfwSetFramebufferSizeCallback(g_window, framebuffer_size_callback);
	glfwSetCursorPosCallback(g_window, mouse_callback);
	glfwSetScrollCallback(g_window, scroll_callback);
	glfwSetInputMode(g_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	/*---Shaders---*/
	GLuint shader_programme = create_programme_from_files(VERTEX_SHADER_FILE, FRAGMENT_SHADER_FILE);

	/*--Perspective---*/
	glm::mat4 projection = glm::perspective(glm::radians(fov), (float)g_gl_width / (float)g_gl_height, 0.1f, 100.0f);
  glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

	int view_mat_location = glGetUniformLocation(shader_programme, "view");
	glUseProgram(shader_programme);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, &view[0][0]);
	int proj_mat_location = glGetUniformLocation(shader_programme, "proj");
	glUseProgram(shader_programme);
	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, &projection[0][0]);

	int model_mat_location = glGetUniformLocation(shader_programme, "model");

	/*---Mesh load---*/
	alien *alien1 = new alien((char*)"mesh/Alien.obj");
	piso *terrain = new piso((char*)"mesh/MapaSimple.obj");
	bala *balax = new bala((char*)"mesh/balaxx.obj");
	prop *mesa = new prop((char*)"mesh/MesaxSuzanne.obj");


	/*---Physic Compound---*/
	btDefaultCollisionConfiguration *collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher *dispatcher = new btCollisionDispatcher(collisionConfiguration);
	btBroadphaseInterface *overlappingPairCache = new btDbvtBroadphase();
	btSequentialImpulseConstraintSolver *solver = new btSequentialImpulseConstraintSolver;
	btDiscreteDynamicsWorld *dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
	btDiscreteDynamicsWorld *dynamicsWorld2 = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

	dynamicsWorld->setGravity(btVector3(0, -9.8f, 0));
	dynamicsWorld2->setGravity(btVector3(0, 0, -19.0f));

	btCollisionShape *balaxShape = new btSphereShape(btScalar(0.4));
	btCollisionShape *alien1Shape = new btSphereShape(btScalar(1.));
	btCollisionShape *terrainShape = new btBoxShape(btVector3(15, 0.05f, 15));
	btCollisionShape *mesaShape = new btBoxShape(btVector3(2,2,2));

	btTransform mesaTransform;
	mesaTransform.setIdentity();
	mesaTransform.setOrigin(btVector3(5, 2, 3));
	btScalar mesaMass(100);

	btTransform balaxTransform;
	balaxTransform.setIdentity();
	balaxTransform.setOrigin(btVector3(3, 2, 3));
	btScalar balaxMass(100);

	btTransform alien1Transform;
	alien1Transform.setIdentity();
	alien1Transform.setOrigin(btVector3(3, 5, -3));
	btScalar alien1Mass(5);

	btTransform terrainTransform;
	terrainTransform.setIdentity();
	terrainTransform.setOrigin(btVector3(0, 0, 0));
	btScalar terrainMass(0.);

	bool isDynamicAlien1 = (alien1Mass != 0.0f);
	bool isDynamicMesa = (mesaMass != 0.0f);
	bool isDynamicBalax = (balaxMass != 0.0f);

	btVector3 localInertiaMesa(1,0.,0);
	btVector3 localInertiaBalax(1, 0, 0);
	btVector3 localInertiaAlien1(1, 0, 0);
  btVector3 localInertiaTerrain(1, 0, 0);

	if(isDynamicAlien1){
		alien1Shape->calculateLocalInertia(alien1Mass, localInertiaAlien1);
	}
	if(isDynamicBalax){
		balaxShape->calculateLocalInertia(balaxMass, localInertiaAlien1);
	}
	if(isDynamicMesa){
		mesaShape->calculateLocalInertia(mesaMass, localInertiaMesa);
	}

	btDefaultMotionState *alien1MotionState = new btDefaultMotionState(alien1Transform);
	btRigidBody::btRigidBodyConstructionInfo alien1RbInfo(alien1Mass, alien1MotionState, alien1Shape, localInertiaAlien1);
	btRigidBody *bodyAlien1 = new btRigidBody(alien1RbInfo);

	btDefaultMotionState *balaxMotionState = new btDefaultMotionState(balaxTransform);
	btRigidBody::btRigidBodyConstructionInfo balaxRbInfo(balaxMass, balaxMotionState, balaxShape, localInertiaBalax);
	btRigidBody *bodyBalax = new btRigidBody(balaxRbInfo);

	btDefaultMotionState *terrainMotionState = new btDefaultMotionState(terrainTransform);
	btRigidBody::btRigidBodyConstructionInfo terrainRbInfo(terrainMass, terrainMotionState, terrainShape, localInertiaTerrain);
	btRigidBody *bodyTerrain = new btRigidBody(terrainRbInfo);

	bodyAlien1->setActivationState(DISABLE_DEACTIVATION);
	bodyBalax->setActivationState(DISABLE_DEACTIVATION);

	btDefaultMotionState *mesaMotionState = new btDefaultMotionState(mesaTransform);
	btRigidBody::btRigidBodyConstructionInfo mesaRbInfo(mesaMass, mesaMotionState, mesaShape, localInertiaMesa);
	btRigidBody *bodyMesa = new btRigidBody(mesaRbInfo);

	dynamicsWorld->addRigidBody(bodyAlien1);
	//dynamicsWorld->addRigidBody(bodyMesa);
	dynamicsWorld->addRigidBody(bodyTerrain);
	dynamicsWorld2->addRigidBody(bodyBalax);


	/*---Debuger---*/
	GLDebugDrawer *debug = new GLDebugDrawer();
	debug->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
	debug->setView(&view);
	debug->setProj(&projection);
	dynamicsWorld->setDebugDrawer(debug);

	/*---Main Loop---*/
	glm::mat4 aux;

	while (!glfwWindowShouldClose(g_window)){

		/*---JoystickTEST-----GastonciñoTrabajan2*/

			int present = glfwJoystickPresent(GLFW_JOYSTICK_1);
			/*std::cout<<"Joystick1 status: "<<present<<std::endl;
			if(present==1){
				int axesCount;
				const float *axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
				std::cout<<"Number of axes available: "<<axesCount<<std::endl;
				std::cout<<"Left Stick X Axis: "<<axes[0]<<std::endl;
				std::cout<<"Left Stick Y Axis: "<<axes[1]<<std::endl;
				std::cout<<"Right Stick X Axis: "<<axes[4]<<std::endl;
				std::cout<<"Right Stick Y Axis: "<<axes[3]<<std::endl;
				std::cout<<"Left Trigger Axis: "<<axes[2]<<std::endl;
				std::cout<<"Right Trigger Axis: "<<axes[5]<<std::endl;
			}*/

		/*---------------*/

		dynamicsWorld->stepSimulation(1.0f / 60.0f, 10);
		dynamicsWorld2->stepSimulation(1.0f / 60.0f, 10);


		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(g_window, bodyAlien1);
			

		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram (shader_programme);

		projection = glm::perspective(glm::radians(fov), (float)g_gl_width / (float)g_gl_height, 0.1f, 100.0f);
		glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, &projection[0][0]);

		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, &view[0][0]);

		btTransform trans;

		bodyAlien1->getMotionState()->getWorldTransform(trans);
		
		trans.getOpenGLMatrix(&aux[0][0]);
		alien1->setModelMatrix(aux);
		alien1->draw(model_mat_location);

		bodyTerrain->getMotionState()->getWorldTransform(trans);

		trans.getOpenGLMatrix(&aux[0][0]);
		terrain->setModelMatrix(aux);
		terrain->draw(model_mat_location);

		bodyBalax->getMotionState()->getWorldTransform(trans); 
		trans.getOpenGLMatrix(&aux[0][0]);
		balax->setModelMatrix(aux);
		balax->draw(model_mat_location);

		bodyMesa->getMotionState()->getWorldTransform(trans);
		trans.getOpenGLMatrix(&aux[0][0]);
		mesa->setModelMatrix(aux);
		mesa->draw(model_mat_location);
		
		/*
		dynamicsWorld->debugDrawWorld();
		debug->drawLines();
		dynamicsWorld2->debugDrawWorld();
		debug->drawLines();
		*/

		glfwSwapBuffers(g_window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window, btRigidBody *player){
	/*------------------UI------------------*/
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	float cameraSpeed = 2.5 * deltaTime;
	/*--------------CameraKeyboard-------------*/
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	/*--------------PlayerKeyboard------------*/
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		player->applyCentralForce(btVector3(0.,0.,-30.0));
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		player->applyCentralForce(btVector3(0.,0.,30.0));
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		player->applyCentralForce(btVector3(-30.0,0.,0.));
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		player->applyCentralForce(btVector3(30.0,0.,0.));
	/*-----------JoystickInputs----------*/
	int present = glfwJoystickPresent(GLFW_JOYSTICK_1);
	if(present==1){
		int axesCount;
		const float *axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
		if (axes[0]<(-0.3))
			player->applyCentralForce(btVector3(-30.0,0.,0.));
		if (axes[0]>(0.3))
			player->applyCentralForce(btVector3(30.0,0.,0.));
		if (axes[1]<(-0.3))
			player->applyCentralForce(btVector3(0.,0.,-30.0));
		if (axes[1]>(0.3))
			player->applyCentralForce(btVector3(0.,0.,30.0));
		if (axes[4]>(0.3))
			cameraPos -= cameraSpeed * cameraFront;
		if (axes[4]<(-0.3))
			cameraPos += cameraSpeed * cameraFront;
		if (axes[3]>(0.3))
			cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		if (axes[3]<(-0.3))
			cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		}
	}

void mouse_callback(GLFWwindow* window, double xpos, double ypos){
	if(firstMouse){
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
}