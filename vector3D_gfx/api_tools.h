#ifndef API_TOOLS_H
#define API_TOOLS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

typedef struct{
  float x, y;
} vec2;

typedef enum{
        UFLOAT, // float
        UVEC2,  // vec2
        UVEC3,  // vec3
        UVEC4,  // vec4
        UI,  // int
        UUI  // unsigned int
} pixel_uniform_data_type;

typedef struct {
  unsigned int width;
  unsigned int height;

  unsigned int ID;
  unsigned int texID;
} FBO;

typedef struct {
  unsigned int size;
  char* data;
} file_data;

unsigned int create_shader(const char* vertShaderSrc, const char* fragShaderSrc);
unsigned int create_shader_file(const char* vert_file_path, const char* frag_file_path);

file_data load_file(const char* path);
void load_file_part(char* content, const char* after_line);
unsigned int get_file_size(const char* path);
void close_file(file_data file);

bool set_uniform(unsigned int shader, const char* name, pixel_uniform_data_type type, void* data);
bool set_uniform_mat4x4(unsigned int shader, const char* name, float* data);
bool set_uniform_array(unsigned int shader, const char* name, pixel_uniform_data_type type, unsigned int count, void* data);

unsigned int create_empty_tex(unsigned int width, unsigned int height, unsigned int* tex_counter);
void populate_tex(unsigned int tex, unsigned int x_off, unsigned int y_off, unsigned int width, unsigned int height, unsigned char* data);
FBO create_FBO(unsigned int width, unsigned int height, unsigned int* tex_counter);

#endif
