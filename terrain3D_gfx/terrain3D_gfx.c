#include "terrain3D_gfx.h"
#include <stdio.h>

void create_perlin_dbg_tex(perlin_dbg_tex* dbg, unsigned int* tex_counter) {
  float verts[] = {
    -1.0f, -1.0f, 0.0f, 0.0f,
    -0.5f, -1.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, 1.0f, 1.0f,
    -1.0f, -0.5f, 0.0f, 1.0f
  };
  unsigned int inds1[] = {
    0, 1, 2, 2, 3, 0
  };

  glGenBuffers(1, &dbg->VBO);
  glGenVertexArrays(1, &dbg->VAO);

  glBindVertexArray(dbg->VAO);
  glBindBuffer(GL_ARRAY_BUFFER, dbg->VBO);
  glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), verts, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(0 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  glGenBuffers(1, &dbg->IBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dbg->IBO);
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
                         "uniform sampler2D dbg_tex;\n"
                         "void main(){\n"
                         "aColor = texture(dbg_tex, oTex);\n"
                         "};\n\0";

    dbg->shader = create_shader(main_vertex, main_fragment);

    int temp = *(tex_counter) - 1;
    set_uniform(dbg->shader, "dbg_tex", UI, (void*)(&temp));
  }

}
void draw_perlin_dbg_tex(perlin_dbg_tex dbg) {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glBindVertexArray(dbg.VAO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dbg.IBO);
  glUseProgram(dbg.shader);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glUseProgram(0);
}

void generate_kth_octave(float* out, float* s, int k, int noise_width, int noise_height) {
  int samplePeriod = 1 << k;
  float sampleFrequency = 1.0f / (float)(samplePeriod);

  int width = noise_width;
  int height = noise_height;

  for (int i = 0; i < noise_width; ++i) {
    //calculate the horizontal sampling indices
    int sample_i0 = (i / samplePeriod) * samplePeriod;
    int sample_i1 = (sample_i0 + samplePeriod) % width; //wrap around
    float horizontal_blend = (i - sample_i0) * sampleFrequency;

    for (int j = 0; j < noise_height; ++j) {
      int sample_j0 = (j / samplePeriod) * samplePeriod;
      int sample_j1 = (sample_j0 + samplePeriod) % height; //wrap around
      float vertical_blend = (j - sample_j0) * sampleFrequency;
      
      float top = linear_interpolate(s[sample_i0 * height + sample_j0], s[sample_i1 * height + sample_j0], horizontal_blend);

      float bottom = linear_interpolate(s[sample_i0 * height + sample_j1], s[sample_i1 * height + sample_j1], horizontal_blend);
      
      float total = linear_interpolate(top, bottom, vertical_blend);

      out[i * noise_height + j] = total;//s[i * noise_height + j];
    }
  }
}

terrain_noise generate_noise(int noise_width, int noise_height, int octave_count, float bias) {
  srand(time(0));

  float* noise = malloc(noise_width * noise_height * sizeof(float));
  for (int i = 0; i < noise_height; ++i) {
    for (int j = 0; j < noise_width; ++j) {
      noise[i * noise_width + j] = (float)(rand() % 99);
      noise[i * noise_width + j] /= 100.0f;
    }
  }

  float* coherent_noise = malloc(noise_width * noise_height * sizeof(float));
  memset(coherent_noise, 0.0f, noise_width * noise_height * sizeof(float));

  float** octaves = malloc(octave_count * sizeof(float*));
 
  // generate octaves
  float amp = 1.0f;
  float luc = 1.0f;
  float amp_total = 0.0f;
  for (int oct_i = 0; oct_i < octave_count; ++oct_i) {
    octaves[oct_i] = malloc(noise_width * noise_height * sizeof(float));
    memset(octaves[oct_i], 0.0f, noise_width * noise_height * sizeof(float));
    generate_kth_octave(octaves[oct_i], noise, oct_i, noise_width, noise_height);
  }

  // blend octaves
  for (int octave_index = 0; octave_index < octave_count; ++octave_index) {
    for (int i = 0; i < noise_height; ++i) {
      for (int j = 0; j < noise_width; ++j) {
        coherent_noise[i * noise_width + j] += octaves[octave_index][i * noise_width + j] * amp;
      }
    }
    amp_total += amp;
    amp /= bias;

    free(octaves[octave_index]);
  }

  // normalize
  for (int i = 0; i < noise_height; ++i) {
    for (int j = 0; j < noise_width; ++j) {
      coherent_noise[i * noise_width + j] = coherent_noise[i * noise_width + j] / (amp_total);
    }
  }

  free(noise);
  free(octaves);
  return (terrain_noise){coherent_noise, noise_width, noise_height};
}

