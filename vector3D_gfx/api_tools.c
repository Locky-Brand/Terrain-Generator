#include "api_tools.h"
#include <stdio.h>

unsigned int create_shader(const char* vertShaderSrc, const char* fragShaderSrc){

    GLuint vertShad = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShad, 1, (const char* const*)&vertShaderSrc, NULL);
    glCompileShader(vertShad);

    {
        int success;
        glGetShaderiv(vertShad, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            char log[512];
            GLsizei length;
            glGetShaderInfoLog(vertShad, 512, &length, log);
            printf("%s\n", log);
        }
    }

    GLuint fragShad = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShad, 1, (const char* const*)&fragShaderSrc, NULL);
    glCompileShader(fragShad);

    {
        int success;
        glGetShaderiv(fragShad, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            char log[512];
            GLsizei length;
            glGetShaderInfoLog(fragShad, 512, &length, log);
            printf("%s\n", log);
        }
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertShad);
    glAttachShader(program, fragShad);
    glLinkProgram(program);
    {
        int success;
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success)
        {
            char log[512];
            GLsizei length = 0;
            glGetProgramInfoLog(program, 512, &length, log);
            printf("%s", log);
        }
    }
    return program;
}
unsigned int create_shader_file(const char* vert_file_path, const char* frag_file_path){
    unsigned int vert_file_size, frag_file_size;
    FILE* vfptr; 
    FILE* ffptr;
    
    vfptr = fopen(vert_file_path, "r");
    if(vfptr == NULL){
        printf("%s\n%s\n", "ERROR: vertex shader not found: file path: ", vert_file_path);
        return 0;
    }

    ffptr = fopen(frag_file_path, "r");
    if(ffptr == NULL){
        printf("%s\n%s\n", "ERROR: fragment shader not found: file path: ", frag_file_path);
        return 0;
    }

    fseek(vfptr, 0L, SEEK_END);
    vert_file_size = ftell(vfptr);
    
    fseek(ffptr, 0L, SEEK_END);
    frag_file_size = ftell(ffptr);
    
    char vertS[vert_file_size];
    vertS[vert_file_size] = '\0';

    char fragS[frag_file_size];
    fragS[frag_file_size] = '\0';

    fseek(vfptr, 0L, SEEK_SET);
    fseek(ffptr, 0L, SEEK_SET);

    fread(vertS, 1, vert_file_size, vfptr);
    fread(fragS, 1, frag_file_size, ffptr);

    const char* vertShaderSrc = vertS;
    const char* fragShaderSrc = fragS;

    GLuint vertShad = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShad, 1, (const char* const*)&vertShaderSrc, NULL);
    glCompileShader(vertShad);

    {
        int success;
        glGetShaderiv(vertShad, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            char log[512];
            GLsizei length;
            glGetShaderInfoLog(vertShad, 512, &length, log);
            printf("%s\n", log);
        }
    }

    GLuint fragShad = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShad, 1, (const char* const*)&fragShaderSrc, NULL);
    glCompileShader(fragShad);

    {
        int success;
        glGetShaderiv(fragShad, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            char log[512];
            GLsizei length;
            glGetShaderInfoLog(fragShad, 512, &length, log);
            printf("%s\n", log);
        }
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertShad);
    glAttachShader(program, fragShad);
    glLinkProgram(program);
    {
        int success;
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success)
        {
            char log[512];
            GLsizei length = 0;
            glGetProgramInfoLog(program, 512, &length, log);
            printf("%s", log);
        }
    }

    fclose(vfptr);
    fclose(ffptr);

    return program;
}
bool set_uniform(unsigned int shader, const char* name, pixel_uniform_data_type type, void* data){
    glUseProgram(shader);
    int uniform_location = glGetUniformLocation(shader, name);
    switch(type){
        case UFLOAT: glUniform1f(uniform_location, *(float*)(data)); break;
        case UVEC2: glUniform2f(uniform_location, ((float*)data)[0], ((float*)data)[1]); break;
        case UVEC3: glUniform3f(uniform_location, ((float*)data)[0], ((float*)data)[1], ((float*)data)[2]); break;
        case UVEC4: glUniform4f(uniform_location, ((float*)data)[0], ((float*)data)[1], ((float*)data)[2], ((float*)data)[3]); break;
        case UI: glUniform1i(uniform_location, *(int*)(data)); break;
        case UUI: glUniform1i(uniform_location, *(unsigned int*)(data)); break;
        default:
            printf("error: unknown uniform type\n");
        break;
    }
    glUseProgram(0);
    return (uniform_location != -1);
}
bool set_uniform_mat4x4(unsigned int shader, const char* name, float* data){
  glUseProgram(shader);
  int uniform_location = glGetUniformLocation(shader, name);
  glUniformMatrix4fv(uniform_location, 1, GL_TRUE, data);
  glUseProgram(0);
  return (uniform_location != -1);
}
bool set_uniform_array(unsigned int shader, const char* name, pixel_uniform_data_type type, unsigned int count, void* data){
    glUseProgram(shader);
    int uniform_location = glGetUniformLocation(shader, name);
    switch(type){
        case UFLOAT: glUniform1fv(uniform_location, count, (float*)(data)); break;
        case UI: glUniform1iv(uniform_location, count, (int*)(data)); break;
        case UUI: glUniform1iv(uniform_location, count, (int*)(data)); break;
        default:
            printf("error: unknown uniform type\n");
        break;
    }
    glUseProgram(0);
    
    return (uniform_location != -1);
}

