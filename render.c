#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "linear_algebra.h"

int read_file(char* buffer, const char* filename) {
  FILE* file = fopen(filename, "r");

  if (file == NULL){
    fprintf(stderr, "Could not open file !\n");
    return -1;
  }
  else {
    printf("Successfully opened file %s!\n", filename);
  }

  while ((*buffer = fgetc(file)) != EOF) {
    buffer++;
  }

  *buffer = '\0';

  fclose(file);
  return 0;
}

GLuint load_shaders(const char* vertex_file_path, const char* fragment_file_path){

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	char VertexShaderCode[2048];
	read_file(VertexShaderCode, vertex_file_path);

	// Read the Fragment Shader code from the file
	char FragmentShaderCode[2048];
	read_file(FragmentShaderCode, fragment_file_path);

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const* VertexSourcePointer = VertexShaderCode;

	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);

	if (InfoLogLength > 0){
	  char* VertexShaderErrorMessage = malloc(sizeof(char) * (InfoLogLength+1));
	  glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, VertexShaderErrorMessage);
	  printf("%s\n", VertexShaderErrorMessage);

	  free(VertexShaderErrorMessage);
	}

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode;
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);

	if (InfoLogLength > 0){
	  char* FragmentShaderErrorMessage = malloc(sizeof(char) * (InfoLogLength+1));
	  glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, FragmentShaderErrorMessage);
	  printf("%s\n", FragmentShaderErrorMessage);

	  free(FragmentShaderErrorMessage);
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);

	if (InfoLogLength > 0){
	  char* ProgramErrorMessage = malloc(sizeof(char) * (InfoLogLength+1));
	  glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
	  printf("%s\n", ProgramErrorMessage);

	  free(ProgramErrorMessage);
	}

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

int initialize_glfw() {
  glewExperimental = 1;

  if(!glfwInit()){
    fprintf(stderr, "GLFW not initialized correctly !\n");
    return -1;
  }

  glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL

  return 0;
}

void camera_create_rotation_matrix(Mat4 destination, float rx, float ry) {
  float rotation_matrix_x[16];
  float rotation_matrix_y[16];

  mat4_create_rotation_x(rotation_matrix_x, rx);
  mat4_create_rotation_y(rotation_matrix_y, ry);

  mat4_mat4_mul(destination, rotation_matrix_y, rotation_matrix_x); /* First, Y rotation, after X rotation */
}

void camera_create_final_matrix(Mat4 destination, Mat4 perspective, Mat4 rotation, Vector3 position) {
  float translation_matrix[16];
  float temporary_matrix[16];

  mat4_create_translation(translation_matrix, position);

  mat4_mat4_mul(temporary_matrix, translation_matrix, rotation);
  mat4_mat4_mul(destination, temporary_matrix, perspective);
}
