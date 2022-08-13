/**
 BackgroundEntity
 @brief A class which stores a background entity for the game states
 By: Lim Chian Song
 Date: May 2020
 */
#include "BackgroundEntity.h"

//For allowing creating of Quad Mesh 
#include "Primitives/MeshBuilder.h"

// Include ImageLoader
#include "System\ImageLoader.h"

#include <iostream>
#include <vector>

using namespace std;
CBackgroundEntity::CBackgroundEntity(std::string name)
{
	mesh = NULL;

	transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
	textureName = name;
}

CBackgroundEntity::~CBackgroundEntity()
{
	if (mesh)
	{
		delete mesh;
		mesh = NULL;
	}
}

bool CBackgroundEntity::Init()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Load a texture 
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID(textureName.c_str(), true);
	if (iTextureID == 0)
	{
		cout << "Unable to load " << textureName.c_str() << endl;
		return false;
	}

	//CS: Create the Quad Mesh using the mesh builder
	mesh = CMeshBuilder::GenerateQuad(glm::vec4(1, 1, 1, 1), 2, 2);

	return true;
}