#include "game.hpp"

#include <iostream>

/*
	Game class is a class that "hacks" our backend code to create pacman
	as the backend code is incomplete. It also contains most of the game logic
*/

Game::Game() {
	camera = std::make_shared<Camera>();
	gameState = glob::MENU;
}

void Game::update(float deltaTime, glob::ButtonPressed b) {
	if (gameState != glob::PLAYING)
		return;

	if (deltaTime > 0.02) {
		deltaTime = 0.02;
	}

	ghostThinkTimer += deltaTime;

	glm::vec3 newPos = camera->position;

	// record previous postion of pacman
	glm::vec3 previousPos;
	glm::decompose(dynamicRender[0].transform, glm::vec3(), glm::quat(), previousPos, glm::vec3(), glm::vec4());

	if (ghostThinkTimer >= 0.15) {
		ghostAI();
		ghostThinkTimer = 0;
	}
	
	for (auto&& render : dynamicRender) {
		render.prevDirection = render.direction;
	}

	if (b.wKey) {
		dynamicRender[0].desiredDirection = glob::UP;
		dynamicRender[0].crashed = false;
	}
	else if (b.aKey) {
		dynamicRender[0].desiredDirection = glob::LEFT;
		dynamicRender[0].crashed = false;
	}
	else if (b.sKey) {
		dynamicRender[0].desiredDirection = glob::DOWN;
		dynamicRender[0].crashed = false;
	}
	else if (b.dKey) {
		dynamicRender[0].desiredDirection = glob::RIGHT;
		dynamicRender[0].crashed = false;
	}
	if (b.spaceKey) {
		newPos[2] += camera->speed * deltaTime;
	}
	if (b.lCtrlKey) {
		newPos[2] -= camera->speed * deltaTime;
	}
	if (b.up) {
		newPos[1] += camera->speed * deltaTime;
	}
	if (b.down) {
		newPos[1] -= camera->speed * deltaTime;
	}
	if (b.left) {
		newPos[0] -= camera->speed * deltaTime;
	}
	if (b.right) {
		newPos[0] += camera->speed * deltaTime;
	}

	
	// move all dynamics (ghosts and pacman)
	for (auto&& render : dynamicRender) {

		if(!render.crashed){
			bool newPath = render.direction != render.desiredDirection;
			render.direction = render.desiredDirection;

			if (newPath) {
				moveModel(render, deltaTime * 4);
			}
			else{
				moveModel(render, deltaTime);
			}
			

			// loop all of render hitboxes
			//for (auto hitbox : render.hitboxes) { *removed for more frames

			// if render object collide with wall
			if (staticRender[0].checkCollision(render.hitboxes[2], render.transform)) {
				// fix position
				collided(render);
				render.crashed = true;

				if (newPath) {
					moveModel(render, deltaTime);
					render.crashed = false;			
					if (staticRender[0].checkCollision(render.hitboxes[2], render.transform)) {
						collided(render);
						render.crashed = true;			
					}
				}
			}		
		}
	}

	for (auto ghost = dynamicRender.begin() + 1; ghost != dynamicRender.end(); ghost++) {
		for (auto&& pacmanHitbox : dynamicRender[0].hitboxes) {

			if (ghost->checkCollision(pacmanHitbox, dynamicRender[0].transform)) {
				gameState = glob::DEAD;
			}
		}	
	}

	int index = scoreRender.checkScoreCollision(dynamicRender[0].hitboxes[2], dynamicRender[0].transform);
	if (index > -1) {
		playerScore += glob::SCORE_POINT;
		scoreRender.deleteIndices(index);

		if (playerScore >= level->winningScore) {
			gameState = glob::WON;
		}
	}

	// Teleport check
	for (auto&& render : dynamicRender) {
		int index = staticRender[1].checkTeleportCollision(render.hitboxes[2], render.transform);
		if (index > -1) {

			int newPosIndex = index;
			while (newPosIndex == index) {
				newPosIndex = rand() % level->teleportPos.size();
			}
			render.transform = glm::translate(glm::mat4(1.0f), level->teleportPos[newPosIndex]);
			render.transform = glm::rotate(render.transform, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			render.transform = glm::rotate(render.transform, glm::radians((float)level->teleportDir[newPosIndex]), glm::vec3(0.0f, 1.0f, 0.0f));
			render.transform = glm::scale(render.transform, glm::vec3(glob::LEVEL_SCALE * characterScale, glob::LEVEL_SCALE * characterScale, glob::LEVEL_SCALE * characterScale));
			//render.transform = glm::translate(render.transform, -previousPos);
			//render.transform = glm::translate(render.transform, level->teleportPos[newPosIndex]);
			render.prevDirection = render.direction = render.desiredDirection = level->teleportDir[newPosIndex];
		}
	}
	
	// retrieve new pacman position
	glm::vec3 target;
	glm::decompose(dynamicRender[0].transform, glm::vec3(), glm::quat() , target, glm::vec3(), glm::vec4());
	
	// make camera look at pacman and move camera with pacman
	camera->newView(newPos+((target-previousPos)), target);
	

}

void Game::draw() {
	if (gameState == glob::MENU)
		return;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for (auto&& render : staticRender) {
		render.draw();
	}

	scoreRender.draw();

	for (auto&& render : dynamicRender) {
		render.draw();
	}

}
void Game::initPacman(std::string levelPath) {

	staticRender.clear();
	dynamicRender.clear();

	camera->position = glm::vec3(3.5f, -6.0f, 13.0f);

	ghostThinkTimer = 0.0f;
	pacmanVel		= 6.0f;
	playerScore		= 0;

	std::string vert = "./shaders/testvertex.vert";
	std::string frag = "./shaders/testfragment.frag";

	Render wallRenderer;
	wallRenderer.init(vert, frag, 100, 100, camera);
	Render floorRenderer;
	floorRenderer.init(vert, frag, 10, 10, camera);
	Render teleporter;
	teleporter.init(vert, frag, 10, 10, camera);
	scoreRender.init(vert, frag, 100, 100, camera);

	level = std::make_unique<Level>(levelPath.c_str(), wallRenderer, floorRenderer, teleporter, scoreRender);

	Render pacmanRenderer;
	pacmanRenderer.init(vert, frag, 100, 100, camera);
	Model pacmanModel("assets/models/PacmanHigh.obj", pacmanRenderer);
	pacmanRenderer.setTexture(pacmanModel.TextureFromFile("textures/s_pac_a0_cmp4.png", "assets"));

	characterScale = 0.055f;

	pacmanRenderer.transform = glm::translate(pacmanRenderer.transform, level->pacmanSpawn);
	pacmanRenderer.transform = glm::rotate(pacmanRenderer.transform, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	pacmanRenderer.transform = glm::rotate(pacmanRenderer.transform, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	pacmanRenderer.transform = glm::scale(pacmanRenderer.transform, glm::vec3(glob::LEVEL_SCALE * characterScale, glob::LEVEL_SCALE * characterScale, glob::LEVEL_SCALE * characterScale));

	pacmanRenderer.direction		= glob::UP;
	pacmanRenderer.prevDirection	= glob::UP;
	pacmanRenderer.desiredDirection = glob::UP;
	pacmanRenderer.crashed			= false;

	dynamicRender.push_back(pacmanRenderer);

	for (int i = 1; i <= 4; i++) {
		Render ghost;
		ghost.init(vert, frag, 100, 100, camera);
		Model ghostModel("assets/models/PacmanHigh.obj", ghost);

		std::string path = "textures/ghost";
		path.append(std::to_string(i));
		path.append(".png");

		ghost.setTexture(pacmanModel.TextureFromFile(path.c_str(), "assets"));

		ghost.transform = glm::translate(ghost.transform, level->ghostSpawn);
		ghost.transform = glm::rotate(ghost.transform, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		ghost.transform = glm::rotate(ghost.transform, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		ghost.transform = glm::scale(ghost.transform, glm::vec3(glob::LEVEL_SCALE * characterScale, glob::LEVEL_SCALE * characterScale, glob::LEVEL_SCALE * characterScale));

		ghost.direction			= glob::UP;
		ghost.prevDirection		= glob::UP;
		ghost.desiredDirection	= glob::UP;
		ghost.crashed = false;

		dynamicRender.push_back(ghost);
	}
	//scoreRender.makeScoreHitbox();	
	staticRender.push_back(wallRenderer);
	staticRender.push_back(teleporter);
	staticRender.push_back(floorRenderer);
}

void Game::moveModel(Render& r, float deltaTime) {
	r.prevTransform = r.transform;

	if (r.direction != r.prevDirection) {
		glm::quat orientation;
		glm::decompose(r.transform, glm::vec3(), orientation, glm::vec3(), glm::vec3(), glm::vec4());

		int frameRotation = abs(r.direction - r.prevDirection);

		if (frameRotation == 180) {
			r.transform = glm::rotate(r.transform, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		}
		else {
			r.transform = glm::rotate(r.transform, glm::radians((float)(180 + r.direction - r.prevDirection % 360)), glm::vec3(0.0f, 1.0f, 0.0f));
		}

	}

	r.transform = glm::translate(r.transform, glm::vec3(0, 0, pacmanVel*deltaTime));
}

void Game::collided(Render& render) {
	render.direction = render.prevDirection;
	render.transform = render.prevTransform;
}

void Game::ghostAI() {
	for (auto ghost = dynamicRender.begin() + 1; ghost != dynamicRender.end(); ghost++) {
		ghost->crashed = false;

			while (true) {

				//if (ghost->crashed) {
				//	ghost->desiredDirection = (glob::Direction)((ghost->direction + 180) % 360);
				//	ghost->crashed = false;
				//	break;
				//}

				int random = rand() % 4;
				if (random == 0) ghost->desiredDirection = glob::UP;
				else if (random == 1) ghost->desiredDirection = glob::RIGHT;
				else if (random == 2) ghost->desiredDirection = glob::DOWN;
				else if (random == 3) ghost->desiredDirection = glob::LEFT;

				if (ghost->direction != ghost->desiredDirection) {
					if (ghost->direction == glob::UP && ghost->desiredDirection != glob::DOWN) break;
					else if (ghost->direction == glob::DOWN && ghost->desiredDirection != glob::UP) break;
					else if (ghost->direction == glob::LEFT && ghost->desiredDirection != glob::RIGHT) break;
					else if (ghost->direction == glob::RIGHT && ghost->desiredDirection != glob::LEFT) break;
				}
			}
	}
}