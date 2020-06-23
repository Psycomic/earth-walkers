#include <stdio.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "linear_algebra.h"
#include "misc.h"

extern float camera_final_matrix[16];

void game_init();
void game_loop();
void game_handle_events(GLFWwindow* window);

int main()
{
  game_init();

  float cube_color[] = {
			0.583f,  0.771f,  0.014f,
			0.609f,  0.115f,  0.436f,
			0.327f,  0.483f,  0.844f,
			0.822f,  0.569f,  0.201f,
			0.435f,  0.602f,  0.223f,
			0.310f,  0.747f,  0.185f,
			0.597f,  0.770f,  0.761f,
			0.559f,  0.436f,  0.730f,
			0.359f,  0.583f,  0.152f,
			0.483f,  0.596f,  0.789f,
			0.559f,  0.861f,  0.639f,
			0.195f,  0.548f,  0.859f,
			0.014f,  0.184f,  0.576f,
			0.771f,  0.328f,  0.970f,
			0.406f,  0.615f,  0.116f,
			0.676f,  0.977f,  0.133f,
			0.971f,  0.572f,  0.833f,
			0.140f,  0.616f,  0.489f,
			0.997f,  0.513f,  0.064f,
			0.945f,  0.719f,  0.592f,
			0.543f,  0.021f,  0.978f,
			0.279f,  0.317f,  0.505f,
			0.167f,  0.620f,  0.077f,
			0.347f,  0.857f,  0.137f,
			0.055f,  0.953f,  0.042f,
			0.714f,  0.505f,  0.345f,
			0.783f,  0.290f,  0.734f,
			0.722f,  0.645f,  0.174f,
			0.302f,  0.455f,  0.848f,
			0.225f,  0.587f,  0.040f,
			0.517f,  0.713f,  0.338f,
			0.053f,  0.959f,  0.120f,
			0.393f,  0.621f,  0.362f,
			0.673f,  0.211f,  0.457f,
			0.820f,  0.883f,  0.371f,
			0.982f,  0.099f,  0.879f
  };

  float cube_vertices[] = {
			   -1.0f,-1.0f,-1.0f, // triangle 1 : begin
			   -1.0f,-1.0f, 1.0f,
			   -1.0f, 1.0f, 1.0f, // triangle 1 : end
			   1.0f, 1.0f,-1.0f, // triangle 2 : begin
			   -1.0f,-1.0f,-1.0f,
			   -1.0f, 1.0f,-1.0f, // triangle 2 : end
			   1.0f,-1.0f, 1.0f,
			   -1.0f,-1.0f,-1.0f,
			   1.0f,-1.0f,-1.0f,
			   1.0f, 1.0f,-1.0f,
			   1.0f,-1.0f,-1.0f,
			   -1.0f,-1.0f,-1.0f,
			   -1.0f,-1.0f,-1.0f,
			   -1.0f, 1.0f, 1.0f,
			   -1.0f, 1.0f,-1.0f,
			   1.0f,-1.0f, 1.0f,
			   -1.0f,-1.0f, 1.0f,
			   -1.0f,-1.0f,-1.0f,
			   -1.0f, 1.0f, 1.0f,
			   -1.0f,-1.0f, 1.0f,
			   1.0f,-1.0f, 1.0f,
			   1.0f, 1.0f, 1.0f,
			   1.0f,-1.0f,-1.0f,
			   1.0f, 1.0f,-1.0f,
			   1.0f,-1.0f,-1.0f,
			   1.0f, 1.0f, 1.0f,
			   1.0f,-1.0f, 1.0f,
			   1.0f, 1.0f, 1.0f,
			   1.0f, 1.0f,-1.0f,
			   -1.0f, 1.0f,-1.0f,
			   1.0f, 1.0f, 1.0f,
			   -1.0f, 1.0f,-1.0f,
			   -1.0f, 1.0f, 1.0f,
			   1.0f, 1.0f, 1.0f,
			   -1.0f, 1.0f, 1.0f,
			   1.0f,-1.0f, 1.0f
  };

  uint vertices_number = sizeof(cube_vertices) / sizeof(float);

  /* Initialize GLFW and the opengl context */
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

  GLFWwindow* window;
  window = glfwCreateWindow(800, 800, "Tutorial 01", NULL, NULL);

  if(window == NULL) {
    fprintf(stderr, "Failed to open GLFW window.");
    glfwTerminate();

    return -1;
  }

  glfwMakeContextCurrent(window);

  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    return -1;
  }

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  /* Initialize the triangle and the shaders */

  GLuint VertexArrayID;
  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);

  GLuint vertexbuffer;
  glGenBuffers(1, &vertexbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

  GLuint colorbuffer;
  glGenBuffers(1, &colorbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cube_color), cube_color, GL_STATIC_DRAW);

  GLuint programID = load_shaders("./shaders/vertex_shader_test.glsl", "./shaders/fragment_shader_test.glsl");

  GLuint MatrixID = glGetUniformLocation(programID, "MVP");

  while(!glfwWindowShouldClose(window)) {
    game_loop();

    glUseProgram(programID);
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, camera_final_matrix);

    /* Drawing the triangles */
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(programID);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glDrawArrays(GL_TRIANGLES, 0, vertices_number);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    glfwSwapBuffers(window);
    glfwPollEvents();

    game_handle_events(window);
  }

  glfwTerminate();
  return 0;
}
