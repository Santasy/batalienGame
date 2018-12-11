#ifndef _TOOLS_H_
#define _TOOLS_H_

#include <bits/stdc++.h>
#include <GL/glew.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

bool load_mesh(const char *file_name, GLuint* vao, int *point_count);
//bool load_collider(const char *file_name, int *point_count);//por mientras
bool gameplay(float cam_speed, double elapsed_seconds, float *cam_pos, float *cam_yaw, float cam_yaw_speed);

#endif