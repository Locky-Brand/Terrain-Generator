#ifndef LIGHTING3D_GFX
#define LIGHTING3D_GFX

#include <stdlib.h>

#include "../../../Things/vector3D_gfx/vector3D_gfx.h"

typedef struct {
  vec3 pos;
  float strength;
  vec3 color;
  int light_index;
} point_light;

typedef struct {
  vec4 ambient_light;

  unsigned int point_light_count;
  point_light* point_lights;
} lighting3D_gfx;

void set_ambient_light(vector3D_gfx* box, vec4 ambient_light);

point_light create_point_light(vector3D_gfx* box, lighting3D_gfx* lighting, vec3 pos, vec3 color, float strength);

void update_lighting(vector3D_gfx* box, lighting3D_gfx* lighting);

void set_light_property(vector3D_gfx* box, point_light* light, const char* prop, void* data);

void move_light_to(vector3D_gfx* box, point_light* light, vec3 to);

#endif
