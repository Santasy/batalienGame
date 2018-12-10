/* ----- Batalien, by Pencasoft ----- */
/* Librerias */
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
#include "stb_image.h"
#include "maths_funcs.h"
#include "GLDebugDrawer.hpp"

#define GL_LOG_FILE "log/gl.log"
#define VERTEX_SHADER_FILE "shaders/vShader.glsl"
#define FRAGMENT_SHADER_FILE "shaders/fShader.glsl"

#define SKYBOX_VERTEX_SHADER_FILE "shaders/sky_vert.glsl"
#define SKYBOX_FRAGMENT_SHADER_FILE "shaders/sky_frag.glsl"

/*---Interaction Functions---*/
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window, btRigidBody *player);
void shootBullet(alien* shooter);
void reducirCooldowns();
bool contactAddedCallbackBullet( btManifoldPoint& cp, const btCollisionObjectWrapper * colObj0, int partId0, int index0, const btCollisionObjectWrapper * colObj1, int partId1, int index1);

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

btDiscreteDynamicsWorld *world; //lo usaremos para acceder al mundo de forma global.
std::vector<bala*> bullets; //vector de balas
alien *alien1; //lo usaremos para acceder al alien de forma global, está aquí temporalmente para programar y debugear
alien *alien2; //otro alien, para probar

int main(){
	/*---Window---*/
	init();

	/*---Shaders---*/
	GLuint shader_programme = create_programme_from_files(VERTEX_SHADER_FILE, FRAGMENT_SHADER_FILE);

	/*--Perspective---*/
	glm::mat4 projection = glm::perspective(glm::radians(fov), (float)g_gl_width / (float)g_gl_height, 0.1f, 10000.0f);
  	glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

	int view_mat_location = glGetUniformLocation(shader_programme, "view");
	glUseProgram(shader_programme);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, &view[0][0]);
	int proj_mat_location = glGetUniformLocation(shader_programme, "proj");
	glUseProgram(shader_programme);
	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, &projection[0][0]);

	int model_mat_location = glGetUniformLocation(shader_programme, "model");

	glfwSetFramebufferSizeCallback(g_window, framebuffer_size_callback);
	glfwSetCursorPosCallback(g_window, mouse_callback);
	glfwSetScrollCallback(g_window, scroll_callback);
	glfwSetInputMode(g_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		/*----------*/

	/*---Mesh and Textures---*/
	alien1 = new alien((char*)"mesh/Alien.obj");
	alien2 = new alien((char*)"mesh/Alien.obj");
	piso *terrain = new piso((char*)"mesh/MapaSimple.obj");
	terrain->load_texture("textures/mars4k.jpg");

	alien1->cooldown = 0;
	alien1->hp = 3;
	alien2->cooldown = 0;
	alien2->hp = 3;

	/*---Physic Compound---*/

	btDefaultCollisionConfiguration *collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher *dispatcher = new btCollisionDispatcher(collisionConfiguration);
	btBroadphaseInterface *overlappingPairCache = new btDbvtBroadphase();
	btSequentialImpulseConstraintSolver *solver = new btSequentialImpulseConstraintSolver;
	btDiscreteDynamicsWorld *dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

	world = dynamicsWorld; //le pasamos el puntero a nuestro mundo global

	dynamicsWorld->setGravity(btVector3(0, -9.8f, 0));

	gContactAddedCallback=contactAddedCallbackBullet;

		/*---Objects---*/
	alien1->createRigidBody(
		new btSphereShape(btScalar(1.0f)), //CollisionShape
		btVector3(0, 1, 0), //Origin
		5.0f); //Mass

	alien2->createRigidBody(
		new btSphereShape(btScalar(1.0f)), //CollisionShape
		btVector3(5, 1, 1), //Origin
		5.0f); //Mass

	terrain->createRigidBody(
		new btBoxShape(btVector3(15.0f, 0.05f, 15.0f)), //CollisionShape
		btVector3(0, 0, 0), //Origin
		0.0f); //Mass

	dynamicsWorld->addRigidBody(alien1->getBody());
	dynamicsWorld->addRigidBody(alien2->getBody());
	dynamicsWorld->addRigidBody(terrain->getBody());

	alien1->getBody()->setUserPointer(alien1);
	alien2->getBody()->setUserPointer(alien2);

	/*---SkyBox Shader---*/
	GLuint vbosky;
	GLuint vaosky;
	createSkyBox(vbosky, vaosky);

	GLuint skybox_shader = create_programme_from_files(SKYBOX_VERTEX_SHADER_FILE, SKYBOX_FRAGMENT_SHADER_FILE);
	glUseProgram(skybox_shader);

	int view_skybox = glGetUniformLocation(skybox_shader, "view");
	int proj_skybox = glGetUniformLocation(skybox_shader, "proj");
	glUniformMatrix4fv(view_skybox, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(proj_skybox, 1, GL_FALSE, &projection[0][0]);

	GLuint cube_map_texture;
	textureSkyBox(cube_map_texture);
		/*----------*/

	/*---Debuger---*/
	GLDebugDrawer *debug = new GLDebugDrawer();
	debug->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
	debug->setView(&view);
	debug->setProj(&projection);
	dynamicsWorld->setDebugDrawer(debug);

	/*---Main Loop---*/

	glm::mat4 aux;
	while (!glfwWindowShouldClose(g_window)){
		/*---Frames---*/
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		/*---Input---*/
		processInput(g_window, alien1->getBody());

			/*---JoystickTEST-----GastonciñoTrabajan2*/
			/*
			int present = glfwJoystickPresent(GLFW_JOYSTICK_1);
			std::cout<<"Joystick1 status: "<<present<<std::endl;
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

		/*---World---*/
		glUseProgram (shader_programme);

		dynamicsWorld->stepSimulation(1.0f / 60.0f, 10);

		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		projection = glm::perspective(glm::radians(fov), (float)g_gl_width / (float)g_gl_height, 0.1f, 100.0f);
		glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, &projection[0][0]);
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, &view[0][0]);

		reducirCooldowns();
		
		/*---Draws---*/
		btTransform trans;

		alien1->draw(model_mat_location, aux, trans);
		alien2->draw(model_mat_location, aux, trans);
		terrain->draw(model_mat_location, aux, trans);

		for (int i = 0; i < bullets.size(); i++){
			bullets[i]->draw(model_mat_location, aux, trans);
		}


		

		/*---SkyBox Programme---*/
		glUseProgram(skybox_shader);
		glDepthMask(GL_FALSE);
		glUseProgram(skybox_shader);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cube_map_texture);
		glBindVertexArray(vaosky);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthMask(GL_TRUE);
		glUniformMatrix4fv(view_skybox, 1, GL_FALSE, &view[0][0]);
			/*----------*/

		/*---World Debug---*/
		dynamicsWorld->debugDrawWorld();
		debug->drawLines();

		/*----------*/
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

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		shootBullet(alien1); //a la funcion shootBullet se le pasa un alien* que determina quién está disparando 
	
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

