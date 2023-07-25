#include "mat4x4.h"

void identity_temp(mat4x4_temp* ret) {
  memset(ret->elems, 0.0f, 16 * sizeof(float));
  ret->elems[0] = 1.0f;
  ret->elems[5] = 1.0f;
  ret->elems[10] = 1.0f;
  ret->elems[15] = 1.0f;
}
void zero_temp(mat4x4_temp* ret) {
  memset(ret->elems, 0.0f, 16 * sizeof(float));
}
void scale_temp(mat4x4_temp* ret, float x, float y, float z) {
  memset(ret->elems, 0.0f, 16 * sizeof(float));
  ret->elems[0] = x;
  ret->elems[5] = y;
  ret->elems[10] = z;
  ret->elems[15] = 1.0f;
}
void translate_temp(mat4x4_temp* ret, float x, float y, float z) {
  memset(ret->elems, 0.0f, 16 * sizeof(float));
  ret->elems[0] = 1.0f;
  ret->elems[5] = 1.0f;
  ret->elems[10] = 1.0f;
  ret->elems[3] = x;
  ret->elems[7] = y;
  ret->elems[11] = z;
  ret->elems[15] = 1.0f;
}
void rotate_temp(mat4x4_temp* ret, float x, float y, float z, float theta) {
  memset(ret->elems, 0.0f, 16 * sizeof(float));
  
  float ci = cos(theta);
  float cii = 1 - cos(theta);
  float s = sin(theta);

  ret->elems[0] = ci + (cii) * (x * x);
  ret->elems[1] = (cii) * x * y - s * z;
  ret->elems[2] = (cii) * x * z + s * y;
  ret->elems[3] = 0.0f;

  ret->elems[4] = (cii) * x * y + s * z;
  ret->elems[5] = ci + (cii) * (y * y);
  ret->elems[6] = (cii) * y * z - s * x;
  ret->elems[7] = 0.0f;

  ret->elems[8] = (cii) * x * z - s * y;
  ret->elems[9] = (cii) * y * z + s * x;
  ret->elems[10] = ci + (cii) * (z * z);
  ret->elems[11] = 0.0f;

  ret->elems[15] = 1.0f;
}

void view_temp(mat4x4_temp* ret, vec3 pos, vec3 right, vec3 up, vec3 dir) {
  memset(ret->elems, 0.0f, 16 * sizeof(float));

  ret->elems[0] = right.x;
  ret->elems[1] = right.y;
  ret->elems[2] = right.z;
  ret->elems[3] = -right.x * pos.x - right.y * pos.y - right.z * pos.z;

  ret->elems[4] = up.x;
  ret->elems[5] = up.y;
  ret->elems[6] = up.z;
  ret->elems[7] = -up.x * pos.x - up.y * pos.y - up.z * pos.z;

  ret->elems[8] = dir.x;
  ret->elems[9] = dir.y;
  ret->elems[10] = dir.z;
  ret->elems[11] = -dir.x * pos.x - dir.y * pos.y - dir.z * pos.z;

  ret->elems[15] = 1.0f;
}

void view_set(mat4x4* ret, vec3 pos, vec3 right, vec3 up, vec3 dir) {
  memset(ret->elems, 0.0f, 16 * sizeof(float));

  ret->elems[0] = right.x;
  ret->elems[1] = right.y;
  ret->elems[2] = right.z;
  ret->elems[3] = -right.x * pos.x - right.y * pos.y - right.z * pos.z;

  ret->elems[4] = up.x;
  ret->elems[5] = up.y;
  ret->elems[6] = up.z;
  ret->elems[7] = -up.x * pos.x - up.y * pos.y - up.z * pos.z;

  ret->elems[8] = dir.x;
  ret->elems[9] = dir.y;
  ret->elems[10] = dir.z;
  ret->elems[11] = -dir.x * pos.x - dir.y * pos.y - dir.z * pos.z;

  ret->elems[15] = 1.0f;
}
void zero_set(mat4x4* mat) {
  memset(mat->elems, 0.0f, 16 * sizeof(float));
}

mat4x4 scale(mat4x4* mat, float x, float y, float z) {
  mat4x4 ret;
  if (mat != NULL) {
    ret.elems = mat->elems;
  }
  else {
    ret.elems = malloc(16 * sizeof(float));
  }
  memset(ret.elems, 0.0f, 16 * sizeof(float));
  ret.elems[0] = x;
  ret.elems[5] = y;
  ret.elems[10] = z;
  ret.elems[15] = 1.0f;
  return ret;
}
mat4x4 translate(mat4x4* mat, float x, float y, float z) {
  mat4x4 ret;
  if (mat != NULL) {
    ret.elems = mat->elems;
  }
  else {
    ret.elems = malloc(16 * sizeof(float));
  }
  memset(ret.elems, 0.0f, 16 * sizeof(float));
  ret.elems[0] = 1.0f;
  ret.elems[5] = 1.0f;
  ret.elems[10] = 1.0f;
  ret.elems[3] = x;
  ret.elems[7] = y;
  ret.elems[11] = z;
  ret.elems[15] = 1.0f;
  return ret;
}
mat4x4 rotate(mat4x4* mat, float x, float y, float z, float theta) {  
  mat4x4 ret;
  if (mat != NULL) {
    ret.elems = mat->elems;
  }
  else {
    ret.elems = malloc(16 * sizeof(float));
  }
  memset(ret.elems, 0.0f, 16 * sizeof(float));
  
  float ci = cos(theta);
  float cii = 1 - cos(theta);
  float s = sin(theta);

  ret.elems[0] = ci + (cii) * (x * x);
  ret.elems[1] = (cii) * x * y - s * z;
  ret.elems[2] = (cii) * x * z + s * y;
  ret.elems[3] = 0.0f;

  ret.elems[4] = (cii) * x * y + s * z;
  ret.elems[5] = ci + (cii) * (y * y);
  ret.elems[6] = (cii) * y * z - s * x;
  ret.elems[7] = 0.0f;

  ret.elems[8] = (cii) * x * z - s * y;
  ret.elems[9] = (cii) * y * z + s * x;
  ret.elems[10] = ci + (cii) * (z * z);
  ret.elems[11] = 0.0f;

  ret.elems[15] = 1.0f;
  return ret;
}

