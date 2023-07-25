#ifndef MAT4X4_H
#define MAT4X4_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "math_def.h"

void identity_temp(mat4x4_temp* ret);
void zero_temp(mat4x4_temp* ret);
void scale_temp(mat4x4_temp* ret, float x, float y, float z);
void translate_temp(mat4x4_temp* ret, float x, float y, float z);
void rotate_temp(mat4x4_temp* ret, float x, float y, float z, float theta);

void view_temp(mat4x4_temp* ret, vec3 pos, vec3 right, vec3 up, vec3 dir);

void view_set(mat4x4* ret, vec3 pos, vec3 right, vec3 up, vec3 dir);
void zero_set(mat4x4* mat);

mat4x4 identity(mat4x4* mat);
mat4x4 zero(mat4x4* mat);
mat4x4 scale(mat4x4* mat, float x, float y, float z);
mat4x4 translate(mat4x4* mat, float x, float y, float z);
mat4x4 rotate(mat4x4* mat, float x, float y, float z, float theta);

mat4x4 view(mat4x4* mat, vec3 pos, vec3 right, vec3 up, vec3 dir);
mat4x4 perspective(float fov, float z_near, float view_dist, unsigned int scr_width, unsigned int scr_height);

void delete_mat4x4(mat4x4* mat);

void debug_print_mat4x4(mat4x4 mat);

void mul_mat4x4(mat4x4 A, mat4x4 B, mat4x4* C);

void mul_mat4x4_t(mat4x4 A, mat4x4_temp B, mat4x4* C);
void mul_mat4x4_ti(mat4x4_temp A, mat4x4_temp B, mat4x4* C);
void mul_mat4x4_tii(mat4x4_temp A, mat4x4_temp B, mat4x4_temp* C);

#endif
