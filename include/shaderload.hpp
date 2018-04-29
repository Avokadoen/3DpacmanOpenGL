#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <GL/glew.h>
#include "GLFW/glfw3.h"

#include "glm.hpp"
#include <vector>
#include <fstream>

void read_shader_src(const char *fname, std::vector<char> &buffer);
GLuint load_and_compile_shader(const char *fname, GLenum shaderType);
GLuint create_program(const char *path_vert_shader, const char *path_frag_shader);
