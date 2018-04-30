#include "level.hpp"

/*
	this class is used by the game class to initialize a level
	and can create 3D data from level files
*/

Level::Level(std::string path, Render& wallrender, Render& floorRender, Render& teleportRender, Score& scoreRender) :  Model() {
	if (!loadModel(path, wallrender, floorRender, teleportRender, scoreRender)) {
		throw std::runtime_error("failed loading level. maybe path is invalid?");
	}
}


bool Level::loadModel(std::string path, Render& wallrender, Render& floorRender, Render& teleportRender, Score& scoreRender) {
	
	winningScore	= 0;
	objectCount		= 0;
	pacmanSpawn		= glm::vec3(0, 0, 0);
	ghostSpawn		= glm::vec3(0, 0, 0);

	wallTexture.id	= TextureFromFile("textures/wall.jpg", "assets");
	wallrender.setTexture(wallTexture.id);
	
	floorTexture.id = TextureFromFile("textures/floor.jpg", "assets");
	floorRender.setTexture(floorTexture.id);

	teleporter.id	= TextureFromFile("textures/teleporter.png", "assets");
	teleportRender.setTexture(teleporter.id);

	scoreTexture.id = TextureFromFile("textures/point.png", "assets");
	scoreRender.setTexture(scoreTexture.id);

	std::stringstream fileBuffer;

	std::ifstream levelFile;
	levelFile.open(path, std::ifstream::in);

	if (levelFile.good()) {
		fileBuffer << levelFile.rdbuf();
	}
	else {
		std::cout << "\nfailed to read file";
		return 0;
	}

	// retrieve level dimention
	dimX = atoi(&fileBuffer.str()[0]);
	dimY = atoi(&fileBuffer.str()[3]);

	// check if level is of valid size
	if (dimY >= glob::MAX_LEVEL_SIZE || dimX >= glob::MAX_LEVEL_SIZE) {
		// TODO: throw
		std::cout << "level size is too big";
		return 0;
	}

	levelScale = glob::LEVEL_SCALE / (dimY / 2);

	bool drawing = false;
	int topLeft[2];
	int bottomRight[2];
	for (int i = 0; i < 2; i++) {
		topLeft[i] = -1;
		bottomRight[i] = -1;
	}

	std::string fileContent = fileBuffer.str();
	fileContent.erase(remove_if(fileContent.begin(), fileContent.end(), isspace), fileContent.end());

	int indexX, indexY = 0;
	boxScale = levelScale / 2;

	int strIndex = 5;

	int levelDetail = dimY*dimX;

	bool setPacmanSpawn = false;
	bool setGhostSpawn	= false;

	std::array<std::array<int, glob::MAX_LEVEL_SIZE>, glob::MAX_LEVEL_SIZE> verticesData;
	for (int i = 5; i < levelDetail + 5; i++) {
		
		int levelDataIndex = i - 5;
		indexX = levelDataIndex % (dimX);
		indexY = (levelDataIndex / (dimX));
		
		if (fileContent[i] == '1') {
			verticesData[indexY][indexX] = 1;

			float topPoint[2] = {
				(indexX * levelScale),
				(indexY * levelScale)	
			};

			float bottomPoint[2] = {
				(indexX * levelScale + boxScale * 2),			
				(indexY * levelScale) - boxScale * 2
			};
			createLevelObject(topPoint, bottomPoint, wallrender);

		}

		else if (fileContent[i] == '0') {
			verticesData[indexY][indexX] = 0;

			float floorTopPoint[2] = {
				(indexX * levelScale),
				(indexY * levelScale)
			};

			float floorBottomPoint[2] = {
				(indexX * levelScale + boxScale * 2),
				(indexY * levelScale) - boxScale * 2
			};

			float scoreTopPoint[2] = {
				(indexX * levelScale + boxScale * 0.75),
				(indexY * levelScale - boxScale * 1.25)
			};

			float scoreBottomPoint[2] = {
				(indexX * levelScale + boxScale * 1.25),
				(indexY * levelScale) - boxScale * 0.75
			};

			if (!setPacmanSpawn) {
				pacmanSpawn.x = levelScale * (indexX) + boxScale;
				pacmanSpawn.y = levelScale * (indexY) - boxScale;
				pacmanSpawn.z = 0.02f;
				setPacmanSpawn = true;
			}

			if (indexX > dimX / 2 && indexY > dimY / 2 && !setGhostSpawn) {
				ghostSpawn.x = levelScale * (indexX)+boxScale;
				ghostSpawn.y = levelScale * (indexY)-boxScale;
				ghostSpawn.z = 0.02f;
				setGhostSpawn = true;
			}
		
			if (indexX == 0 || indexX == dimX - 1 || indexY == 0 || indexY == dimY - 1) {
				createFloorObject(floorTopPoint, floorBottomPoint, teleportRender);

				glm::vec3 pos;
				pos.z = 0.02f;
				if (indexX == 0) {
					teleportDir.push_back(glob::RIGHT);
					pos.x = levelScale * (indexX)+boxScale * 3;
					pos.y = levelScale * (indexY)-boxScale;
				}
				else if(indexX == dimX - 1) {
					teleportDir.push_back(glob::LEFT);
					pos.x = levelScale * (indexX)-boxScale * 3;
					pos.y = levelScale * (indexY)-boxScale;
				}
				else if (indexY == 0) {
					teleportDir.push_back(glob::DOWN);
					pos.x = levelScale * (indexX)+boxScale;
					pos.y = levelScale * (indexY)+boxScale * 3;
				}
				else if (indexY == dimY - 1) {
					teleportDir.push_back(glob::UP);
					pos.x = levelScale * (indexX)+boxScale;
					pos.y = levelScale * (indexY)-boxScale * 3;
				}
				teleportPos.push_back(pos);
			}
			else{ 
				createFloorObject(floorTopPoint, floorBottomPoint, floorRender);
				createScoreObject(scoreTopPoint, scoreBottomPoint, scoreRender);
			}
			
			
		}
	}

	overrideHitbox(wallrender, verticesData);

	return 1;
}