mat4x4 view(mat4x4* mat, vec3 pos, vec3 right, vec3 up, vec3 dir){ 
  mat4x4 ret;
  if (mat != NULL) {
    ret.elems = mat->elems;
  }
  else {
    ret.elems = malloc(16 * sizeof(float));
  }
  memset(ret.elems, 0.0f, 16 * sizeof(float));

  ret.elems[0] = right.x;
  ret.elems[1] = right.y;
  ret.elems[2] = right.z;
  ret.elems[3] = -right.x * pos.x - right.y * pos.y - right.z * pos.z;

  ret.elems[4] = up.x;
  ret.elems[5] = up.y;
  ret.elems[6] = up.z;
  ret.elems[7] = -up.x * pos.x - up.y * pos.y - up.z * pos.z;

  ret.elems[8] = dir.x;
  ret.elems[9] = dir.y;
  ret.elems[10] = dir.z;
  ret.elems[11] = -dir.x * pos.x - dir.y * pos.y - dir.z * pos.z;

  ret.elems[15] = 1.0f;
  return ret;
}

mat4x4 perspective(float fov, float z_near, float view_dist, unsigned int scr_width, unsigned int scr_height) {
  mat4x4 ret;
  ret.elems = malloc(16 * sizeof(float));
  memset(ret.elems, 0.0f, 16 * sizeof(float));

  float aspect = scr_width / scr_height;

  ret.elems[0] = (1/(aspect * tanf(0.5f * fov)));
  ret.elems[5] = (1/tanf(0.5f * fov));
  ret.elems[10] = (-z_near - view_dist)/(z_near - view_dist);
  ret.elems[11] = (2 * view_dist * z_near)/(z_near - view_dist);
  ret.elems[14] = 1.0f;

  return ret;
}
mat4x4 identity(mat4x4* mat) {
  mat4x4 ret;
  if (mat != NULL) {
    ret.elems = mat->elems;
  }
  else {
    ret.elems = malloc(16 * sizeof(float));
  }
  memset(ret.elems, 0.0f, 16 * sizeof(float));
  ret.elems[0] = 1.0f;
  ret.elems[5] = 1.0f;
  ret.elems[10] = 1.0f;
  ret.elems[15] = 1.0f;
  return ret;
}
mat4x4 zero(mat4x4* mat) {
  mat4x4 ret;
  if (mat != NULL) {
    ret.elems = mat->elems;
  }
  else {
    ret.elems = malloc(16 * sizeof(float));
  }
  memset(ret.elems, 0.0f, 16 * sizeof(float));
  return ret;
}

void delete_mat4x4(mat4x4* mat) {
  free(mat->elems);
  mat->elems = NULL;
}

void debug_print_mat4x4(mat4x4 mat) {
  for (int width = 0; width < 4; ++width) {
    for (int height = 0; height < 4; ++height) {
      printf("%f ", mat.elems[width * 4 + height]);
    }
    printf("\n");
  }
}

void mul_mat4x4(mat4x4 A, mat4x4 B, mat4x4* C) {
  for (int m = 0; m < 4; m++) {
    for (int k = 0; k < 4; k++) {
      for (int n = 0; n < 4; n++) {
        C->elems[m * 4  + n] += A.elems[m * 4 + k] * B.elems[k * 4 + n];
      }
    }
  }
}
void mul_mat4x4_t(mat4x4 A, mat4x4_temp B, mat4x4* C) {
  for (int m = 0; m < 4; m++) {
    for (int k = 0; k < 4; k++) {
      for (int n = 0; n < 4; n++) {
        C->elems[m * 4  + n] += A.elems[m * 4 + k] * B.elems[k * 4 + n];
      }
    }
  }
}
void mul_mat4x4_ti(mat4x4_temp A, mat4x4_temp B, mat4x4* C) {
  for (int m = 0; m < 4; m++) {
    for (int k = 0; k < 4; k++) {
      for (int n = 0; n < 4; n++) {
        C->elems[m * 4  + n] += A.elems[m * 4 + k] * B.elems[k * 4 + n];
      }
    }
  }
}
void mul_mat4x4_tii(mat4x4_temp A, mat4x4_temp B, mat4x4_temp* C) {
  for (int m = 0; m < 4; m++) {
    for (int k = 0; k < 4; k++) {
      for (int n = 0; n < 4; n++) {
        C->elems[m * 4  + n] += A.elems[m * 4 + k] * B.elems[k * 4 + n];
      }
    }
  }
}
