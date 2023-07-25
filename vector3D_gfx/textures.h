#ifndef TEXTURES_H
#define TEXTURES_H

#include <stdio.h>
#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>

#include "api_tools.h"

typedef struct {
  unsigned int index;
  unsigned int ID;
  vec2 sizes;
  vec2 sub_divisions;
} Texture;

Texture load_texture(const char* src, unsigned int* texture_counter, int unpack_alignment);
void set_texture_sub_division(Texture* tex, vec2 sub_sizes);
Texture load_sub_texture(Texture tex, vec2 sheet_index);

#endif
