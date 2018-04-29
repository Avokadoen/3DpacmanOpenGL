#pragma once 

#include "glm.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/Importer.hpp"

#include "mesh.hpp"
#include "globals.hpp"
#include "render.hpp"

#include <vector>
#include <string>
#include <iostream>

/*
	This class is ment as an interface for simple
	manipulation of loaded meshes and loading models 
	from file
*/

class Model {

public:
	Model();
	Model			(std::string path, Render& renderer);
	
	void move		(glm::mat4 translate);
	void scale		(glm::mat4 scale);
	void rotate		(glm::mat4 rotation);
	

	void newChildMesh(Mesh mesh);
	unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);

protected:
	virtual bool loadModel(std::string path, Render& renderer);
	std::vector<Mesh> meshes;
private:
	static std::vector<glob::Texture> textures_loaded;
	std::string directory;

	std::vector<GLfloat> hitbox;

	void processNode(aiNode *node, const aiScene *scene, Render& renderer);
	Mesh processMesh(aiMesh *mesh, const aiScene *scene, Render& renderer);

	std::vector<glob::Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
		std::string typeName);
	// TODO: maybe move this to another class
	
};