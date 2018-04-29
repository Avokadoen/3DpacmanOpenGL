#pragma once

#include "ft2build.h"
#include FT_FREETYPE_H
#include <map>
#include "globals.hpp"
#include "glm.hpp"
#include <iostream>
#include "GL/glew.h"
#include "GLFW/glfw3.h"

//#include "shaderload.h"


// copy paste from: https://learnopengl.com/In-Practice/Text-Rendering

struct Character {
	GLuint     TextureID;  // ID handle of the  glyph texture
	glm::ivec2 Size;       // Size of glyph
	glm::ivec2 Bearing;    // Offset from baseline to left/top of glyph
	GLuint     Advance;    // Offset to advance to next glyph
};

class TextController {
private:
	std::map<GLchar, Character> Characters;
	GLuint VAO, VBO;
	GLuint shader;
	
public:
	void ftInit(GLfloat wWidth, GLfloat wHeight);
	void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
};