file_data load_file(const char* path){
    file_data f;
    unsigned int file_size;
    FILE* vfptr; 
    
    vfptr = fopen(path, "r");
    if(vfptr == NULL){
      printf("%s\n%s\n", "ERROR: vertex shader not found: file path: ", path);
      return (file_data){0, NULL};
    }

    fseek(vfptr, 0L, SEEK_END);
    file_size = ftell(vfptr);
    
    char src[file_size];
    src[file_size] = '\0';

    fseek(vfptr, 0L, SEEK_SET);

    fread(src, 1, file_size, vfptr);

    fclose(vfptr);

    f.data = malloc(file_size * sizeof(char));
    for (int i = 0; i < file_size; ++i) {
        f.data[i] = src[i];
    }
    f.size = file_size;

    return f;
}
void load_file_part(char* content, const char* after_line){

}
unsigned int get_file_size(const char* path){
    unsigned int file_size;
    FILE* vfptr; 
    
    vfptr = fopen(path, "r");
    if(vfptr == NULL){
        printf("%s\n%s\n", "ERROR: vertex shader not found: file path: ", path);
        return 0;
    }

    fseek(vfptr, 0L, SEEK_END);
    file_size = ftell(vfptr);

    fclose(vfptr);

    return file_size;
}
void close_file(file_data file){
    free(file.data);
}

unsigned int create_empty_tex(unsigned int width, unsigned int height, unsigned int* tex_counter) {
  unsigned int tex;

  glGenTextures(1, &tex);
  glActiveTexture(GL_TEXTURE0 + *(tex_counter));
  glBindTexture(GL_TEXTURE_2D, tex);
  // set the texture wrapping parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // set texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

  (*tex_counter)++;
  
  return tex;
}
void populate_tex(unsigned int tex, unsigned int x_off, unsigned int y_off, unsigned int width, unsigned int height, unsigned char* data) {
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexSubImage2D(GL_TEXTURE_2D, 0, x_off, y_off, width, height, GL_RGB, GL_UNSIGNED_BYTE, (void*)data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

FBO create_FBO(unsigned int width, unsigned int height, unsigned int* tex_counter) {
  FBO frame = (FBO){width, height, 0, 0};

  frame.texID = create_empty_tex(width, height, tex_counter);
  
  // generate renderer frame buffer
	glGenFramebuffers(1, &frame.ID);
	glBindFramebuffer(GL_FRAMEBUFFER, frame.ID);
	glBindTexture(GL_TEXTURE_2D, frame.texID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frame.texID, 0);
  
  // create render buffer
  unsigned int rbo;
  glGenRenderbuffers(1, &rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
 
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	int error = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(error != GL_FRAMEBUFFER_COMPLETE){
		printf("ERROR (%d): frame buffer creation failed\n", error);
    return (FBO){width, height, 0, 0};
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

  return frame;
}
