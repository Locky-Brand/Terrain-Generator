#include "textures.h"

Texture load_texture(const char* src, unsigned int* texture_counter, int unpack_alignment) {
  Texture tex;

  int width, height, channels;

  // generate new texture object
  glGenTextures(1, &tex.ID);
  glActiveTexture(GL_TEXTURE0 + (*texture_counter));
  tex.index = (*texture_counter);
  (*texture_counter)++;
  //glBindTextureUnit(mTextureCount, mTexID);
  glBindTexture(GL_TEXTURE_2D, tex.ID);
  // set the texture wrapping parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP); // set texture wrapping to GL_REPEAT (default wrapping method)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  // set texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  stbi_set_flip_vertically_on_load(true);
  unsigned char* data = stbi_load(src, &width, &height, &channels, 0);
  if(data){
     // load image, create texture and generate mipmaps
    GLenum format;
    glPixelStorei(GL_UNPACK_ALIGNMENT, unpack_alignment);
    switch(channels){
      case 3:
        format = GL_RGB;
      break;
      case 4:
        format = GL_RGBA;
      break;
      default:
        printf("ERROR: unknown image format\n");
        format = GL_RGB;
      break;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  else {
    printf("ERROR: failed to load texture\n");
  }
  stbi_image_free(data);

  tex.sizes = (vec2){width, height};
  tex.sub_divisions = (vec2){width, height};

  return tex;
}

void set_texture_sub_division(Texture* tex, vec2 sub_sizes){
  tex->sub_divisions = sub_sizes;
}

Texture load_sub_texture(Texture tex, vec2 sheet_index) {
  //vec2 sub_coord = (vec2){tex.sub_divisions.x * sheet_index.x, tex.sub_divisions.y * sheet_index.y};
  return (Texture){tex.index, tex.ID, tex.sizes, tex.sub_divisions};
}