void Level::overrideHitbox(Render& wallRender, std::array<std::array<int, glob::MAX_LEVEL_SIZE>, glob::MAX_LEVEL_SIZE>& verticesData) {
	
	wallRender.hitboxes.clear();

	for (int y = 0; y < dimY; y++) {
		for (int x = 0; x < dimX; x++) {
			int startX = -999;
			int endX = -999;
			if (verticesData[y][x] == 1) {
				startX = x;
				while (++x < dimX && verticesData[y][x] == 1) {
					endX = x;
				}
			}
			if (startX >= 0) {
				if (endX < 0) endX = startX;
				createVertexDataFromIndex(wallRender, startX, endX, y);
			}
		
		}
	}
	
}

void Level::createVertexDataFromIndex(Render& wallRender, int startX, int endX, int indexY) {
	float top[2] = {
		(startX * levelScale),
		(indexY * levelScale)
	};

	float bottom[2] = {
		(endX * levelScale + boxScale * 2),
		(indexY * levelScale) - boxScale * 2
	};

	std::vector<glob::Vertex> vertices = {
		{ glm::vec3{ top[0],		bottom[1],		glob::LEVEL_BOTTOM },		// Position
		glm::vec3{ 0,		0,		0 },										// Normals
		glm::vec2{ 0,		0 } },												// TexCoords

		{ glm::vec3{ bottom[0],	bottom[1],		glob::LEVEL_BOTTOM },			// Position
		glm::vec3{ 0,		0,		0 },										// Normals
		glm::vec2{ 1,		0 } },												// TexCoords

		{ glm::vec3{ top[0],		top[1],			glob::LEVEL_BOTTOM },		// ..
		glm::vec3{ 0,		0,		0 },										// ...
		glm::vec2{ 0,		1 } },												// ....

		{ glm::vec3{ bottom[0],	top[1],			glob::LEVEL_BOTTOM },	
		glm::vec3{ 0,		0,		0 },								
		glm::vec2{ 1,		1 } }	,									

		{ glm::vec3{ top[0],		bottom[1],	boxScale + glob::LEVEL_BOTTOM },	
		glm::vec3{ 0,		0,		0 },								
		glm::vec2{ 1,		1 } }	,									

		{ glm::vec3{ bottom[0],	bottom[1],		boxScale + glob::LEVEL_BOTTOM },	
		glm::vec3{ 0,		0,		0 },								
		glm::vec2{ 0,		1 } }	,									

		{ glm::vec3{ top[0],		top[1],		boxScale + glob::LEVEL_BOTTOM },	
		glm::vec3{ 0,		0,		0 },								
		glm::vec2{ 1,		0 } }	,									

		{ glm::vec3{ bottom[0],	top[1],			boxScale + glob::LEVEL_BOTTOM },	
		glm::vec3{ 0,		0,		0 },							
		glm::vec2{ 0,		0 } }									

	};

	wallRender.hitboxes.push_back(wallRender.createHitbox(vertices));
}


