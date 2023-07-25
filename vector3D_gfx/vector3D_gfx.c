#include "vector3D_gfx.h"

vector3D_gfx init(const char* iWindowName, unsigned int scr_width, unsigned int scr_height, unsigned int gl_major_version, unsigned int gl_minor_version, unsigned int texture_count, char** texture_list, vec4 clear_color){
  vector3D_gfx gfx;
  gfx.tex_counter = 0;
  gfx.obj_count = 0;
  gfx.scr_width = scr_width;
  gfx.scr_height = scr_height;
  //gfx.max_obj_count = max_obj_count;
  gfx.frames = malloc(sizeof(FBO));
  gfx.textures = NULL;
  if (texture_count) {
    gfx.textures = malloc(texture_count * sizeof(Texture));
  }
  gfx.r = clear_color.x;
  gfx.g = clear_color.y;
  gfx.b = clear_color.z;
  gfx.a = clear_color.w;
  gfx.buffers = malloc(sizeof(gfx_render_unit));

  gfx.cameras = malloc(sizeof(camera3D*));
  camera3D* default_cam = malloc(sizeof(camera3D));
  default_cam->cam_index = 0;
  create_camera3D(NULL, default_cam, (vec3){0.0f, 5.0f, 3.0f});
  gfx.cameras[0] = default_cam;
  gfx.cam_index = 0;
  gfx.cam_count = 1;

  gfx.in = (input_state){0, 1, 0.0, 0.0};

  gfx.projection_matrix = perspective(PI/4.0f, 1.0f, 100.0f, 400, 400);

  // glfw: initialize and configure
  // ------------------------------
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, gl_major_version);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, gl_minor_version);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // glfw window creation
  // --------------------
  GLFWwindow* window = glfwCreateWindow(scr_width, scr_height, iWindowName, NULL, NULL);
  if (window == NULL)
  {
    printf("%s\n", "Failed to create GLFW window");
    glfwTerminate();
    while(1);
  }
  glfwMakeContextCurrent(window);

  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    printf("%s\n", "Failed to initialize GLAD");
    while(1);
  }

  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

  gfx.window = window;

  (*(gfx.frames)) = create_FBO(scr_width, scr_height, &gfx.tex_counter);

  for (int i = 0; i < texture_count; ++i) {
    gfx.textures[i] = load_texture(texture_list[i], &gfx.tex_counter, (find_str(texture_list[i], "png\0") != -1));
  }

  // create main frame buffer
  float verts[] = {
    -1.0f, -1.0f, 0.0f, 0.0f,
     1.0f, -1.0f, 1.0f, 0.0f,
     1.0f,  1.0f, 1.0f, 1.0f,
    -1.0f,  1.0f, 0.0f, 1.0f
  };
  unsigned int inds1[] = {
    0, 1, 2, 2, 3, 0
  };

  glGenBuffers(1, &gfx.main_VBO);
  glGenVertexArrays(1, &gfx.main_VAO);

  glBindVertexArray(gfx.main_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, gfx.main_VBO);
  glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), verts, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(0 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  glGenBuffers(1, &gfx.main_IBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gfx.main_IBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), inds1, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  {
    const char* main_vertex = "#version 330 core\n"
                         "layout(location = 0) in vec2 aPos;\n"
                         "layout(location = 1) in vec2 aTex;\n"
                         "out vec2 oTex;\n"
                         "void main(){\n"
                         "oTex = aTex;\n"
                         "gl_Position = vec4(aPos, 0.0f, 1.0f);\n"
                         "};\n\0";
    const char* main_fragment = "#version 330 core\n"
                         "out vec4 aColor;\n"
                         "in vec2 oTex;\n"
                         "uniform sampler2D main_frame;\n"
                         "void main(){\n"
                         "aColor = texture(main_frame, oTex);\n"
                         "};\n\0";

    gfx.main_shader = create_shader(main_vertex, main_fragment);

    int temp = 0;
    set_uniform(gfx.main_shader, "main_frame", UI, (void*)(&temp));
  }

  //glEnable(GL_CULL_FACE);
  //glCullFace(GL_BACK);

  return gfx;
}
void destroy(vector3D_gfx* box) {
  free(box->frames);
  free(box->buffers);
  if (box->textures != NULL) {
    free(box->textures);
  }
  delete_mat4x4(&box->projection_matrix);
  destroy_camera3D(box->cameras[0]);
  free(box->cameras[0]);
  free(box->cameras);
}

