SRC=src/*.cpp src/stb_image.c
LIBS=-lGL -lGLEW -lglfw -lassimp -lLinearMath -lBulletDynamics -lBulletCollision
INC=-I/usr/include/bullet
EXEC=bin/batalien

all:
	g++ ${SRC} ${INC} ${LIBS} -o ${EXEC}

clear:
	rm bin/*