terrain_mesh create_terrain_plane(vector3D_gfx* box, float quad_area, unsigned int pt_width, unsigned int pt_height, float max_height, int octave_count, float persistence, float elevation, perlin_dbg_tex* dbg) {
  float side_len = sqrtf(quad_area);
  unsigned int quad_width = pt_width - 1;
  unsigned int quad_height = pt_height - 1;
  terrain_vertex* verts = malloc(quad_width * quad_height * 6 * sizeof(terrain_vertex));
  
  terrain_noise noise = generate_noise(pt_width, pt_height, octave_count, persistence);

  if (dbg != NULL) {
    unsigned int tex = create_empty_tex(noise.noise_width, noise.noise_height, &box->tex_counter);
    unsigned char* pix = malloc(noise.noise_width * noise.noise_height * 3 * sizeof(unsigned char));
    int g = 0;
    for (int i = 0; i < noise.noise_width * noise.noise_height * 3; i += 3) {
      pix[i] = (unsigned char)((float)noise.noise_data[g] * 255.0f);
      pix[i+1] = (unsigned char)((float)noise.noise_data[g] * 255.0f);
      pix[i+2] = (unsigned char)((float)noise.noise_data[g] * 255.0f);
      g++;
    }
    populate_tex(tex, 0, 0, noise.noise_width, noise.noise_height, pix);
    glActiveTexture(GL_TEXTURE0 + (box->tex_counter - 1));
    glBindTexture(GL_TEXTURE_2D, tex);
    glGenerateMipmap(GL_TEXTURE_2D);
    *dbg = (perlin_dbg_tex){pix, noise.noise_width, noise.noise_height, tex, 0, 0, 0, 0};
    create_perlin_dbg_tex(dbg, &box->tex_counter);
  }


  // generate verticies
  vec2 offset = (vec2){0.0f, 0.0f};
  for (int y = 0; y < quad_height; ++y) {
    for (int x = 0; x < quad_width * 6; x += 6) {
      int index = y * (quad_width * 6) + x;

      // tri 1
      verts[index].a = (vec3){offset.x, 0.0f, offset.y};
      verts[index + 1].a = (vec3){offset.x, 0.0f, offset.y + side_len};
      verts[index + 2].a = (vec3){offset.x + side_len, 0.0f, offset.y + side_len};

      // tri 2
      verts[index + 3].a = (vec3){offset.x, 0.0f, offset.y};
      verts[index + 4].a = (vec3){offset.x + side_len, 0.0f, offset.y};
      verts[index + 5].a = (vec3){offset.x + side_len, 0.0f, offset.y + side_len};

      offset.x += side_len;
    }
    offset.x = 0.0f;
    offset.y += side_len;
  }

  // generate heights
  for (int i = 0; i < quad_height * quad_width * 6; ++i) {
    int index = (verts[i].a.z / side_len) * (noise.noise_width) + (verts[i].a.x / side_len);
    verts[i].a.y = pow((float)(noise.noise_data[index] * max_height), elevation);
  }

  // generate normals
  for (int y = 0; y < quad_height; ++y) {
    for (int x = 0; x < quad_width * 6; x += 6) {
      int index = y * (quad_width * 6) + x;

      // tri 1
      vec3 norm1 = cross(sub_vec3(verts[index+1].a, verts[index].a), sub_vec3(verts[index+2].a, verts[index].a));
      normalize(&norm1);
      verts[index].b = norm1;
      verts[index + 1].b = norm1;
      verts[index + 2].b = norm1;

      // tri 2
      vec3 norm2 = cross(sub_vec3(verts[index+4].a, verts[index+5].a), sub_vec3(verts[index+3].a, verts[index+5].a));
      normalize(&norm2);
      verts[index + 3].b = norm2;
      verts[index + 4].b = norm2;
      verts[index + 5].b = norm2;
    }
  }

  // generate colors
  for (int y = 0; y < quad_height; ++y) {
    for (int x = 0; x < quad_width * 6; x++) {
      int index = y * (quad_width * 6) + x;

      if (verts[index].a.y > 45.0f) {
        verts[index].c = (vec4){1.0f, 1.0f, 1.0f, 1.0f};
      }
      else if (verts[index].a.y < 26.0f) {
        verts[index].c = (vec4){0.7f, 0.8f, 0.0f, 1.0f};
      }
      else {
        verts[index].c = (vec4){0.0f, 1.0f, 0.0f, 1.0f};
      }
    }
  }

  object3D_gfx plane = create_obj_array(box, (float*)verts, quad_width * quad_height * 6 * 10, gfx_layout(POS | NORMAL | COLOR), "./terrain3D_gfx/terrain_vert_shader", "./terrain3D_gfx/terrain_frag_shader", (vec3){0.0f, 0.0f, 0.0f}, (vec3){1.0f, 1.0f, 1.0f});

  vec4 col = (vec4){1.0f, 1.0f, 1.0f, 1.0f};
  set_uniform(plane.buffers.shader, "color", UVEC4, (void*)(&col));

  free(noise.noise_data);
  return (terrain_mesh){verts, noise.noise_width, noise.noise_height, plane};
}

