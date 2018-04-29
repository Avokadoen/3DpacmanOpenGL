#pragma once 

#define GLM_ENABLE_EXPERIMENTAL

#include "model.hpp"
#include "globals.hpp"
#include "level.hpp"
#include "camera.hpp"
#include "gtx/matrix_decompose.hpp"
#include "score.hpp"

#include <memory>
#include <vector>
#include <string>


/*
	Update's task is to interface to controller and to 
	deal with time for any given command like "move",
	"rotate", etc.
*/


// deal with change in any model/mesh transform and update render data

// update position, rotation and scale based on deltatime



class Game {
public:
	Game();											
	void update(float deltaTime, glob::ButtonPressed b);					
	void draw();									
	void initPacman(std::string levelPath);								

	glob::GameState gameState; 

private:

	void moveModel(Render& r, float deltaTime);
	//void rotateRenderObject(Render& r, float degree, glm::vec3 up);
	//void scaleRenderObject(Render& r, glm::vec3 scale);
	//void translateRenderObject(Render& r, glm::vec3 position);
	void collided(Render& render);

	float characterScale;
	float ghostThinkTimer;
	void ghostAI();

	std::shared_ptr<Camera> camera;

	std::unique_ptr<Level> level;
	std::unique_ptr<Model> player;					
	//std::shared_ptr <std::vector<Model>> modelptr;	// TODO
	std::vector<Render> staticRender;
	std::vector<Render> dynamicRender;
	Score scoreRender;
	// player state
	int playerScore;
	float pacmanVel;

};

/*
------	TODO	------
Game features needed:
	- Read the provided map layout and render it on the screen. [1] [V]
		- Walls must have textures									[V]

	- Present a menu that allows starting and exiting the game. 
		- Navigate with arrow keys.
		- "ESC" pressed while in-game should bring up a pause menu that allows
		the player to exit or resume the game.

	- Have a Pacman player character that is controlled with WASD	[2] [V]
		- Pacman can not run through walls								[V]

	- Maintain a player score.											[V]
		- Score visible during gameplay, and shown on mainscreen upon success/death.[X]

	- Include collectible items that add to the player's score.			[V]
		- Once collected, the items should dissappear from the map.		[V]

	- Have moving ghosts. Touching these leads to loss of Pacman's life.
		- Note: Similar to the collected items, ghost can be represented by symbolic shapes.
		- Ghosts movement does not need to be strategic, but can be purely random.	[V]

	- At least one character type (Pacman, ghosts, collectibles)	[V]
		- Needs to be loaded from a model (e.g. .obj file).			[V]
		- The model has to be textured.								[V]

	- Basic lighting model.

	- You can choose to implement the game from a first-person perspective.
	In that case you will need to display a separate 2D projection of the
	grid for navigation purposes. (Map?) Alternatively, you can implement a
	third-person perspective, in which case you do not require the 2D grid
	representation.


Optional Game Features:

	- Advanced lighting/reflection models

	- Switching between perspectives (e.g. first-person, third-person, 2D)
	
	- Use of 3D models for multiple character types/walls, or use of advanced models.

	- Write your own object loader.

	- Make camera moveable		[V]

	- Introduce multiple levels	[50%]

	- Any other feature that you think adds an edge to your game (e.g. increses the
	fun factor, or usability, or aesthetics; see Marking Consideration section below)
*/