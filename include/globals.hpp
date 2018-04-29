#pragma once

#include "glm.hpp"

#include <string>

namespace glob {
	const int WINDOW_X				= 1920;
	const int WINDOW_Y				= 1080;
	const int MAX_OBJECT_PR_RENDER	= 400;
	const int MAX_LEVEL_SIZE		= 100;
	const int SCORE_POINT = 10;

	const float LEVEL_SCALE = 10.0f;

	const float MAX_RENDER_DISTANCE = 1000.0f;
	const float MIN_RENDER_DISTANCE = 1.0f;
	const float FOV					= glm::radians(70.0f);
	const float LEVEL_HEIGHT		= 0.2f;
	const float LEVEL_BOTTOM		= 0.0f;

	const std::string WINDOW_NAME	= "pacman the retaliation";



	// common structs:

	struct Texture {
		unsigned int id;
		std::string type;
		std::string path;  // we store the path of the texture to compare with other textures
	};

	struct Vertex {
		// position
		glm::vec3 Position;
		// normal
		glm::vec3 Normal;
		// texCoords
		glm::vec2 TexCoords;

	};

	struct ButtonPressed {

		bool wKey;
		bool aKey;
		bool sKey;
		bool dKey;

		bool spaceKey;
		bool lCtrlKey;

		// arrows
		bool up;
		bool down;
		bool left;
		bool right;
	};


	// Game states 

	enum Direction {
		UP = 0, RIGHT = 90, DOWN = 180, LEFT = 270
	};

	enum GameState {
		PLAYING, PAUSED, MENU, DEAD, WON
	};
}