int close_flag(vector3D_gfx* gfx) {
  return glfwWindowShouldClose(gfx->window);
}

void frame_begin_internal(float r, float g, float b, float a) {
  glClearColor(r, g, b, a);
  glClear(GL_COLOR_BUFFER_BIT);
}

void frame_begin(vector3D_gfx* gfx) {
  glBindFramebuffer(GL_FRAMEBUFFER, gfx->frames->ID);
  glClearColor(gfx->r, gfx->g, gfx->b, gfx->a);
  glClear(GL_COLOR_BUFFER_BIT);
}

void frame_end(vector3D_gfx* gfx) {
  glfwPollEvents();
  glfwSwapBuffers(gfx->window);
}

void render(vector3D_gfx* gfx) {
  update_camera(gfx, gfx->cameras[gfx->cam_index]);

  glClear(GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  for (int i = 0; i < gfx->obj_count; ++i) {
    glBindVertexArray(gfx->buffers[i].VAO);
    glBindBuffer(GL_ARRAY_BUFFER, gfx->buffers[i].VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gfx->buffers[i].IBO);
    glUseProgram(gfx->buffers[i].shader);
    switch (gfx->buffers[i].IBO) {
      case 0:
        glDrawArrays(GL_TRIANGLES, 0, gfx->buffers[i].verts_count);
      break;
      default:
        glDrawElements(GL_TRIANGLES, gfx->buffers[i].inds_count, GL_UNSIGNED_INT, NULL);
      break;
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glUseProgram(0);
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  frame_begin_internal(gfx->r, gfx->g, gfx->b, gfx->a);
  glDisable(GL_DEPTH_TEST);

  glBindVertexArray(gfx->main_VAO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gfx->main_IBO);
  glUseProgram(gfx->main_shader);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glUseProgram(0);
}

void render_wire(vector3D_gfx* gfx) {
  update_camera(gfx, gfx->cameras[gfx->cam_index]);

  glClear(GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  for (int i = 0; i < gfx->obj_count; ++i) {
    glBindVertexArray(gfx->buffers[i].VAO);
    glBindBuffer(GL_ARRAY_BUFFER, gfx->buffers[i].VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gfx->buffers[i].IBO);
    glUseProgram(gfx->buffers[i].shader);
    switch (gfx->buffers[i].IBO) {
      case 0:
        glDrawArrays(GL_LINE_LOOP, 0, gfx->buffers[i].verts_count);
      break;
      default:
        glDrawElements(GL_LINE_LOOP, gfx->buffers[i].inds_count, GL_UNSIGNED_INT, NULL);
      break;
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glUseProgram(0);
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  frame_begin_internal(gfx->r, gfx->g, gfx->b, gfx->a);
  glDisable(GL_DEPTH_TEST);

  glBindVertexArray(gfx->main_VAO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gfx->main_IBO);
  glUseProgram(gfx->main_shader);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glUseProgram(0);
}

void update_camera(vector3D_gfx* box, camera3D* cam) {

  view_set(&box->cameras[box->cam_index]->cam_view, box->cameras[box->cam_index]->cam_pos, box->cameras[box->cam_index]->cam_right, box->cameras[box->cam_index]->cam_up, box->cameras[box->cam_index]->cam_dir);

  for (int i = 0; i < box->obj_count; ++i) {
    set_uniform_mat4x4(box->buffers[i].shader, "view", cam->cam_view.elems);
  }
}
void bind_camera3D(vector3D_gfx* box, camera3D cam) {
  box->cam_index = cam.cam_index;
}

void add_to_vector3D_gfx(vector3D_gfx* box, object3D_gfx* obj) {
  box->buffers = realloc(box->buffers, (box->obj_count + 1) * sizeof(gfx_render_unit));
  box->buffers[box->obj_count] = obj->buffers;
  set_uniform_mat4x4(box->buffers[box->obj_count].shader, "proj", box->projection_matrix.elems);
  ++box->obj_count;
}

object3D_gfx create_obj(vector3D_gfx* box, float* verts, unsigned int verts_count, unsigned int* inds, unsigned int inds_count, layout_table layout, const char* vert_shad_file, const char* frag_shad_file, vec3 pos, vec3 size) {
  object3D_gfx obj;

  obj.attribs = layout;

  glGenBuffers(1, &obj.buffers.VBO);
  glGenVertexArrays(1, &obj.buffers.VAO);

  glBindVertexArray(obj.buffers.VAO);
  glBindBuffer(GL_ARRAY_BUFFER, obj.buffers.VBO);
  glBufferData(GL_ARRAY_BUFFER, verts_count * sizeof(float), verts, GL_DYNAMIC_DRAW);

  int vertex_size = 0;
  for (int i = 0; i < 5; ++i) {
    vertex_size += layout.a_sizes[i];
  }

  int accum = 0;
  int index = 0;
  for (int i = 0; i < 5; ++i) {
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, layout.a_sizes[i], GL_FLOAT, GL_FALSE, vertex_size * sizeof(float), (void*)(accum * sizeof(float)));
    index += (layout.a_sizes[i])?(1):(0);
    accum += layout.a_sizes[i];
  }

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  unsigned int* indicies = inds;
  unsigned int indicies_count = inds_count;

  if (indicies == NULL) {
    indicies_count = verts_count / vertex_size;
    indicies = malloc(indicies_count * sizeof(unsigned int));
    for (int i = 0; i < indicies_count; ++i) {
      indicies[i] = i;
    }
  }

  glGenBuffers(1, &obj.buffers.IBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.buffers.IBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicies_count * sizeof(unsigned int), indicies, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  obj.buffers.shader = create_shader_file(vert_shad_file, frag_shad_file);

  if (inds == NULL) {
    free(indicies);
  }

  obj.pos = pos;
  obj.rot = (vec3){0.0f, 0.0f, 0.0f};
  obj.scale = size;

  mat4x4 translator = translate(NULL, obj.pos.x, obj.pos.y, obj.pos.z);
  mat4x4 rotator = identity(NULL);
  mat4x4 scalor = scale(NULL, obj.scale.x, obj.scale.y, obj.scale.z);

  set_uniform_mat4x4(obj.buffers.shader, "translator", translator.elems);
  set_uniform_mat4x4(obj.buffers.shader, "rotator", rotator.elems);
  set_uniform_mat4x4(obj.buffers.shader, "scalor", scalor.elems);

  delete_mat4x4(&translator);
  delete_mat4x4(&rotator);
  delete_mat4x4(&scalor);

  obj.buffers.inds_count = indicies_count;

  add_to_vector3D_gfx(box, &obj);

  return obj;
}

object3D_gfx create_obj_array(vector3D_gfx* box, float* verts, unsigned int float_count, layout_table layout, const char* vert_shad_file, const char* frag_shad_file, vec3 pos, vec3 size) {
  object3D_gfx obj;

  obj.attribs = layout;

  obj.buffers.IBO = 0;
  glGenBuffers(1, &obj.buffers.VBO);
  glGenVertexArrays(1, &obj.buffers.VAO);

  glBindVertexArray(obj.buffers.VAO);
  glBindBuffer(GL_ARRAY_BUFFER, obj.buffers.VBO);
  glBufferData(GL_ARRAY_BUFFER, float_count * sizeof(float), verts, GL_DYNAMIC_DRAW);

  int vertex_size = 0;
  for (int i = 0; i < 5; ++i) {
    vertex_size += layout.a_sizes[i];
  }

  int accum = 0;
  int index = 0;
  for (int i = 0; i < 5; ++i) {
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, layout.a_sizes[i], GL_FLOAT, GL_FALSE, vertex_size * sizeof(float), (void*)(accum * sizeof(float)));
    index += (layout.a_sizes[i])?(1):(0);
    accum += layout.a_sizes[i];
  }

  obj.buffers.verts_count = float_count / vertex_size;

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  obj.buffers.shader = create_shader_file(vert_shad_file, frag_shad_file);

  obj.pos = pos;
  obj.rot = (vec3){0.0f, 0.0f, 0.0f};
  obj.scale = size;

  mat4x4 translator = translate(NULL, obj.pos.x, obj.pos.y, obj.pos.z);
  mat4x4 rotator = identity(NULL);
  mat4x4 scalor = scale(NULL, obj.scale.x, obj.scale.y, obj.scale.z);

  set_uniform_mat4x4(obj.buffers.shader, "translator", translator.elems);
  set_uniform_mat4x4(obj.buffers.shader, "rotator", rotator.elems);
  set_uniform_mat4x4(obj.buffers.shader, "scalor", scalor.elems);

  delete_mat4x4(&translator);
  delete_mat4x4(&rotator);
  delete_mat4x4(&scalor);

  add_to_vector3D_gfx(box, &obj);

  return obj;
}

object3D_gfx create_cube(vector3D_gfx* box, layout_table layout, vec3 pos, vec3 size) {
  object3D_gfx obj;

  int norm_flag = layout.a & 0b00010000;

  vec4 color = (vec4){0.5f, 0.5f, 0.5f, 1.0f};
  vec3 norms[] = {
    (vec3){0.0f, 0.0f, -1.0f}, // back
    (vec3){0.0f, 0.0f, 1.0f}, // front
    (vec3){-1.0f, 0.0f, 0.0f}, // west
    (vec3){1.0f, 0.0f, 0.0f}, // east
    (vec3){0.0f, -1.0f, 0.0f}, // south
    (vec3){0.0f, 1.0f, 0.0f} // north
  };
  vec3 cube_verts[] = {
    (vec3){-0.5f, -0.5f, -0.5f}, // back
     (vec3){0.5f, -0.5f, -0.5f},
     (vec3){0.5f,  0.5f, -0.5f},
     (vec3){0.5f,  0.5f, -0.5f},
    (vec3){-0.5f,  0.5f, -0.5f},
    (vec3){-0.5f, -0.5f, -0.5f},

    (vec3){-0.5f, -0.5f,  0.5f}, // front
     (vec3){0.5f, -0.5f,  0.5f},
     (vec3){0.5f,  0.5f,  0.5f},
     (vec3){0.5f,  0.5f,  0.5f},
    (vec3){-0.5f,  0.5f,  0.5f},
    (vec3){-0.5f, -0.5f,  0.5f},

    (vec3){-0.5f,  0.5f,  0.5f}, // west
    (vec3){-0.5f,  0.5f, -0.5f},
    (vec3){-0.5f, -0.5f, -0.5f},
    (vec3){-0.5f, -0.5f, -0.5f},
    (vec3){-0.5f, -0.5f,  0.5f},
    (vec3){-0.5f,  0.5f,  0.5f},

     (vec3){0.5f,  0.5f,  0.5f}, // east
     (vec3){0.5f,  0.5f, -0.5f},
     (vec3){0.5f, -0.5f, -0.5f},
     (vec3){0.5f, -0.5f, -0.5f},
     (vec3){0.5f, -0.5f,  0.5f},
     (vec3){0.5f,  0.5f,  0.5f},

    (vec3){-0.5f, -0.5f, -0.5f}, // south
     (vec3){0.5f, -0.5f, -0.5f},
     (vec3){0.5f, -0.5f,  0.5f},
     (vec3){0.5f, -0.5f,  0.5f},
    (vec3){-0.5f, -0.5f,  0.5f},
    (vec3){-0.5f, -0.5f, -0.5f},

    (vec3){-0.5f,  0.5f, -0.5f}, // north
     (vec3){0.5f,  0.5f, -0.5f},
     (vec3){0.5f,  0.5f,  0.5f},
     (vec3){0.5f,  0.5f,  0.5f},
    (vec3){-0.5f,  0.5f,  0.5f},
    (vec3){-0.5f,  0.5f, -0.5f}
  };
  int float_count = 36 * 3;

  vec3* verts = cube_verts;

  if (norm_flag) {
    verts = malloc(36 * 2 * sizeof(vec3));

    int j = 0;
    int face_index = 0;
    int face = 0;
    for (int i = 0; i < 36 * 2; i += 2) {
      verts[i] = cube_verts[j];
      verts[i + 1] = norms[face_index];
      ++j;
      ++face;
      if (face == 6) {
        face = 0;
        ++face_index;
      }
    }
    float_count = 36 * 6;
  }

  obj = create_obj(box, (float*)verts, float_count, NULL, 0, layout, "./vector3D_gfx/box_vert_shader", "./vector3D_gfx/box_frag_shader", pos, size);

  if (norm_flag) {
    free(verts);
  }

  set_uniform(obj.buffers.shader, "color", UVEC4, (void*)(&color));

  return obj;
}
object3D_gfx create_sphere(vector3D_gfx* box, layout_table layout, vec3 pos, float radius) {
  object3D_gfx obj;

  vec4 color = (vec4){0.5f, 0.5f, 0.5f, 1.0f};
  int resolution = 10;
  vec3* sphere_verts = malloc((resolution+1) * (resolution+1) * sizeof(vec3));

  // vertex generation
  vec3 vert = (vec3){0.0f, 0.0f, 0.0f};
  float lon_inc = (2.0f * PI) / resolution;
  float lat_inc = PI / resolution;
  const float HALF_PI = PI / 2.0f;
  unsigned int arr_index = 0;
  float lon = -PI;
  float lat = -HALF_PI;
  for (int i = 0; i < resolution + 1; ++i) {
    for (int j = 0; j < resolution + 1; ++j) {
      vert.x = radius * sinf(lon) * cosf(lat);
      vert.y = radius * sinf(lon) * sinf(lat);
      vert.z = radius * cosf(lon);
      sphere_verts[arr_index] = vert;
      ++arr_index;
      lon += lon_inc;
    }
    lat += lat_inc;
  }

  face* sphere_inds = malloc(2 * (resolution) * (resolution+1) * sizeof(face));

  // index generation
  // vertex count spread: 1 | 2*res | ... | 2*res | 1
  unsigned int inds_index = 0;
  for (int i = 0; i < resolution; ++i) {
    for (int j = 0; j < resolution + 1; ++j) {
      unsigned int current_vert_index = j * resolution + i;
      face tri0;
      tri0.inds[0] = current_vert_index;
      tri0.inds[1] = current_vert_index + resolution;
      tri0.inds[2] = current_vert_index + 1;
      sphere_inds[inds_index] = tri0;
      ++inds_index;

      face tri1;
      tri1.inds[0] = current_vert_index;
      tri1.inds[1] = current_vert_index + resolution;
      tri1.inds[2] = current_vert_index + resolution - 1;
      sphere_inds[inds_index] = tri1;
      ++inds_index;
    }
  }

  obj = create_obj(box, (float*)sphere_verts, resolution * resolution * 3, (unsigned int*)(sphere_inds), 2 * resolution * resolution * 3, layout, "./vector3D_gfx/sphere_vert_shader", "./vector3D_gfx/sphere_frag_shader", pos, (vec3){radius, radius, radius});

  set_uniform(obj.buffers.shader, "color", UVEC4, (void*)(&color));

  free(sphere_verts);
  free(sphere_inds);

  return obj;
}
object3D_gfx create_pyramid(vector3D_gfx* box, layout_table layout, vec3 pos, vec3 size) {
  object3D_gfx obj;
  vec4 color = (vec4){0.5f, 0.5f, 0.5f, 1.0f};
  vec3 pyr_verts[] = {
    (vec3){-0.5f, 0.0f, 0.5f},
    (vec3){-0.5f, 0.0f, -0.5f},
    (vec3){0.5f, 0.0f, -0.5f},
    (vec3){0.5f, 0.0f, 0.5f},
    (vec3){0.0f, 0.8f, 0.0f}
  };
  unsigned int pyr_inds[] = {
    0, 1, 2,
    0, 2, 3,
    0, 1, 4,
    1, 2, 4,
    2, 3, 4,
    3, 0, 4
  };

  obj = create_obj(box, (float*)pyr_verts, 5 * 3, pyr_inds, 18, layout, "./vector3D_gfx/pyramid_vert_shader", "./vector3D_gfx/pyramid_frag_shader", pos, size);

  set_uniform(obj.buffers.shader, "color", UVEC4, (void*)(&color));

  return obj;
}

void rotate_obj(object3D_gfx* obj, vec3 axis, float by) {
  obj->rot = (vec3){by * axis.x, by * axis.y, by * axis.z};
  mat4x4_temp rotator;
  rotate_temp(&rotator, axis.x, axis.y, axis.z, by);

  set_uniform_mat4x4(obj->buffers.shader, "rotator", rotator.elems);
}

void destroy_obj(object3D_gfx* obj) {

}

void create_camera3D(vector3D_gfx* box, camera3D* cam, vec3 pos) {
  cam->cam_pos = pos;
  cam->cam_dir = (vec3){0.0f, 0.0f, -1.0f};
  cam->cam_up = (vec3){0.0f, 1.0f, 0.0f};
  cam->cam_right = (vec3){1.0f, 0.0f, 0.0f};
  cam->cam_view = view(NULL, cam->cam_pos, cam->cam_right, cam->cam_up, cam->cam_dir);
  cam->cam_pitch = 0.0f;
  cam->cam_yaw = 0.0f;

  if (box == NULL) {
    return;
  }

  box->cameras = realloc(box->cameras, (box->cam_count + 1) * sizeof(camera3D*));
  box->cameras[box->cam_count] = cam;
  cam->cam_index = box->cam_count;
  ++box->cam_count;
}

void destroy_camera3D(camera3D* cam) {
  delete_mat4x4(&cam->cam_view);
}

void get_camera3D_input(vector3D_gfx* box, camera3D* cam_in) {
  camera3D* cam = cam_in;
  if (cam_in == NULL) {
    cam = box->cameras[box->cam_index];
  }
  if ((glfwGetKey(box->window, GLFW_KEY_ESCAPE) == GLFW_PRESS) && (box->in.cleared)) {
    box->in.locked = !box->in.locked;
    box->in.cleared = 0;
    glfwSetInputMode(box->window, GLFW_CURSOR, (box->in.locked)?(GLFW_CURSOR_HIDDEN):(GLFW_CURSOR_NORMAL));
  }
  if (glfwGetKey(box->window, GLFW_KEY_ESCAPE) == GLFW_RELEASE) {
    box->in.cleared = 1;
  }

  if (box->in.locked) {
    // get mouse
    double mouse_x, mouse_y;
    glfwGetCursorPos(box->window, &mouse_x, &mouse_y);
    double dx = (mouse_x - box->scr_width/2)/box->scr_width;
    double dy = (mouse_y - box->scr_height/2)/box->scr_height;

    mat4x4_temp rotor_x;
    rotate_temp(&rotor_x, 0.0f, 1.0f, 0.0f, dx);

    mat4x4_temp rotor_y;
    rotate_temp(&rotor_y, box->cameras[box->cam_index]->cam_right.x, 0.0f, box->cameras[box->cam_index]->cam_right.z, dy);

    vec4 new_right = (vec4){box->cameras[box->cam_index]->cam_right.x, 0.0f, box->cameras[box->cam_index]->cam_right.z, 0.0f};
    vec4 new_dir = (vec4){box->cameras[box->cam_index]->cam_dir.x, box->cameras[box->cam_index]->cam_dir.y, box->cameras[box->cam_index]->cam_dir.z, 0.0f};

    mul_vec4_tmat4x4(&new_right, rotor_x);

    mul_vec4_tmat4x4(&new_dir, rotor_x);
    mul_vec4_tmat4x4(&new_dir, rotor_y);

    vec4 new_up = cross3_vec4(new_right, new_dir);
    box->cameras[box->cam_index]->cam_dir = (vec3){new_dir.x, new_dir.y, new_dir.z};
    box->cameras[box->cam_index]->cam_up = (vec3){new_up.x, new_up.y, new_up.z};
    box->cameras[box->cam_index]->cam_right = (vec3){new_right.x, new_right.y, new_right.z};

    normalize(&box->cameras[box->cam_index]->cam_dir);
    normalize(&box->cameras[box->cam_index]->cam_up);
    normalize(&box->cameras[box->cam_index]->cam_right);

    glfwSetCursorPos(box->window, box->scr_width/2, box->scr_height/2);

    // get keys
    float speed = 0.15f;
    if (glfwGetKey(box->window, GLFW_KEY_W) == GLFW_PRESS) {
      box->cameras[box->cam_index]->cam_pos.x += box->cameras[box->cam_index]->cam_dir.x * speed;
      box->cameras[box->cam_index]->cam_pos.y += box->cameras[box->cam_index]->cam_dir.y * speed;
      box->cameras[box->cam_index]->cam_pos.z += box->cameras[box->cam_index]->cam_dir.z * speed;
    }
    if (glfwGetKey(box->window, GLFW_KEY_A) == GLFW_PRESS) {
      box->cameras[box->cam_index]->cam_pos.x -= box->cameras[box->cam_index]->cam_right.x * speed;
      box->cameras[box->cam_index]->cam_pos.y -= box->cameras[box->cam_index]->cam_right.y * speed;
      box->cameras[box->cam_index]->cam_pos.z -= box->cameras[box->cam_index]->cam_right.z * speed;
    }
    if (glfwGetKey(box->window, GLFW_KEY_S) == GLFW_PRESS) {
      box->cameras[box->cam_index]->cam_pos.x -= box->cameras[box->cam_index]->cam_dir.x * speed;
      box->cameras[box->cam_index]->cam_pos.y -= box->cameras[box->cam_index]->cam_dir.y * speed;
      box->cameras[box->cam_index]->cam_pos.z -= box->cameras[box->cam_index]->cam_dir.z * speed;
    }
    if (glfwGetKey(box->window, GLFW_KEY_D) == GLFW_PRESS) {
      box->cameras[box->cam_index]->cam_pos.x += box->cameras[box->cam_index]->cam_right.x * speed;
      box->cameras[box->cam_index]->cam_pos.y += box->cameras[box->cam_index]->cam_right.y * speed;
      box->cameras[box->cam_index]->cam_pos.z += box->cameras[box->cam_index]->cam_right.z * speed;
    }
    if (glfwGetKey(box->window, GLFW_KEY_E) == GLFW_PRESS) {
      box->cameras[box->cam_index]->cam_pos.x += box->cameras[box->cam_index]->cam_up.x * speed;
      box->cameras[box->cam_index]->cam_pos.y += box->cameras[box->cam_index]->cam_up.y * speed;
      box->cameras[box->cam_index]->cam_pos.z += box->cameras[box->cam_index]->cam_up.z * speed;
    }
    if (glfwGetKey(box->window, GLFW_KEY_Q) == GLFW_PRESS) {
      box->cameras[box->cam_index]->cam_pos.x -= box->cameras[box->cam_index]->cam_up.x * speed;
      box->cameras[box->cam_index]->cam_pos.y -= box->cameras[box->cam_index]->cam_up.y * speed;
      box->cameras[box->cam_index]->cam_pos.z -= box->cameras[box->cam_index]->cam_up.z * speed;
    }
  }
}

layout_table gfx_layout(int opt) {
  layout_table gfx_table;

  gfx_table.a = (unsigned char)opt;

  gfx_table.a_sizes[0] = (gfx_table.a & 0b00000001) * 3; // POS
  gfx_table.a_sizes[1] = ((gfx_table.a & 0b00000010) >> 1) * 4; // COLOR
  gfx_table.a_sizes[2] = ((gfx_table.a & 0b00000100) >> 2) * 2; // TEX_COORD
  gfx_table.a_sizes[3] = ((gfx_table.a & 0b00001000) >> 3) * 1; // TEX_INDEX
  gfx_table.a_sizes[4] = ((gfx_table.a & 0b00010000) >> 4) * 3; // NORMAL

  gfx_table.vertex_size = 0;
  for (int i = 0; i < 5; ++i) {
    gfx_table.vertex_size += gfx_table.a_sizes[i];
  }

  return gfx_table;
}

int has_normal(object3D_gfx obj) {
  return (obj.attribs.a & 0b00010000) >> 4;
}
int has_color(object3D_gfx obj) {
  return (obj.attribs.a & 0b00000010) >> 1;
}
int has_tex_coord(object3D_gfx obj) {
  return (obj.attribs.a & 0b00000100) >> 2;
}
int has_tex_index(object3D_gfx obj) {
  return (obj.attribs.a & 0b00001000) >> 3;
}

int set_object_property(object3D_gfx* obj, layout_attrib property, float* data, int count) {

  glBindBuffer(GL_ARRAY_BUFFER, obj->buffers.VBO);

  switch (property) {
    case POS:
    {
      for (int i = 0; i < count; ++i) {
        glBufferSubData(GL_ARRAY_BUFFER, i * obj->attribs.vertex_size, 3 * sizeof(float), &(((vec3*)data)[i]));
      }
    }
      break;
    case COLOR:
    {
      int offset = obj->attribs.a_sizes[0];
      for (int i = 0; i < count; ++i) {
        glBufferSubData(GL_ARRAY_BUFFER, i * obj->attribs.vertex_size + offset, obj->attribs.a_sizes[1] * sizeof(float), &(((vec4*)data)[i]));
      }
    }
      break;
    case TEX_COORD:
    {
      int offset = obj->attribs.a_sizes[0] + obj->attribs.a_sizes[1];
      for (int i = 0; i < count; ++i) {
        glBufferSubData(GL_ARRAY_BUFFER, i * obj->attribs.vertex_size + offset, obj->attribs.a_sizes[2] * sizeof(float), &(((vec2*)data)[i]));
      }
    }
      break;
    case TEX_INDEX:
    {
      int offset = obj->attribs.a_sizes[0] + obj->attribs.a_sizes[1] + obj->attribs.a_sizes[2];
      for (int i = 0; i < count; ++i) {
        glBufferSubData(GL_ARRAY_BUFFER, i * obj->attribs.vertex_size + offset, obj->attribs.a_sizes[3] * sizeof(float), &(((float*)data)[i]));
      }
    }
      break;
    case NORMAL:
    {
      int offset = obj->attribs.a_sizes[0] + obj->attribs.a_sizes[1] + obj->attribs.a_sizes[2] + obj->attribs.a_sizes[3];
      for (int i = 0; i < count; ++i) {
        glBufferSubData(GL_ARRAY_BUFFER, i * obj->attribs.vertex_size + offset, obj->attribs.a_sizes[4] * sizeof(float), &(((vec3*)data)[i]));
      }
    }
      break;
    default:
        printf("ERROR: object property not found\n");
      break;
  };

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  return 0;
}

int find_str(const char* line, char* str){
  for (int i = 0; i < strlen(line); ++i) {
    if (line[i] == str[0]) {
      int match_still = 1;
      for (int j = 0; j < strlen(str); ++j) {
        if (!(match_still && (line[i + j] == str[j]))) {
          match_still = 0;
          break;
        }
      }
      if (match_still) {
        return i;
      }
    }
  }
  return -1;
}
