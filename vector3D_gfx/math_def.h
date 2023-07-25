#ifndef MATH_DEF_H
#define MATH_DEF_H

#include <math.h>

#define PI 3.14159

typedef struct {
  float x, y, z, w;
} vec4;

typedef struct {
  float x, y, z;
} vec3;

vec3 sub_vec3(vec3 a, vec3 b);

typedef struct {
  float* elems;
} mat4x4;

typedef struct {
  float elems[16];
} mat4x4_temp;

float length(vec3 vec);
void normalize(vec3* vec);
vec4 cross3_vec4(vec4 a, vec4 b);
vec3 cross(vec3 a, vec3 b);
float dot(vec3 a, vec3 b);

void mul_tmat4x4_vec4(mat4x4_temp mat, vec4* vec);
void mul_vec4_tmat4x4(vec4* vec, mat4x4_temp mat);

float map(float val, float from_min, float from_max, float to_min, float to_max);

int same_direction(vec3 a, vec3 b);

float linear_interpolate(float x0, float x1, float alpha);

#endif
