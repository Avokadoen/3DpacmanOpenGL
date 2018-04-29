#pragma once

#include "model.hpp"
#include "globals.hpp"
#include "glm.hpp"
#include "score.hpp"

#include <sstream>
#include <array>

class Level : Model {
public:
	Level(std::string path, Render& wallRender, Render& floorRender, Render& teleportRender, Score& scoreRender);

	void overrideHitbox(Render& wallRender, std::array<std::array<int, glob::MAX_LEVEL_SIZE>, glob::MAX_LEVEL_SIZE>& verticesData);
	void createVertexDataFromIndex(Render& wallRender, int startX, int endX, int indexY);
	
	int winningScore;
	glm::vec3 pacmanSpawn;
	glm::vec3 ghostSpawn;
	std::vector<glm::vec3> teleportPos;
	std::vector<glob::Direction> teleportDir;
private:
	glob::Texture wallTexture;
	glob::Texture floorTexture;
	glob::Texture scoreTexture;
	glob::Texture teleporter;

	int objectCount;
	int dimX;
	int dimY;
	float levelScale;
	float boxScale;


	virtual bool loadModel(std::string path, Render& wallRender, Render& floorRender, Render& teleportRender, Score& scoreRender);
	void createLevelObject(float top[2], float bottom[2], Render& renderer);
	void createScoreObject(float top[2], float bottom[2], Score& renderer);
	void createFloorObject(float top[2], float bottom[2], Render& renderer);
};