void Level::createLevelObject(float top[2], float bottom[2], Render& renderer) {

	float localBoxScale = abs(top[0] - bottom[0]);

	std::vector<glob::Vertex> vertices = {			
		{glm::vec3{	top[0],		bottom[1],		glob::LEVEL_BOTTOM},	// Position
		glm::vec3{	0,		0,		0},									// Normals
		glm::vec2{	0,		0 }},										// TexCoords

		{glm::vec3{ bottom[0],	bottom[1],		glob::LEVEL_BOTTOM },	// Position
		glm::vec3{ 0,		0,		0 },								// Normals
		glm::vec2{ 1,		0 }},										// TexCoords

		{glm::vec3{ top[0],		top[1],			glob::LEVEL_BOTTOM },	// Position
		glm::vec3{ 0,		0,		0 },								// Normals
		glm::vec2{ 0,		1 }},										// TexCoords

		{glm::vec3{ bottom[0],	top[1],			glob::LEVEL_BOTTOM },	// Position
		glm::vec3{ 0,		0,		0 },								// Normals
		glm::vec2{ 1,		1 } }	,									// TexCoords

		{ glm::vec3{ top[0],		bottom[1],	localBoxScale + glob::LEVEL_BOTTOM },	// Position
		glm::vec3{ 0,		0,		0 },								// Normals
		glm::vec2{ 1,		1 } }	,									// TexCoords

		{ glm::vec3{ bottom[0],	bottom[1],		localBoxScale + glob::LEVEL_BOTTOM },	// Position
		glm::vec3{ 0,		0,		0 },								// Normals
		glm::vec2{ 0,		1 } }	,									// TexCoords

		{ glm::vec3{ top[0],		top[1],		localBoxScale + glob::LEVEL_BOTTOM },	// Position
		glm::vec3{ 0,		0,		0 },								// Normals
		glm::vec2{ 1,		0 } }	,									// TexCoords

		{ glm::vec3{ bottom[0],	top[1],			localBoxScale + glob::LEVEL_BOTTOM },	// Position
		glm::vec3{ 0,		0,		0 },								// Normals
		glm::vec2{ 0,		0 } }										// TexCoords

	};

	std::vector<GLuint> indices{
		0, 1, 2,
		2, 1, 3,
		1, 3, 7,
		1, 5, 7,
		0, 1, 5,
		0, 5, 4,
		0, 2, 6,
		0, 6, 4,
		2, 3, 7,
		2, 7, 6,
		4, 5, 7,
		4, 6, 7
	};

	Mesh mesh(vertices);
	std::vector<glob::Texture> t;
	renderer.registerMesh(mesh, vertices, indices, t);
	//meshes.push_back(mesh);
}