void shootBullet(alien* shooter){ //mas tarde mover este metodo  a la clase alien. shooter = el alien que dispara
	if (!(shooter->cooldown)){ //ver si el disparo de este alien esta en enfriamiento
		bala *new_bullet = new bala((char*)"mesh/Alien.obj");
		new_bullet->createRigidBody(
			new btSphereShape(btScalar(0.3f)),
			shooter->getBody()->getCenterOfMassPosition() + (shooter->getBody()->getLinearVelocity().normalized() * 1.5), //dispara en la direccion de movimiento de shooter
			1.0f); //mass
		bullets.push_back(new_bullet); //agrega la bala al vector de balas
		new_bullet->getBody()->setUserPointer(bullets[bullets.size()-1]);
		new_bullet->getBody()->setCollisionFlags(new_bullet->getBody()->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
		new_bullet->getBody()->setLinearVelocity(15 * shooter->getBody()->getLinearVelocity().normalized()); 
		world->addRigidBody(new_bullet->getBody()); //agrega la bala al mundo
		new_bullet->getBody()->setGravity(btVector3(0,0,0)); //para que las balas no caigan. OBS: esto debe ir despues de agregarse la bala al mundo
		shooter->cooldown = 30; //ponemos el disparo en cd
	}
}

void reducirCooldowns(){
	if (alien1->cooldown > 0)
		(alien1->cooldown)--;
	if (alien2->cooldown > 0)
		(alien2->cooldown)--;
}


//esta funcion se llama cuando hay colision de alguna bala con otro objeto
bool contactAddedCallbackBullet(btManifoldPoint& cp, const btCollisionObjectWrapper * colObj0, int partId0, int index0, const btCollisionObjectWrapper * colObj1, int partId1, int index1){
	std::cout << "COLISIÓN" << std::endl;
	
	bala* bala_atacante = ((bala*)(colObj1->getCollisionObject()->getUserPointer()));

	world->removeRigidBody(bala_atacante->getBody());

	auto it = std::find(bullets.begin(), bullets.end(), bala_atacante); 
   	if (it != bullets.end()) { bullets.erase(it); } //borra la bala del vector de balas

	alien* alien_victima = ((alien*)(colObj0->getCollisionObject()->getUserPointer()));
	alien_victima->hp--;
	std::cout << "HP DEL ALIEN VICTIMA: " << alien_victima->hp << std::endl;
	
	
	return false;
}








