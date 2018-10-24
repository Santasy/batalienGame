#ifndef _GL_UTILS_H
#define _GL_UTILS_H

#include <stdarg.h>
#include <bits/stdc++.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

/* Propiedades de ventana */
extern int g_gl_width;
extern int g_gl_height;
extern GLFWwindow* g_window;

/* Registro */
bool restart_gl_log();
bool gl_log(const char *message, ...);
bool gl_log_err(const char *message, ...);

/* GLFW3 y GLEW */
bool init();
void glfw_error_callback(int error, const char *description);
void glfw_window_size_callback(GLFWwindow *window, int width, int height);
void updateFPS(GLFWwindow *window);

/* Shaders */
bool parse_file_into_str(const char *file_name, char *shader_str, int max_len);
void log_shader_info(GLuint shader_index);
bool create_shader(const char *file_name, GLuint *shader, GLenum type);
void print_programme_info_log(GLuint sp);
bool is_programme_valid(GLuint sp);
bool create_programme(GLuint vert, GLuint frag, GLuint *programme);
GLuint create_programme_from_files(const char *vert_file_name, const char *frag_file_name);

#endif