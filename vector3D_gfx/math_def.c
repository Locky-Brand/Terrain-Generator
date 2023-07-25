#include "math_def.h"

vec3 sub_vec3(vec3 a, vec3 b) {
  return (vec3){a.x - b.x, a.y - b.y, a.z - b.z};
}

float length(vec3 vec) {
  return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z); 
}
void normalize(vec3* vec) {
  float len = length((*vec));
  vec->x /= len;
  vec->y /= len;
  vec->z /= len;
}
vec4 cross3_vec4(vec4 a, vec4 b) {
  return (vec4){(a.y * b.z - a.z * b.y), (a.z * b.x - a.x * b.z), (a.x * b.y - a.y * b.z), a.w};
}
vec3 cross(vec3 a, vec3 b) {
  return (vec3){(a.y * b.z - a.z * b.y), (a.z * b.x - a.x * b.z), (a.x * b.y - a.y * b.z)};
}


void mul_tmat4x4_vec4(mat4x4_temp mat, vec4* vec) {
  float x = mat.elems[0] * vec->x + mat.elems[1] * vec->y + mat.elems[2] * vec->z + mat.elems[3] * vec->w;
  float y = mat.elems[4] * vec->x + mat.elems[5] * vec->y + mat.elems[6] * vec->z + mat.elems[7] * vec->w;
  float z = mat.elems[8] * vec->x + mat.elems[9] * vec->y + mat.elems[10] * vec->z + mat.elems[11] * vec->w;
  float w = mat.elems[12] * vec->x + mat.elems[13] * vec->y + mat.elems[14] * vec->z + mat.elems[15] * vec->w;
  (*vec) = (vec4){x, y, z, w}; 
}
void mul_vec4_tmat4x4(vec4* vec, mat4x4_temp mat) {
  float x = mat.elems[0] * vec->x + mat.elems[4] * vec->y + mat.elems[8] * vec->z + mat.elems[12] * vec->w;
  float y = mat.elems[1] * vec->x + mat.elems[5] * vec->y + mat.elems[9] * vec->z + mat.elems[13] * vec->w;
  float z = mat.elems[2] * vec->x + mat.elems[6] * vec->y + mat.elems[10] * vec->z + mat.elems[14] * vec->w;
  float w = mat.elems[3] * vec->x + mat.elems[7] * vec->y + mat.elems[11] * vec->z + mat.elems[15] * vec->w;
  (*vec) = (vec4){x, y, z, w}; 
}

float dot(vec3 a, vec3 b){
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

float map(float val, float from_min, float from_max, float to_min, float to_max) {
  float percent = val / (from_max - from_min);
  return (percent * (to_max - to_min));
}

int same_direction(vec3 a, vec3 b) {
  return (dot(a, b) > 0);
}

float linear_interpolate(float x0, float x1, float alpha) {
  return (float)((1 - alpha) * x0 + (alpha) * x1);
}
