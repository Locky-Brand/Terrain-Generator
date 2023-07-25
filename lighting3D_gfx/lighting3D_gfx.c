#include "lighting3D_gfx.h"

void set_ambient_light(vector3D_gfx* box, vec4 ambient_light) {
  box->r *= ambient_light.x;
  box->g *= ambient_light.y;
  box->b *= ambient_light.z;
  box->a *= ambient_light.w;

  vec4 ambient = ambient_light;

  for (int i = 0; i < box->obj_count; ++i) {
    set_uniform(box->buffers[i].shader, "ambient_light", UVEC4, (void*)(&ambient));
  }
}

point_light create_point_light(vector3D_gfx* box, lighting3D_gfx* lighting, vec3 pos, vec3 color, float strength) {
  if (lighting->point_lights == NULL) {
    lighting->point_lights = malloc(sizeof(point_light));
  }
  else {
    lighting->point_lights = realloc(lighting->point_lights, (lighting->point_light_count + 1) * sizeof(point_light));
  }

  point_light light = (point_light){pos, strength, color, lighting->point_light_count};
  lighting->point_lights[lighting->point_light_count] = light;

  int count = lighting->point_light_count + 1;

  for (int i = 0; i < box->obj_count; ++i) {
    set_uniform(box->buffers[i].shader, "point_light_count", UI, (void*)(&count));

    char uniform_pos_str[strlen("lights[@@@].pos\0")];
    uniform_pos_str[0] = '\0';
    strcpy(uniform_pos_str, "lights[\0");
    uniform_pos_str[strlen("lights[\0")] = '\0';
    char prop0[] = "].pos\0";

    char uniform_col_str[strlen("lights[@@@].color\0")];
    uniform_col_str[0] = '\0';
    strcpy(uniform_col_str, "lights[\0");
    uniform_col_str[strlen("lights[\0")] = '\0';
    char prop1[] = "].color\0";

    char uniform_strength_str[strlen("lights[@@@].strength\0")];
    uniform_strength_str[0] = '\0';
    strcpy(uniform_strength_str, "lights[\0");
    uniform_strength_str[strlen("lights[\0")] = '\0';
    char prop2[] = "].strength\0";

    char num[4] = "\0\0\0\0";

    int hundreds = (int)(lighting->point_light_count / 100);
    int tens = (int)(lighting->point_light_count / 10) - (hundreds * 100);
    int ones = (int)(lighting->point_light_count) - (tens * 10) - (hundreds * 100);

    int offset = 0;

    if (hundreds) {
      num[offset] = hundreds + '0';
      ++offset;
    }
    if (tens) {
      num[offset] = tens + '0';
      ++offset;
    }
    num[offset] = ones + '0';

    strcat(uniform_col_str, num);
    strcat(uniform_pos_str, num);
    strcat(uniform_strength_str, num);
    strcat(uniform_col_str, prop1);
    strcat(uniform_pos_str, prop0);
    strcat(uniform_strength_str, prop2);
    
    set_uniform(box->buffers[i].shader, uniform_pos_str, UVEC3, (void*)(&light.pos));
    set_uniform(box->buffers[i].shader, uniform_strength_str, UFLOAT, (void*)(&light.strength));
    set_uniform(box->buffers[i].shader, uniform_col_str, UVEC3, (void*)(&light.color));

    set_uniform(box->buffers[i].shader, "cam_pos", UVEC3, (void*)(&box->cameras[box->cam_index]->cam_pos));
  } 

  ++lighting->point_light_count;

  return (point_light){pos, strength, color};
}

void update_lighting(vector3D_gfx* box, lighting3D_gfx* lighting) {
  for (int i = 0; i < box->obj_count; ++i) {
    set_uniform(box->buffers[i].shader, "cam_pos", UVEC3, (void*)(&box->cameras[box->cam_index]->cam_pos));
  }
}

void set_light_property(vector3D_gfx* box, point_light* light, const char* prop, void* data) {
  char uniform_prop_str[strlen("lights[@@@].\0") + strlen(prop)];
  uniform_prop_str[0] = '\0';
  strcpy(uniform_prop_str, "lights[\0");
  uniform_prop_str[strlen("lights[\0")] = '\0';

  char num[4] = "\0\0\0\0";

  int hundreds = (int)(light->light_index / 100);
  int tens = (int)(light->light_index / 10) - (hundreds * 100);
  int ones = (int)(light->light_index) - (tens * 10) - (hundreds * 100);

  int offset = 0;

  if (hundreds) {
    num[offset] = hundreds + '0';
    ++offset;
  }
  if (tens) {
    num[offset] = tens + '0';
    ++offset;
  }
  num[offset] = ones + '0';

  strcat(uniform_prop_str, num);
  strcat(uniform_prop_str, "].\0");
  strcat(uniform_prop_str, prop);
  
  pixel_uniform_data_type type;
  if (!strcmp(prop, "pos\0")) {
    type = UVEC3;
    light->pos = *((vec3*)(data));
  }
  else if (!strcmp(prop, "strength\0")) {
    type = UFLOAT;
    light->strength = *((float*)(data));
  }
  else if (!strcmp(prop, "color\0")) {
    type = UVEC4;
    light->color = *((vec3*)(data));
  }

  for (int i = 0; i < box->obj_count; ++i) {
    set_uniform(box->buffers[i].shader, uniform_prop_str, type, data);
  }
}

void move_light_to(vector3D_gfx* box, point_light* light, vec3 to) {
  vec3 temp = to;

  char uniform_prop_str[strlen("lights[@@@].pos\0")];
  uniform_prop_str[0] = '\0';
  strcpy(uniform_prop_str, "lights[\0");
  uniform_prop_str[strlen("lights[\0")] = '\0';

  char num[4] = "\0\0\0\0";

  int hundreds = (int)(light->light_index / 100);
  int tens = (int)(light->light_index / 10) - (hundreds * 100);
  int ones = (int)(light->light_index) - (tens * 10) - (hundreds * 100);

  int offset = 0;

  if (hundreds) {
    num[offset] = hundreds + '0';
    ++offset;
  }
  if (tens) {
    num[offset] = tens + '0';
    ++offset;
  }
  num[offset] = ones + '0';

  strcat(uniform_prop_str, num);
  strcat(uniform_prop_str, "].pos\0");

  for (int i = 0; i < box->obj_count; ++i) {
    set_uniform(box->buffers[i].shader, uniform_prop_str, UVEC3, (void*)(&temp));
  }
}