void Level::createScoreObject(float top[2], float bottom[2], Score& renderer) {

	float localBoxScale = abs(top[0] - bottom[0]);

	std::vector<glob::Vertex> vertices = {
		{ glm::vec3{ top[0],		bottom[1],		glob::LEVEL_BOTTOM },	// Position
		glm::vec3{ 0,		0,		0 },									// Normals
		glm::vec2{ 0,		0 } },										// TexCoords

		{ glm::vec3{ bottom[0],	bottom[1],		glob::LEVEL_BOTTOM },	// Position
		glm::vec3{ 0,		0,		0 },								// Normals
		glm::vec2{ 1,		0 } },										// TexCoords

		{ glm::vec3{ top[0],		top[1],			glob::LEVEL_BOTTOM },	// Position
		glm::vec3{ 0,		0,		0 },								// Normals
		glm::vec2{ 0,		1 } },										// TexCoords

		{ glm::vec3{ bottom[0],	top[1],			glob::LEVEL_BOTTOM },	// Position
		glm::vec3{ 0,		0,		0 },								// Normals
		glm::vec2{ 1,		1 } }	,									// TexCoords

		{ glm::vec3{ top[0],		bottom[1],	localBoxScale + glob::LEVEL_BOTTOM },	// Position
		glm::vec3{ 0,		0,		0 },								// Normals
		glm::vec2{ 1,		1 } }	,									// TexCoords

		{ glm::vec3{ bottom[0],	bottom[1],		localBoxScale + glob::LEVEL_BOTTOM },	// Position
		glm::vec3{ 0,		0,		0 },								// Normals
		glm::vec2{ 0,		1 } }	,									// TexCoords

		{ glm::vec3{ top[0],		top[1],		localBoxScale + glob::LEVEL_BOTTOM },	// Position
		glm::vec3{ 0,		0,		0 },								// Normals
		glm::vec2{ 1,		0 } }	,									// TexCoords

		{ glm::vec3{ bottom[0],	top[1],			localBoxScale + glob::LEVEL_BOTTOM },	// Position
		glm::vec3{ 0,		0,		0 },								// Normals
		glm::vec2{ 0,		0 } }										// TexCoords

	};

	std::vector<GLuint> indices{
		0, 1, 2,
		2, 1, 3,
		1, 3, 7,
		1, 5, 7,
		0, 1, 5,
		0, 5, 4,
		0, 2, 6,
		0, 6, 4,
		2, 3, 7,
		2, 7, 6,
		4, 5, 7,
		4, 6, 7
	};

	winningScore += glob::SCORE_POINT;

	Mesh mesh(vertices);
	std::vector<glob::Texture> t;
	renderer.registerMesh(mesh, vertices, indices, t);
	//meshes.push_back(mesh);
}


void Level::createFloorObject(float top[2], float bottom[2], Render& renderer) {
	// TODO: Score objects

	std::vector<glob::Vertex> vertices = {
		{ glm::vec3{ top[0],		bottom[1],	glob::LEVEL_BOTTOM },	// Position
		glm::vec3{ 0,		0,		0 },								// Normals
		glm::vec2{ 0,		0 } },										// TexCoords

		{ glm::vec3{ bottom[0],	bottom[1],		glob::LEVEL_BOTTOM },	// Position
		glm::vec3{ 0,		0,		0 },								// Normals
		glm::vec2{ 1,		0 } },										// TexCoords

		{ glm::vec3{ top[0],		top[1],		glob::LEVEL_BOTTOM },	// Position
		glm::vec3{ 0,		0,		0 },								// Normals
		glm::vec2{ 0,		1 } },										// TexCoords

		{ glm::vec3{ bottom[0],	top[1],			glob::LEVEL_BOTTOM },	// Position
		glm::vec3{ 0,		0,		0 },								// Normals
		glm::vec2{ 1,		1 } }	,									// TexCoord
	};

	std::vector<GLuint> indices{
		0, 1, 2,
		2, 1, 3,
	};

	Mesh mesh(vertices);
	std::vector<glob::Texture> t;
	renderer.registerMesh(mesh, vertices, indices, t);

}
