#ifndef VECTOR3D_GFX
#define VECTOR3D_GFX

#include <stdio.h>
#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "mat4x4.h"
#include "api_tools.h"
#include "textures.h"
#include "math_def.h"

typedef struct {
  unsigned int inds[3];
} face;

typedef enum {
  POS = 1,
  COLOR = 2,
  TEX_COORD = 4,
  TEX_INDEX = 8,
  NORMAL = 16
} layout_attrib;

typedef struct {
  int a_sizes[5];
  unsigned char a; // bit 7 < 0 0 0 NORMAL TEX_INDEX TEX_COORD COLOR POS < bit 0
  int vertex_size;
} layout_table;

typedef struct {
  unsigned int VAO, VBO, IBO, shader, verts_count, inds_count;
} gfx_render_unit;

typedef struct {
  int locked;
  int cleared;
  double prev_mouse_x, prev_mouse_y;
} input_state;

typedef struct {
  //FBO frame;
  int cam_index;
  
  float cam_pitch;
  float cam_yaw;

  vec3 cam_pos;
  vec3 cam_dir;
  vec3 cam_up;
  vec3 cam_right;
  mat4x4 cam_view;
} camera3D;

typedef struct {
  vec3 pos;
  vec3 rot;
  vec3 scale;
  gfx_render_unit buffers;
  layout_table attribs;
} object3D_gfx;

typedef struct {
  // window
  GLFWwindow* window; 
  unsigned int scr_width, scr_height;
  float r, g, b, a; // clear color
  mat4x4 projection_matrix;
  
  // input
  input_state in;
  
  // frame buffers
  FBO* frames;
  camera3D** cameras;
  unsigned int cam_index;
  unsigned int cam_count;
  
  // textures
  Texture* textures;
  unsigned int tex_counter;
  
  // objects
  unsigned int main_VBO, main_IBO, main_VAO, main_shader;
  gfx_render_unit* buffers;
  unsigned int obj_count, max_obj_count;
} vector3D_gfx;

// begin
vector3D_gfx init(const char* iWindowName, unsigned int scr_width, unsigned int scr_height, unsigned int gl_major_version, unsigned int gl_minor_version, unsigned int texture_count, char** texture_list, vec4 clear_color);
void destroy(vector3D_gfx* box);

// rendering
int close_flag(vector3D_gfx* gfx);
void frame_begin_internal(float r, float g, float b, float a);
void frame_begin(vector3D_gfx* gfx);
void frame_end(vector3D_gfx* gfx);
void render(vector3D_gfx* gfx);
void render_wire(vector3D_gfx* gfx);

// camera
void update_camera(vector3D_gfx* box, camera3D* cam);
void create_camera3D(vector3D_gfx* box, camera3D* cam, vec3 pos);
void destroy_camera3D(camera3D* cam);
void get_camera3D_input(vector3D_gfx* box, camera3D* cam_in);
void bind_camera3D(vector3D_gfx* box, camera3D cam);

// objects
void add_to_vector3D_gfx(vector3D_gfx* box, object3D_gfx* obj);
void rotate_obj(object3D_gfx* obj, vec3 axis, float by);
void destroy_obj(object3D_gfx* obj);
object3D_gfx create_obj(vector3D_gfx* box, float* verts, unsigned int verts_count, unsigned int* inds, unsigned int inds_count, layout_table layout, const char* vert_shad_file, const char* frag_shad_file, vec3 pos, vec3 size);
object3D_gfx create_obj_array(vector3D_gfx* box, float* verts, unsigned int float_count, layout_table layout, const char* vert_shad_file, const char* frag_shad_file, vec3 pos, vec3 size);
object3D_gfx create_cube(vector3D_gfx* box, layout_table layout, vec3 pos, vec3 size);
object3D_gfx create_sphere(vector3D_gfx* box, layout_table layout, vec3 pos, float radius);
object3D_gfx create_pyramid(vector3D_gfx* box, layout_table layout, vec3 pos, vec3 size);

// object attributes
layout_table gfx_layout(int opt);

int has_normal(object3D_gfx obj);
int has_color(object3D_gfx obj);
int has_tex_coord(object3D_gfx obj);
int has_tex_index(object3D_gfx obj);

int set_object_property(object3D_gfx* obj, layout_attrib property, float* data, int count);

// utility
int find_str(const char* line, char* str);

#endif
