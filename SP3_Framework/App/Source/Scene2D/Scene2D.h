/**
 CScene2D
 @brief A class which manages the 2D game scene
 By: Toh Da Jun
 Date: Mar 2020
 */
#pragma once

// Include SingletonTemplate
#include "DesignPatterns\SingletonTemplate.h"

// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

// Include Shader Manager
#include "RenderControl\ShaderManager.h"

// Include the Map2D as we will use it to check the player's movements and actions
#include "Map2D.h"
// Include CPlayer2D
#include "Player2D.h"
// Include CEnemy2D
#include "Enemy2D.h"

// Include CWoodCrawler
#include "WoodCrawler.h"

// Include CGlutton
#include "Glutton.h"

// Include the MiniBoss
#include "BloodDeer.h"


// Include vector
#include <vector>

// Include Keyboard controller
#include "Inputs\KeyboardController.h"

// GUI_Scene2D
#include "GUI_Scene2D.h"

// Game Manager
#include "GameManager.h"

// Include SoundController
#include "..\SoundController\SoundController.h"

#include "EnemyProjectile.h"


class CScene2D : public CSingletonTemplate<CScene2D>
{
	friend CSingletonTemplate<CScene2D>;
public:
	// Init
	bool Init(void);

	// Update
	bool Update(const double dElapsedTime);

	// PreRender
	void PreRender(void);

	// Render
	void Render(void);

	// PostRender
	void PostRender(void);

	glm::vec3 bgColor;

	bool getTime(void);
	void setTime(bool time);
	float getDuration(void); // How long has it been day / night
	void resetDuration(void); //Reset the counter ^
	vector<CEntity2D*>returnEnemyVector(void); // Returns the enemy vector 
	void setNewEnemyVector(vector<CEntity2D*>& newList);// Set new enemy vector
	// Push Bullet into the vector
	void pushBullet(CEntity2D* bullet);

	// Push Arrow into the vector
	void pushArrow(CEntity2D* arrow);
	// Setter for bullet vector
	void setLiveBulletVector(vector<CEntity2D*>& vectorOfBullets);;

	// Getter for Bullet Vector
	vector<CEntity2D*> getLiveBulletVector(void);

	// Setter for arrow vector
	void setLiveArrowVector(vector<CEntity2D*>& vectorOfArrows);

	// Getter for arrow vector
	vector<CEntity2D*> getLiveArrowVector(void);

	
	

protected:
	// The handler containing the instance of the 2D Map
	CMap2D* cMap2D;
	// The handler containing the instance of CPlayer2Ds
	CPlayer2D* cPlayer2D;
	// A vector containing the instance of CEnemy2Ds
	vector<CEntity2D*> enemyVector;

	// Keyboard Controller singleton instance
	CKeyboardController* cKeyboardController;

	// A transformation matrix for controlling where to render the entities
	glm::mat4 transform;

	// Projectile Handler
	vector<CEntity2D*> liveBullets;

	// Arrow Projectile Handler
	vector<CEntity2D*> liveArrows;

	// GUI_Scene2D
	CGUI_Scene2D* cGUI_Scene2D;

	// Game Manager
	CGameManager* cGameManager;

	// Handler to the CSoundController
	CSoundController* cSoundController;

    

	bool day;

	float enemySpawnTimeCounter; // Countdown before spawning
	
	float enemySpawnRate; // Spawn rate

	float dayCounter; // helps with gradual change in the brightness when it changes from day to night vice versa

	// Constructor
	CScene2D(void);
	// Destructor
	virtual ~CScene2D(void);
};

