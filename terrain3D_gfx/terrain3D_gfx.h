#ifndef TERRAIN_H
#define TERRAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "../../../Things/vector3D_gfx/vector3D_gfx.h"

typedef struct {
  vec3 a; // POS
  vec4 c; // COLOR
  vec3 b; // NORMAL
} terrain_vertex;

typedef struct {
  terrain_vertex* verts;
  int noise_width, noise_height;
  object3D_gfx obj;
} terrain_mesh;

typedef struct {
  float* noise_data;
  int noise_width, noise_height;
} terrain_noise;

typedef struct {
  unsigned char* data;
  int width, height;
  unsigned int tex;
  unsigned int VBO, VAO, IBO, shader;
} perlin_dbg_tex;

void create_perlin_dbg_tex(perlin_dbg_tex* dbg, unsigned int* tex_counter);
void draw_perlin_dbg_tex(perlin_dbg_tex dbg);

void generate_kth_octave(float* noise, float* out, int k, int noise_width, int noise_height);

terrain_noise generate_noise(int noise_width, int noise_height, int octave_count, float bias);

terrain_mesh create_terrain_plane(vector3D_gfx* box, float quad_area, unsigned int pt_width, unsigned int pt_height, float max_height, int octave_count, float persistence, float elevation, perlin_dbg_tex* dbg);

terrain_mesh create_flat_plane(vector3D_gfx* box, float quad_area, unsigned int pt_width, unsigned int pt_height, float elevation, vec4 color);

#endif