terrain_mesh create_flat_plane(vector3D_gfx* box, float quad_area, unsigned int pt_width, unsigned int pt_height, float elevation, vec4 color) {
  float side_len = sqrtf(quad_area);
  unsigned int quad_width = pt_width - 1;
  unsigned int quad_height = pt_height - 1;
  terrain_vertex* verts = malloc(quad_width * quad_height * 6 * sizeof(terrain_vertex));
  for (int i = 0; i < quad_width * quad_height * 6; ++i) {
    verts[i] = (terrain_vertex){(vec3){0.0f, 0.0f, 0.0f}, (vec4){1.0f, 1.0f, 1.0f, 1.0f}, (vec3){0.0f, 0.0f, 0.0f}};
  }
  
  // generate verticies
  vec2 offset = (vec2){0.0f, 0.0f};
  for (int y = 0; y < quad_height; ++y) {
    for (int x = 0; x < quad_width * 6; x += 6) {
      int index = y * (quad_width * 6) + x;

      // tri 1
      verts[index].a = (vec3){offset.x, elevation, offset.y};
      verts[index + 1].a = (vec3){offset.x, elevation, offset.y + side_len};
      verts[index + 2].a = (vec3){offset.x + side_len, elevation, offset.y + side_len};

      // tri 2
      verts[index + 3].a = (vec3){offset.x, elevation, offset.y};
      verts[index + 4].a = (vec3){offset.x + side_len, elevation, offset.y};
      verts[index + 5].a = (vec3){offset.x + side_len, elevation, offset.y + side_len};

      offset.x += side_len;
    }
    offset.x = 0.0f;
    offset.y += side_len;
  }

  // generate normals
  for (int y = 0; y < quad_height; ++y) {
    for (int x = 0; x < quad_width * 6; x += 6) {
      int index = y * (quad_width * 6) + x;

      // tri 1
      vec3 norm1 = cross(sub_vec3(verts[index+1].a, verts[index].a), sub_vec3(verts[index+2].a, verts[index].a));
      normalize(&norm1);
      verts[index].b = norm1;
      verts[index + 1].b = norm1;
      verts[index + 2].b = norm1;

      // tri 2
      vec3 norm2 = cross(sub_vec3(verts[index+4].a, verts[index+5].a), sub_vec3(verts[index+3].a, verts[index+5].a));
      normalize(&norm2);
      verts[index + 3].b = norm2;
      verts[index + 4].b = norm2;
      verts[index + 5].b = norm2;
    }
  }

  object3D_gfx plane = create_obj_array(box, (float*)verts, quad_width * quad_height * 6 * 10, gfx_layout(POS | NORMAL | COLOR), "./terrain3D_gfx/terrain_vert_shader", "./terrain3D_gfx/terrain_frag_shader", (vec3){0.0f, 0.0f, 0.0f}, (vec3){1.0f, 1.0f, 1.0f});

  vec4 col = color;
  set_uniform(plane.buffers.shader, "color", UVEC4, (void*)(&col));

  return (terrain_mesh){verts, pt_width, pt_height, plane};
}
