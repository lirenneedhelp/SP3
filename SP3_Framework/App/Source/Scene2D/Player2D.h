/**
 CPlayer2D
 @brief A class representing the player object
 By: Toh Da Jun
 Date: Mar 2020
 */
#pragma once

// Include Singleton template
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

// Include CEntity2D
#include "Primitives/Entity2D.h"

// Include the Map2D as we will use it to check the player's movements and actions
class CMap2D;

// Include Keyboard controller
#include "Inputs\KeyboardController.h"

//Include Mouse controller
#include "Inputs\MouseController.h"

// Include Physics2D
#include "Physics2D.h"

// Include AnimatedSprites
#include "Primitives/SpriteAnimation.h"

// Include InventoryManager
#include "InventoryManager.h"

// Include SoundController
#include "..\SoundController\SoundController.h"

class CPlayer2D : public CSingletonTemplate<CPlayer2D>, public CEntity2D
{
	friend CSingletonTemplate<CPlayer2D>;
public:

	// Init
	bool Init(void);

	// Reset
	bool Reset(void);

	// Update
	void Update(const double dElapsedTime);

	// PreRender
	void PreRender(void);

	// Render
	void Render(void);

	// PostRender
	void PostRender(void);

	// Don't add your variables here without any reason

	bool isMoving;

	void UpdateDefense(float damage);

	void setPlayerRuntimeColor(glm::vec4 color);

	vector<CEntity2D*> returnNearestEnemy(vector<CEntity2D*> enemyVector);

	CInventoryManager* returnPlayerHealth(void);

	float returnCharge(void);

protected:
	enum DIRECTION
	{
		LEFT = 0,
		RIGHT = 1,
		UP = 2,
		DOWN = 3,
		NUM_DIRECTIONS
	};
	enum ITEM_ID
	{
		SWORD_ID,
		BOW_ID,
		SPEAR_ID,
		AXE_ID,
		SHOVEL_ID,
		HELMET_ID,
		LEGGINGS_ID,
		BOOTS_ID,
		CHESTPLATE_ID,
		BIGRED_ID,
		BIGYELLOW_ID,
		BIGGREEN_ID,
		BIGBLUE_ID,
		WOOD_ID,
		DIRT_ID,
		TOTAL_NUM,
	};

	glm::vec2 vec2OldIndex;
	// Handler to the CMap2D instance
	CMap2D* cMap2D;

	// Keyboard Controller singleton instance
	CKeyboardController* cKeyboardController;

	// Mouse Controlelr singleton instance
	CMouseController* cMouseController;

	// Physics
	CPhysics2D cPhysics2D;

	//CS: Animated Sprite
	CSpriteAnimation* animatedSprites;

	// Current color
	glm::vec4 runtimeColour;

	// InventoryManager
	CInventoryManager* cInventoryManager;

	// InventoryItem
	CInventoryItem* cInventoryItem;
	
	CInventoryManager* damageOnPlayer;

	// List of enemies
	vector <CEntity2D*> enemyList;

	//Check whether the enemy is within range
	bool hitEnemy;

	//Players Attack Range
	float attackRange;

	// Attack Speed of the player
	float attackSpeed;

	// Count the number of jumps
	int iJumpCount;

	bool firstAttack;

	bool shoot;

	// Player Initial Damage

	float playerInitialDamage;

	float defense;

	// Handler to the CSoundController
	CSoundController* cSoundController;

	// Constructor
	CPlayer2D(void);

	// Destructor
	virtual ~CPlayer2D(void);

	// Constraint the player's position within a boundary
	void Constraint(DIRECTION eDirection = LEFT);

	// Check if a position is possible to move into
	bool CheckPosition(DIRECTION eDirection);

	// Check if the player is in mid-air
	bool IsMidAir(void);
	// Update Jump or Fall
	void UpdateJumpFall(const double dElapsedTime = 0.0166666666666667);

	// Let player interact with the map
	void InteractWithMap(void);

	// Update the health and lives
	void UpdateHealthLives(void);



	/*
		!CODE CHANGES START!
		functions for the breaking and building of blocks
	*/
	int direction;
	double breakinterval;
	void BreakBlocks(const double dElapsedTime);
	void BuildBlocks();
	int itemtype;
	int itemID;
	int helmetdefense;
	int chestplatedefense;
	int leggingsdefense;
	int bootsdefense;
	int RandItemGen();
	/*
		!CODE CHANGES END!
	*/
	bool speed;
	bool strength;
	bool bowequip;
	bool helmetequip;
	bool chestplateequip;
	bool leggingsequip;
	bool bootsequip;
	bool highjump;
	int jumps;

	// Checks how many seconds potion has been active
	float strength_runtime;
	float speed_runtime;


	// When Player Charges Up Bow
	float charge;

	float strengthValue;

	/*
	void spawnchest(void);
	bool validposition;
	int xChest;
	int yChest;
	*/
};

