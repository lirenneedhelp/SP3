/**
 Player2D
 @brief A class representing the player object
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "Player2D.h"

#include <iostream>
using namespace std;

// Include Shader Manager
#include "RenderControl\ShaderManager.h"

// Include ImageLoader
#include "System\ImageLoader.h"

// Include the Map2D as we will use it to check the player's movements and actions
#include "Map2D.h"
#include "Primitives/MeshBuilder.h"

// Include Game Manager
#include "GameManager.h"
#include "BloodDeer.h"
#include "Scene2D.h" 

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CPlayer2D::CPlayer2D(void)
	: cMap2D(NULL)
	, cKeyboardController(NULL)
	, cInventoryManager(NULL)
	, cInventoryItem(NULL)
	, cSoundController(NULL)
	, cMouseController(NULL)
	//, cProjectile(NULL)
{
	transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first

	// Initialise vecIndex
	vec2Index = glm::i32vec2(0);

	// Initialise vecNumMicroSteps
	vec2NumMicroSteps = glm::i32vec2(0);

	// Initialise vec2UVCoordinate
	vec2UVCoordinate = glm::vec2(0.0f);

}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CPlayer2D::~CPlayer2D(void)
{
	// We won't delete this since it was created elsewhere
	cSoundController = NULL;

	// We won't delete this since it was created elsewhere
	cInventoryManager = NULL;

	// We won't delete this since it was created elsewhere
	cKeyboardController = NULL;

	// We won't delete this since it was created elsewhere
	cMouseController = NULL;

	// We won't delete this since it was created elsewhere
	cMap2D = NULL;

	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAO);
}

/**
  @brief Initialise this instance
  */
bool CPlayer2D::Init(void)
{
	// Store the keyboard controller singleton instance here
	cKeyboardController = CKeyboardController::GetInstance();
	// Reset all keys since we are starting a new game
	cKeyboardController->Reset();

	// Store the mouse controller singleton instance here
	cMouseController = CMouseController::GetInstance();

	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();

	// Get the handler to the CMap2D instance
	cMap2D = CMap2D::GetInstance();
	// Find the indices for the player in arrMapInfo, and assign it to cPlayer2D
	unsigned int uiRow = -1;
	unsigned int uiCol = -1;
	if (cMap2D->FindValue(200, uiRow, uiCol) == false)
		return false;	// Unable to find the start position of the player, so quit this game

	// Erase the value of the player in the arrMapInfo
	cMap2D->SetMapInfo(uiRow, uiCol, 0);

	// Set the start position of the Player to iRow and iCol
	vec2Index = glm::i32vec2(uiCol, uiRow);

	isMoving = true; // Check if woodCrawler has pulled the player.

	highjump = false; // Check Whether player has taken jump boost potion
	jumps = 0;
	speed = false;
	strength = false;
	attackRange = 1.0f;
	hitEnemy = false;
	defense = 0;
	firstAttack = true; // Check if it's the first click

	//check if weapons are equiped
	swordequip = false;
	spearequip = false;
	axeequip = false;
	bowequip = false;
	shovelequip = false;
	helmetequip = false;
	chestplateequip = false;
	leggingsequip = false;
	bootsequip = false;

	playerInitialDamage = 10;
	
	// By default, microsteps should be zero
	vec2NumMicroSteps = glm::i32vec2(0, 0);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	
	// Load the player texture 
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/player.png", true);
	if (iTextureID == 0)
	{
		std::cout << "Unable to load Image/player.png" << endl;
		return false;
	}

	//CS: Create the animated sprite and setup the animation 
	animatedSprites = CMeshBuilder::GenerateSpriteAnimation(11, 7, cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
	animatedSprites->AddAnimation("idle", 0, 6);
	animatedSprites->AddAnimation("right", 7, 13);
	animatedSprites->AddAnimation("left", 7, 13);
	animatedSprites->AddAnimation("jump", 14, 20);
	animatedSprites->AddAnimation("Attack1", 42, 56);


	//CS: Play the "idle" animation as default
	animatedSprites->PlayAnimation("idle", -1, 1.0f);

	//CS: Init the color to white
	runtimeColour = glm::vec4(1.0, 1.0, 1.0, 1.0);

	// Set the Physics to fall status by default
	cPhysics2D.Init();
	cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);

	// Get the handler to the CInventoryManager instance
	cInventoryManager = CInventoryManager::GetInstance();
	cInventoryItem = cInventoryManager->Add("Lives", "Image/Scene2D_Lives.tga", 3, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	cInventoryItem = cInventoryManager->Add("HealthPotion", "Image/Big_red.tga", 3, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	cInventoryItem = cInventoryManager->Add("StrengthPotion", "Image/Big_green.tga", 3, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	cInventoryItem = cInventoryManager->Add("SpeedPotion", "Image/Big_yellow.tga", 3, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	cInventoryItem = cInventoryManager->Add("JumpPotion", "Image/Big_blue.tga", 3, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	cInventoryItem = cInventoryManager->Add("sword", "Image/sword.tga", 100, 100);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	cInventoryItem = cInventoryManager->Add("spear", "Image/spear.tga", 100, 100);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	cInventoryItem = cInventoryManager->Add("bow", "Image/bow.tga", 100, 100);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	cInventoryItem = cInventoryManager->Add("axe", "Image/axe.tga", 100, 100);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	cInventoryItem = cInventoryManager->Add("shovel", "Image/shovel.tga", 100, 100);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	cInventoryItem = cInventoryManager->Add("helmet", "Image/helmet.tga", 100, 100);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	cInventoryItem = cInventoryManager->Add("chestplate", "Image/chestplate.tga", 100, 100);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	cInventoryItem = cInventoryManager->Add("leggings", "Image/leggings.tga", 100, 100);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	cInventoryItem = cInventoryManager->Add("boots", "Image/boots.tga", 100, 100);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	// Add a Health icon as one of the inventory items
	cInventoryItem = cInventoryManager->Add("Health", "Image/Scene2D_Health.tga", 100, 100);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	iJumpCount = 0;
	attackSpeed = 1.0f;

	direction = 0;
	breakinterval = 0.2f;

	// Get the handler to the CSoundController
	cSoundController = CSoundController::GetInstance();

	return true;
}

/**
 @brief Reset this instance
 */
bool CPlayer2D::Reset()
{
	unsigned int uiRow = -1;
	unsigned int uiCol = -1;
	if (cMap2D->FindValue(200, uiRow, uiCol) == false)
		return false;	// Unable to find the start position of the player, so quit this game

	// Erase the value of the player in the arrMapInfo
	cMap2D->SetMapInfo(uiRow, uiCol, 0);

	// Set the start position of the Player to iRow and iCol
	vec2Index = glm::i32vec2(uiCol, uiRow);
	// By default, microsteps should be zero
	vec2NumMicroSteps = glm::i32vec2(0, 0);

	//Set it to fall upon entering new level
	cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);

	//CS: Reset double jump
	iJumpCount = 0;

	//CS: Play the "idle" animation as default
	animatedSprites->PlayAnimation("idle", -1, 1.0f);

	//CS: Init the color to white
	runtimeColour = glm::vec4(1.0, 1.0, 1.0, 1.0);

	return true;
}

/**
 @brief Update this instance
 */
void CPlayer2D::Update(const double dElapsedTime)
{
	// Store the old position
	vec2OldIndex = vec2Index;
	// Get keyboard updates
	if (isMoving)
	{
		if (cKeyboardController->IsKeyDown(GLFW_KEY_A))
		{
			if (speed == true)
			{
				if (vec2Index.x >= 0)
				{
					vec2NumMicroSteps.x-=2;
					if (vec2NumMicroSteps.x < 0)
					{
						vec2NumMicroSteps.x = ((int)cSettings->NUM_STEPS_PER_TILE_XAXIS);
						vec2Index.x--;
					}
				}
			}
			else if (speed == false)
			{
				// Calculate the new position to the left
				if (vec2Index.x >= 0)
				{
					vec2NumMicroSteps.x--;
					if (vec2NumMicroSteps.x < 0)
					{
						vec2NumMicroSteps.x = ((int)cSettings->NUM_STEPS_PER_TILE_XAXIS) - 1;
						vec2Index.x--;
					}
				}
			}

			direction = 1;

			// Constraint the player's position within the screen boundary
			Constraint(LEFT);
			// If the new position is not feasible, then revert to old position
			if (CheckPosition(LEFT) == false)
			{
				vec2Index = vec2OldIndex;
				vec2NumMicroSteps.x = 0;
			}

			// Check if player is in mid-air, such as walking off a platform
			if (IsMidAir() == true)
			{
				if (cPhysics2D.GetStatus() != CPhysics2D::STATUS::JUMP)
					cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
			}

			//CS: Play the "left" animation
			animatedSprites->PlayAnimation("left", -1, 1.0f);

			//CS: Change Color
			runtimeColour = glm::vec4(1.0, 0.0, 0.0, 1.0);
		}
		else if (cKeyboardController->IsKeyDown(GLFW_KEY_D))
		{
			if (speed == true)
			{
				if (vec2Index.x < (int)cSettings->NUM_TILES_XAXIS)
				{
					vec2NumMicroSteps.x += 2;

					if (vec2NumMicroSteps.x >= cSettings->NUM_STEPS_PER_TILE_XAXIS)
					{
						vec2NumMicroSteps.x = 0;
						vec2Index.x++;
					}
				}
			}
			else if (speed == false)
			{
				// Calculate the new position to the right
				if (vec2Index.x < (int)cSettings->NUM_TILES_XAXIS)
				{
					vec2NumMicroSteps.x++;

					if (vec2NumMicroSteps.x >= cSettings->NUM_STEPS_PER_TILE_XAXIS)
					{
						vec2NumMicroSteps.x = 0;
						vec2Index.x++;
					}
				}
			}

			direction = 2;
			// Constraint the player's position within the screen boundary
			Constraint(RIGHT);

			// If the new position is not feasible, then revert to old position
			if (CheckPosition(RIGHT) == false)
			{
				vec2NumMicroSteps.x = 0;

			}

			// Check if player is in mid-air, such as walking off a platform
			if (IsMidAir() == true)
			{
				if (cPhysics2D.GetStatus() != CPhysics2D::STATUS::JUMP)
					cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
			}

			//CS: Play the "right" animation
			animatedSprites->PlayAnimation("right", -1, 1.0f);

			//CS: Change Color
			runtimeColour = glm::vec4(1.0, 1.0, 0.0, 1.0);

		}
			
		if (cKeyboardController->IsKeyDown(GLFW_KEY_W))
		{
			// Calculate the new position up
			if (vec2Index.y < (int)cSettings->NUM_TILES_YAXIS)
			{
				vec2NumMicroSteps.y++;
				if (vec2NumMicroSteps.y > cSettings->NUM_STEPS_PER_TILE_YAXIS)
				{
					vec2NumMicroSteps.y = 0;
					vec2Index.y++;
				}
			}

			// Constraint the player's position within the screen boundary
			Constraint(UP);

			// If the new position is not feasible, then revert to old position
			if (CheckPosition(UP) == false)
			{
				vec2NumMicroSteps.y = 0;
			}

			//CS: Play the "idle" animation
			animatedSprites->PlayAnimation("idle", -1, 1.0f);

			//CS: Change Color
			runtimeColour = glm::vec4(0.0, 1.0, 1.0, 0.5);
		}
		else if (cKeyboardController->IsKeyDown(GLFW_KEY_S))
		{
			// Calculate the new position down
			if (vec2Index.y >= 0)
			{
				vec2NumMicroSteps.y--;
				if (vec2NumMicroSteps.y < 0)
				{
					vec2NumMicroSteps.y = ((int)cSettings->NUM_STEPS_PER_TILE_YAXIS) - 1;
					vec2Index.y--;
				}
			}

			// Constraint the player's position within the screen boundary
			Constraint(DOWN);

			// If the new position is not feasible, then revert to old position
			if (CheckPosition(DOWN) == false)
			{
				vec2Index = vec2OldIndex;
				vec2NumMicroSteps.y = 0;
			}

			//CS: Play the "idle" animation
			animatedSprites->PlayAnimation("idle", -1, 1.0f);

			//CS: Change Color
			runtimeColour = glm::vec4(1.0, 0.0, 1.0, 0.5);
		}
		if (cKeyboardController->IsKeyPressed(GLFW_KEY_SPACE))
		{
			if (highjump == false)
			{
				if ((cPhysics2D.GetStatus() == CPhysics2D::STATUS::IDLE))
				{
					cPhysics2D.SetStatus(CPhysics2D::STATUS::JUMP);
					cPhysics2D.SetInitialVelocity(glm::vec2(0.0f, 2.f));
					iJumpCount += 1;
					// Play a jump sound
					cSoundController->PlaySoundByID(3);
					animatedSprites->PlayAnimation("jump", -1, 0.5f);
				}
				else
				{
					if (iJumpCount < 2)
					{
						cPhysics2D.SetStatus(CPhysics2D::STATUS::JUMP);
						cPhysics2D.SetInitialVelocity(glm::vec2(0.0f, 2.f));
						iJumpCount += 1;
						// Play a jump sound
						cSoundController->PlaySoundByID(3);
					}
				}
			}
			if (highjump == true)
			{
				if ((cPhysics2D.GetStatus() == CPhysics2D::STATUS::IDLE))
				{
					cPhysics2D.SetStatus(CPhysics2D::STATUS::JUMP);
					cPhysics2D.SetInitialVelocity(glm::vec2(0.0f, 3.f));
					iJumpCount += 1;
					++jumps;
					// Play a jump sound
					cSoundController->PlaySoundByID(3);
				}
				else
				{
					if (iJumpCount < 2)
					{
						cPhysics2D.SetInitialVelocity(glm::vec2(0.0f, 3.f));
						iJumpCount += 1;
						// Play a jump sound
						cSoundController->PlaySoundByID(3);
					}
				}
				if (jumps == 3)
				{
					highjump = false;
				}

			}
		}
		
	}
	//Put mouse inputs here
	if (cMouseController->IsButtonDown(0))
	{
		if (firstAttack)
		{
			//cout << "hello I've LEFT CLICKED\n";
			animatedSprites->PlayAnimation("Attack1", -1, 1.0f);
			firstAttack = false;
		}
		else 
		{ 
			attackSpeed -= dElapsedTime;
			if (attackSpeed <= 0.f)
			{
				
				//cout << "hello I'm holding left click\n";
				//damage the enemy and reset the interval
				attackSpeed = 1.0f;
				//if (cPhysics2D.CalculateDistance(vec2Index,))
				enemyList = CScene2D::GetInstance()->returnEnemyVector();
				//returnNearestEnemy();
				animatedSprites->PlayAnimation("Attack1", -1, 1.0f);
				for (int enemyIndex = 0; enemyIndex != enemyList.size(); ++enemyIndex)
				{
					if (cPhysics2D.CalculateDistance(vec2Index, enemyList[enemyIndex]->vec2Index) <= attackRange && vec2Index.y == enemyList[enemyIndex]->vec2Index.y) // Check if player and enemy are on the same level & check whether the enemy is within the player's range
					{
						std::cout << "Hit Enemy Once\n";
						//TO DO: REDUCE THEIR HP
						enemyList[enemyIndex]->health -= playerInitialDamage;
						std::cout << enemyList[enemyIndex]->health << endl;
						if (enemyList[enemyIndex]->health <= 0)
						{
							enemyList[enemyIndex]->~CEntity2D();					
							enemyList.erase(enemyList.begin() + enemyIndex);
							CScene2D::GetInstance()->setNewEnemyVector(enemyList);
						}
						break;
					}
				}
			}
		}

		if (swordequip = true)
		{
			attackSpeed -= dElapsedTime;
			if (attackSpeed <= 0.f)
			{
				//cout << "hello I'm holding left click\n";
				//damage the enemy and reset the interval
				attackSpeed = 2.0f;
				playerInitialDamage = 30;
				attackRange *= 1.5;
				//if (cPhysics2D.CalculateDistance(vec2Index,))
				enemyList = CScene2D::GetInstance()->returnEnemyVector();
				//returnNearestEnemy();
				animatedSprites->PlayAnimation("Attack1", -1, 1.0f);
				for (int enemyIndex = 0; enemyIndex != enemyList.size(); ++enemyIndex)
				{
					/*if (strength = true)
					{
						playerInitialDamage *= 1.5;
					}*/
					if (cPhysics2D.CalculateDistance(vec2Index, enemyList[enemyIndex]->vec2Index) <= attackRange && vec2Index.y == enemyList[enemyIndex]->vec2Index.y) // Check if player and enemy are on the same level & check whether the enemy is within the player's range
					{
						std::cout << "Hit Enemy Once\n";
						//TO DO: REDUCE THEIR HP
						enemyList[enemyIndex]->health -= playerInitialDamage;
						std::cout << enemyList[enemyIndex]->health << endl;
						if (enemyList[enemyIndex]->health <= 0)
						{
							enemyList[enemyIndex]->~CEntity2D();
							enemyList.erase(enemyList.begin() + enemyIndex);
							CScene2D::GetInstance()->setNewEnemyVector(enemyList);
						}
						break;
					}
				}
			}
		}
		else if (spearequip = true)
		{
			attackSpeed -= dElapsedTime;
			if (attackSpeed <= 0.f)
			{
				//cout << "hello I'm holding left click\n";
				//damage the enemy and reset the interval
				attackSpeed = 1.0f;
				playerInitialDamage = 40;
				attackRange *= 2;
				//if (cPhysics2D.CalculateDistance(vec2Index,))
				enemyList = CScene2D::GetInstance()->returnEnemyVector();
				//returnNearestEnemy();
				animatedSprites->PlayAnimation("Attack1", -1, 1.0f);
				for (int enemyIndex = 0; enemyIndex != enemyList.size(); ++enemyIndex)
				{
					/*if (strength = true)
					{
						playerInitialDamage *= 1.5;
					}*/
					if (cPhysics2D.CalculateDistance(vec2Index, enemyList[enemyIndex]->vec2Index) <= attackRange && vec2Index.y == enemyList[enemyIndex]->vec2Index.y) // Check if player and enemy are on the same level & check whether the enemy is within the player's range
					{
						std::cout << "Hit Enemy Once\n";
						//TO DO: REDUCE THEIR HP
						enemyList[enemyIndex]->health -= playerInitialDamage;
						std::cout << enemyList[enemyIndex]->health << endl;
						if (enemyList[enemyIndex]->health <= 0)
						{
							enemyList[enemyIndex]->~CEntity2D();
							enemyList.erase(enemyList.begin() + enemyIndex);
							CScene2D::GetInstance()->setNewEnemyVector(enemyList);
						}
						break;
					}
				}
			}
		}
		else if (bowequip = true)
		{
			attackSpeed -= dElapsedTime;
			if (attackSpeed <= 0.f)
			{
				//cout << "hello I'm holding left click\n";
				//damage the enemy and reset the interval
				attackSpeed = 1.0f;
				playerInitialDamage = 20;
				attackRange *= 2;
				//if (cPhysics2D.CalculateDistance(vec2Index,))
				enemyList = CScene2D::GetInstance()->returnEnemyVector();
				//returnNearestEnemy();
				animatedSprites->PlayAnimation("Attack1", -1, 1.0f);
				for (int enemyIndex = 0; enemyIndex != enemyList.size(); ++enemyIndex)
				{
					/*if (strength = true)
					{
						playerInitialDamage *= 1.5;
					}*/
					if (cPhysics2D.CalculateDistance(vec2Index, enemyList[enemyIndex]->vec2Index) <= attackRange && vec2Index.y == enemyList[enemyIndex]->vec2Index.y) // Check if player and enemy are on the same level & check whether the enemy is within the player's range
					{
						std::cout << "Hit Enemy Once\n";
						//TO DO: REDUCE THEIR HP
						enemyList[enemyIndex]->health -= playerInitialDamage;
						std::cout << enemyList[enemyIndex]->health << endl;
						if (enemyList[enemyIndex]->health <= 0)
						{
							enemyList[enemyIndex]->~CEntity2D();
							enemyList.erase(enemyList.begin() + enemyIndex);
							CScene2D::GetInstance()->setNewEnemyVector(enemyList);
						}
						break;
					}
				}
			}
		}
		if (strength = true)
		{
			playerInitialDamage *= 1.5;
		}
	}
    else if (cMouseController->IsButtonUp(0))
	{
		if (!firstAttack)
		{
			//cout << "Hello I've released my left click\n";
			animatedSprites->PlayAnimation("Idle", -1, 1.0f);
			firstAttack = true;
		}
	}

	//cMouseController->PostUpdate();


	//cMouseController->PostUpdate();
	
	
	BuildBlocks();
	BreakBlocks(dElapsedTime);

	// Update Jump or Fall
	//CS: Will cause error when debugging. Set to default elapsed time
	UpdateJumpFall(dElapsedTime);

	// Interact with the Map
	InteractWithMap();

	// Update the Health and Lives
	UpdateHealthLives();

	//CS: Update the animated sprite
	animatedSprites->Update(dElapsedTime);

	// Update the UV Coordinates
	vec2UVCoordinate.x = cSettings->ConvertIndexToUVSpace(cSettings->x, vec2Index.x, false, vec2NumMicroSteps.x*cSettings->MICRO_STEP_XAXIS);
	vec2UVCoordinate.y = cSettings->ConvertIndexToUVSpace(cSettings->y, vec2Index.y, false, vec2NumMicroSteps.y*cSettings->MICRO_STEP_YAXIS);
}

/**
 @brief Set up the OpenGL display environment before rendering
 */
void CPlayer2D::PreRender(void)
{
	// Activate blending mode
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Activate the shader
	CShaderManager::GetInstance()->Use(sShaderName);
}

/**
 @brief Render this instance
 */
void CPlayer2D::Render(void)
{
	glBindVertexArray(VAO);
	// get matrix's uniform location and set matrix
	unsigned int transformLoc = glGetUniformLocation(CShaderManager::GetInstance()->activeShader->ID, "transform");
	unsigned int colorLoc = glGetUniformLocation(CShaderManager::GetInstance()->activeShader->ID, "runtimeColour");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

	transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
	transform = glm::translate(transform, glm::vec3(vec2UVCoordinate.x,
													vec2UVCoordinate.y,
													0.0f));
	// Update the shaders with the latest transform
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
	glUniform4fv(colorLoc, 1, glm::value_ptr(runtimeColour));

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);
	// Get the texture to be rendered
	glBindTexture(GL_TEXTURE_2D, iTextureID);

		//CS: Render the animated sprite
		glBindVertexArray(VAO);
		animatedSprites->Render();
		glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

/**
 @brief PostRender Set up the OpenGL display environment after rendering.
 */
void CPlayer2D::PostRender(void)
{
	// Disable blending
	glDisable(GL_BLEND);
}

CInventoryManager* CPlayer2D::returnPlayerHealth(void)
{
	return cInventoryManager;
}


/**
 @brief Constraint the player's position within a boundary
 @param eDirection A DIRECTION enumerated data type which indicates the direction to check
 */
void CPlayer2D::Constraint(DIRECTION eDirection)
{
	if (eDirection == LEFT)
	{	
		if (vec2Index.x < 0)
		{
			vec2Index.x = 0;
			vec2NumMicroSteps.x = 0;
		}
	}
	else if (eDirection == RIGHT)
	{
		if (vec2Index.x >= (int)cSettings->NUM_TILES_XAXIS - 1)
		{
			vec2Index.x = ((int)cSettings->NUM_TILES_XAXIS) - 1;
			vec2NumMicroSteps.x = 0;
		}
	}
	else if (eDirection == UP)
	{
		if (vec2Index.y >= (int)cSettings->NUM_TILES_YAXIS - 1)
		{
			vec2Index.y = ((int)cSettings->NUM_TILES_YAXIS) - 1;
			vec2NumMicroSteps.y = 0;
		}
	}
	else if (eDirection == DOWN)
	{
		if (vec2Index.y < 0)
		{
			vec2Index.y = 0;
			vec2NumMicroSteps.y = 0;
		}
	}
	else
	{
		std::cout << "CPlayer2D::Constraint: Unknown direction." << endl;
	}
}

/**
 @brief Check if a position is possible to move into
 @param eDirection A DIRECTION enumerated data type which indicates the direction to check
 */
bool CPlayer2D::CheckPosition(DIRECTION eDirection)
{
	if (eDirection == LEFT)
	{
		// If the new position is fully within a row, then check this row only
		if (vec2NumMicroSteps.y == 0)
		{
			// If the grid is not accessible, then return false
			if (cMap2D->GetMapInfo(vec2Index.y, vec2Index.x) >= 100)
			{
				return false;
			}
		}
		// If the new position is between 2 rows, then check both rows as well
		else if (vec2NumMicroSteps.y != 0)
		{
			// If the 2 grids are not accessible, then return false
			if ((cMap2D->GetMapInfo(vec2Index.y, vec2Index.x) >= 100) ||
				(cMap2D->GetMapInfo(vec2Index.y + 1, vec2Index.x) >= 100))
			{
				return false;
			}
		}
	}
	else if (eDirection == RIGHT)
	{
		// If the new position is at the top row, then return true
		if (vec2Index.x >= cSettings->NUM_TILES_XAXIS - 1)
		{
			vec2NumMicroSteps.x = 0;
			return true;
		}

		// If the new position is fully within a row, then check this row only
		if (vec2NumMicroSteps.y == 0)
		{
			// If the grid is not accessible, then return false
			if (cMap2D->GetMapInfo(vec2Index.y, vec2Index.x + 1) >= 100)
			{
				return false;
			}
		}
		// If the new position is between 2 rows, then check both rows as well
		else if (vec2NumMicroSteps.y != 0)
		{
			// If the 2 grids are not accessible, then return false
			if ((cMap2D->GetMapInfo(vec2Index.y, vec2Index.x + 1) >= 100) ||
				(cMap2D->GetMapInfo(vec2Index.y + 1, vec2Index.x + 1) >= 100))
			{
				return false;
			}
		}

	}
	else if (eDirection == UP)
	{
		// If the new position is at the top row, then return true
		if (vec2Index.y >= cSettings->NUM_TILES_YAXIS - 1)
		{
			vec2NumMicroSteps.y = 0;
			return true;
		}

		// If the new position is fully within a column, then check this column only
		if (vec2NumMicroSteps.x == 0)
		{
			// If the grid is not accessible, then return false
			if (cMap2D->GetMapInfo(vec2Index.y + 1, vec2Index.x) >= 100)
			{
				return false;
			}
		}
		// If the new position is between 2 columns, then check both columns as well
		else if (vec2NumMicroSteps.x != 0)
		{
			// If the 2 grids are not accessible, then return false
			if ((cMap2D->GetMapInfo(vec2Index.y + 1, vec2Index.x) >= 100) ||
				(cMap2D->GetMapInfo(vec2Index.y + 1, vec2Index.x + 1) >= 100))
			{
				return false;
			}
		}
	}
	else if (eDirection == DOWN)
	{
		// If the new position is fully within a column, then check this column only
		if (vec2NumMicroSteps.x == 0)
		{
			// If the grid is not accessible, then return false
			if (cMap2D->GetMapInfo(vec2Index.y, vec2Index.x) >= 100)
			{
				return false;
			}
		}
		// If the new position is between 2 columns, then check both columns as well
		else if (vec2NumMicroSteps.x != 0)
		{
			// If the 2 grids are not accessible, then return false
			if ((cMap2D->GetMapInfo(vec2Index.y, vec2Index.x) >= 100) ||
				(cMap2D->GetMapInfo(vec2Index.y, vec2Index.x + 1) >= 100))
			{
				return false;
			}
		}
	}
	else
	{
		std::cout << "CPlayer2D::CheckPosition: Unknown direction." << endl;
	}

	return true;
}

// Check if the player is in mid-air
bool CPlayer2D::IsMidAir(void)
{
	// if the player is at the bottom row, then he is not in mid-air for sure
	if (vec2Index.y == 0)
		return false;

	// Check if the tile below the player's current position is empty
	if ((vec2NumMicroSteps.x == 0) && 
		(cMap2D->GetMapInfo(vec2Index.y-1, vec2Index.x) == 0))
	{
		return true;
	}

	return false;
}

// Update Jump or Fall
void CPlayer2D::UpdateJumpFall(const double dElapsedTime)
{
	if (cPhysics2D.GetStatus() == CPhysics2D::STATUS::JUMP)
	{
		// Update the elapsed time to the physics engine
		cPhysics2D.SetTime((float)dElapsedTime);
		// Call the physics engine update method to calculate the final velocity and displacement
		cPhysics2D.Update();
		// Get the displacement from the physics engine
		glm::vec2 v2Displacement = cPhysics2D.GetDisplacement();

		// Store the current vec2Index.y
		int iIndex_YAxis_OLD = vec2Index.y;

		int iDisplacement_MicroSteps = (int)(v2Displacement.y / cSettings->MICRO_STEP_YAXIS); //Displacement divide by distance for 1 microstep
		if (vec2Index.y < (int)cSettings->NUM_TILES_YAXIS)
		{
			vec2NumMicroSteps.y += iDisplacement_MicroSteps;
			if (vec2NumMicroSteps.y > cSettings->NUM_STEPS_PER_TILE_YAXIS)
			{
				vec2NumMicroSteps.y -= cSettings->NUM_STEPS_PER_TILE_YAXIS;
				if (vec2NumMicroSteps.y < 0)
					vec2NumMicroSteps.y = 0;
				vec2Index.y++;
			}
		}

		// Constraint the player's position within the screen boundary
		Constraint(UP);

		// Iterate through all rows until the proposed row
		// Check if the player will hit a tile; stop jump if so.
		int iIndex_YAxis_Proposed = vec2Index.y;
		for (int i = iIndex_YAxis_OLD; i <= iIndex_YAxis_Proposed; i++)
		{
			// Change the player's index to the current i value
			vec2Index.y = i;
			// If the new position is not feasible, then revert to old position
			if (isMoving)
			{
				if (CheckPosition(UP) == false)
				{
					// Align with the row
					vec2NumMicroSteps.y = 0;
					// Set the Physics to fall status
					cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
					break;
				}
			}
		}

		// If the player is still jumping and the initial velocity has reached zero or below zero, 
		// then it has reach the peak of its jump
		if ((cPhysics2D.GetStatus() == CPhysics2D::STATUS::JUMP) && (cPhysics2D.GetDisplacement().y <= 0.0f))
		{
			// Set status to fall
			cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
		}
	}
	else if (cPhysics2D.GetStatus() == CPhysics2D::STATUS::FALL)
	{
		// Update the elapsed time to the physics engine
		cPhysics2D.SetTime((float)dElapsedTime);
		// Call the physics engine update method to calculate the final velocity and displacement
		cPhysics2D.Update();
		// Get the displacement from the physics engine
		glm::vec2 v2Displacement = cPhysics2D.GetDisplacement();

		// Store the current vec2Index.y
		int iIndex_YAxis_OLD = vec2Index.y;

		// Translate the displacement from pixels to indices
		int iDisplacement_MicroSteps = (int)(v2Displacement.y / cSettings->MICRO_STEP_YAXIS);

		if (vec2Index.y >= 0)
		{
			vec2NumMicroSteps.y -= fabs(iDisplacement_MicroSteps);
			if (vec2NumMicroSteps.y < 0)
			{
				vec2NumMicroSteps.y = ((int)cSettings->NUM_STEPS_PER_TILE_YAXIS) - 1;
				vec2Index.y--;
			}
		}

		// Constraint the player's position within the screen boundary
		Constraint(DOWN);

		// Iterate through all rows until the proposed row
		// Check if the player will hit a tile; stop fall if so.
		int iIndex_YAxis_Proposed = vec2Index.y;
		for (int i = iIndex_YAxis_OLD; i >= iIndex_YAxis_Proposed; i--)
		{
			// Change the player's index to the current i value
			vec2Index.y = i;
			// If the new position is not feasible, then revert to old position
			if (CheckPosition(DOWN) == false)
			{
				// Revert to the previous position
				if (i != iIndex_YAxis_OLD)
					vec2Index.y = i + 1;
				// Set the Physics to idle status
				cPhysics2D.SetStatus(CPhysics2D::STATUS::IDLE);
				iJumpCount = 0;
				vec2NumMicroSteps.y = 0;
				break;
			}
		}
	}
}

/**
 @brief Let player interact with the map. You can add collectibles such as powerups and health here.
 */
void CPlayer2D::InteractWithMap(void)
{
	switch (cMap2D->GetMapInfo(vec2Index.y, vec2Index.x))
	{
	case 2:
		// Erase the Potion from this position
		cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 0);
		// Increase the potion by 1
		cInventoryItem = cInventoryManager->GetItem("Health");
		cInventoryItem->Add(20);
		//// Play a bell sound
		//cSoundController->PlaySoundByID(1);
		break;
	case 3:
		// Erase the potion from this position
		cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 0);
		// Increase the potion by 1
		cInventoryItem = cInventoryManager->GetItem("SpeedPotion");
		cInventoryItem->Add(1);
		// Play a bell sound
		cSoundController->PlaySoundByID(1);
		speed = true;
		
		break;
	case 4:
		// Erase the potion from this position
		cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 0);
		// Increase the potion by 1
		cInventoryItem = cInventoryManager->GetItem("StrengthPotion");
		cInventoryItem->Add(1);
		strength = true;
		// Play a aroller->PlaySoundByID(1);
		break;
	case 5:
		// Erase the potion from this position
		cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 0);
		// Increase the potion by 1
		cInventoryItem = cInventoryManager->GetItem("JumpPotion");
		cInventoryItem->Add(1);
		highjump = true;
		// Play a bell sound
		cSoundController->PlaySoundByID(1);
		break;
	case 30:
		// Erase the potion from this position
		cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 0);
		// Increase the potion by 1
		cInventoryItem = cInventoryManager->GetItem("sword");
		cInventoryItem->Add(1);
		swordequip = true;
		std::cout << "sword equiped\n";
		// Play a bell sound
		cSoundController->PlaySoundByID(1);
		break;
	case 31:
		// Erase the potion from this position
		cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 0);
		// Increase the potion by 1
		cInventoryItem = cInventoryManager->GetItem("spear");
		cInventoryItem->Add(1);
		spearequip = true;
		// Play a bell sound
		cSoundController->PlaySoundByID(1);
		break;
	case 32:
		// Erase the potion from this position
		cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 0);
		// Increase the potion by 1
		cInventoryItem = cInventoryManager->GetItem("bow");
		cInventoryItem->Add(1);
		// Play a bell sound
		cSoundController->PlaySoundByID(1);
		break;
	case 33:
		// Erase the potion from this position
		cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 0);
		// Increase the potion by 1
		cInventoryItem = cInventoryManager->GetItem("axe");
		cInventoryItem->Add(1);
		axeequip = true;
		// Play a bell sound
		cSoundController->PlaySoundByID(1);
		break;
	case 34:
		// Erase the potion from this position
		cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 0);
		// Increase the potion by 1
		cInventoryItem = cInventoryManager->GetItem("shovel");
		cInventoryItem->Add(1);
		shovelequip = true;
		// Play a bell sound
		cSoundController->PlaySoundByID(1);
		break;
	case 10:
		// Increase the lives by 1
		cInventoryItem = cInventoryManager->GetItem("Lives");
		cInventoryItem->Add(1);
		// Erase the life from this position
		cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 0);
		break;
	case 20:
		// Decrease the health by 1
		cInventoryItem = cInventoryManager->GetItem("Health");
		cInventoryItem->Remove(1);
		break;
	case 41:
		// Erase the potion from this position
		cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 0);
		// Increase the potion by 1
		cInventoryItem = cInventoryManager->GetItem("helmet");
		cInventoryItem->Add(1);
		std::cout << "got armour" << endl;
		helmetequip = true;
		// Play a bell sound
		cSoundController->PlaySoundByID(1);
		if (helmetequip = true)
		{
			defense = 20;
			std::cout << "u're protected!" << endl;
			
		}
		break;
	case 42:
		// Erase the potion from this position
		cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 0);
		// Increase the potion by 1
		cInventoryItem = cInventoryManager->GetItem("chestplate");
		cInventoryItem->Add(1);
		chestplateequip = true;
		// Play a bell sound
		cSoundController->PlaySoundByID(1);
		break;
	case 43:
		// Erase the potion from this position
		cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 0);
		// Increase the potion by 1
		cInventoryItem = cInventoryManager->GetItem("leggings");
		cInventoryItem->Add(1);
		leggingsequip = true;
		// Play a bell sound
		cSoundController->PlaySoundByID(1);
		break;
	case 44:
		// Erase the potion from this position
		cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 0);
		// Increase the potion by 1
		cInventoryItem = cInventoryManager->GetItem("boots");
		cInventoryItem->Add(1);
		bootsequip = true;
		// Play a bell sound
		cSoundController->PlaySoundByID(1);
		break;
	case 99:
		// Level has been completed
		CGameManager::GetInstance()->bLevelCompleted = true;
		break;
	case 98:
		// Level has been completed
		CGameManager::GetInstance()->bPlayerLost = true;
		break;
	default:
		break;
	}
}

/**
 @brief Update the health and lives.
 */
void CPlayer2D::UpdateHealthLives(void)
{
	// Update health and lives
	cInventoryItem = cInventoryManager->GetItem("Health");
	// Check if a life is lost
	if (cInventoryItem->GetCount() <= 0)
	{
		// But we reduce the lives by 1.
		cInventoryItem = cInventoryManager->GetItem("Lives");
		cInventoryItem->Remove(1);
		// Check if there is no lives left...
		if (cInventoryItem->GetCount() < 0)
		{
			// Player loses the game
			CGameManager::GetInstance()->bPlayerLost = true;
		}
	}
}

//void CPlayer2D::UpdateDefense(void)
//{
//
//	if (helmetequip = true)
//	{	
//		defense = 20;
//		cout << "u're protected!" << endl;
//		damageOnPlayer = returnPlayerHealth();
//		cInventoryItem = damageOnPlayer->GetItem("Health");
//		cInventoryItem->Remove(30 - defense);
//		cout << "damage reduced" << endl;
//	}
//	if (chestplateequip = true)
//	{
//		defense = 40;
//		std::cout << "u're protected!" << endl;
//	/*	damageOnPlayer = returnPlayerHealth();
//		cInventoryItem = damageOnPlayer->GetItem("Health");
//		cInventoryItem->Remove(30-defense);
//		*/
//	}
//	if (leggingsequip = true)
//	{
//		defense = 30;
//		std::cout << "u're protected!" << endl;
//		/*damageOnPlayer = returnPlayerHealth();
//		cInventoryItem = damageOnPlayer->GetItem("Health");
//		cInventoryItem->Remove(30-defense);
//		*/
//	}
//	if (bootsequip = true)
//	{
//		defense = 10;
//		std::cout << "u're protected!" << endl;
//		/*damageOnPlayer = returnPlayerHealth();
//		cInventoryItem = damageOnPlayer->GetItem("Health");
//		cInventoryItem->Remove(30-defense);*/
//	
//	}
//}
/*
	!CODE CHANGES START!
	code here should handle the breaking and placing of blocks
*/
void CPlayer2D::BuildBlocks() {
	if (cKeyboardController->IsKeyDown(GLFW_KEY_L)) {
		if (direction == 1) {
			switch (cMap2D->GetMapInfo(vec2Index.y, vec2Index.x - 1))
			{
			case 0:
				cMap2D->SetMapInfo(vec2Index.y, vec2Index.x - 1, 100);
				break;
			}
		}
		else if (direction == 2) {
			switch (cMap2D->GetMapInfo(vec2Index.y, vec2Index.x + 1))
			{
			case 0:
				cMap2D->SetMapInfo(vec2Index.y, vec2Index.x + 1, 100);
				break;
			}
		}
	}
	if (cKeyboardController->IsKeyDown(GLFW_KEY_J)) {
		if (direction == 1) {
			switch (cMap2D->GetMapInfo(vec2Index.y, vec2Index.x + 1))
			{
			case 0:
				cMap2D->SetMapInfo(vec2Index.y, vec2Index.x + 1, 100);
				break;
			}
		}
		else if (direction == 2) {
			switch (cMap2D->GetMapInfo(vec2Index.y, vec2Index.x - 1))
			{
			case 0:
				cMap2D->SetMapInfo(vec2Index.y, vec2Index.x - 1, 100);
				break;
			}
		}
	}
	if (cKeyboardController->IsKeyDown(GLFW_KEY_I)) {
		switch (cMap2D->GetMapInfo(vec2Index.y + 1, vec2Index.x))
		{
		case 0:
			cMap2D->SetMapInfo(vec2Index.y + 1, vec2Index.x, 100);
			break;
		}
	}
	if (cKeyboardController->IsKeyDown(GLFW_KEY_K)) {
		switch (cMap2D->GetMapInfo(vec2Index.y - 1, vec2Index.x))
		{
		case 0:
			cMap2D->SetMapInfo(vec2Index.y - 1, vec2Index.x, 100);
			break;
			
		}
	}
}


CEntity2D* CPlayer2D::returnNearestEnemy(void)
{
	for (int i = 0; i != enemyList.size(); ++i)
	{
		if (cPhysics2D.CalculateDistance(vec2Index, enemyList.front()->vec2Index) < (cPhysics2D.CalculateDistance(vec2Index, enemyList[i + 1]->vec2Index)))
		{
			continue;
		}
		else
		{
			CEntity2D* temp = enemyList.front();
			enemyList.front() = enemyList[i + 1];
			enemyList[i + 1] = temp;
			
		}
	}
	 return enemyList.front();
	
}

void CPlayer2D::BreakBlocks(const double dElapsedTime) {
	breakinterval -= dElapsedTime;
	if (cKeyboardController->IsKeyDown(GLFW_KEY_O) && breakinterval <= 0.f) {

		if (direction == 1) {
			switch (cMap2D->GetMapInfo(vec2Index.y, vec2Index.x - 1))
			{

			case 100:
				cMap2D->SetMapInfo(vec2Index.y, vec2Index.x - 1, 102);
				break;
			case 102:
				cMap2D->SetMapInfo(vec2Index.y, vec2Index.x - 1, 103);
				break;
			case 103:
				cMap2D->SetMapInfo(vec2Index.y, vec2Index.x - 1, 0);
				break;
			}
		}
		else if (direction == 2) {
			switch (cMap2D->GetMapInfo(vec2Index.y, vec2Index.x + 1))
			{
			case 100:
				cMap2D->SetMapInfo(vec2Index.y, vec2Index.x + 1, 102);
				break;
			case 102:
				cMap2D->SetMapInfo(vec2Index.y, vec2Index.x + 1, 103);
				break;
			case 103:
				cMap2D->SetMapInfo(vec2Index.y, vec2Index.x + 1, 0);
				break;
			}
		}
		if (shovelequip = true)
		{
			breakinterval = 0.1f;
		}
		else
		{
			breakinterval = 0.2f;
		}
		
	}
	if (cKeyboardController->IsKeyDown(GLFW_KEY_U) && breakinterval <= 0.f) {
		if (direction == 1) {
			switch (cMap2D->GetMapInfo(vec2Index.y - 1, vec2Index.x))
			{
			case 100:
				cMap2D->SetMapInfo(vec2Index.y - 1, vec2Index.x, 102);
				break;
			case 102:
				cMap2D->SetMapInfo(vec2Index.y - 1, vec2Index.x, 103);
				break;
			case 103:
				cMap2D->SetMapInfo(vec2Index.y - 1, vec2Index.x, 0);
				break;
			}
		}
		if (shovelequip = true)
		{
			breakinterval = 0.1f;
		}
		else
		{
			breakinterval = 0.2f;
		}
	}
	if (cKeyboardController->IsKeyDown(GLFW_KEY_P) && breakinterval <= 0.f) {

		switch (cMap2D->GetMapInfo(vec2Index.y + 1, vec2Index.x))
		{
		case 100:
			cMap2D->SetMapInfo(vec2Index.y + 1, vec2Index.x, 102);
			break;
		case 102:
			cMap2D->SetMapInfo(vec2Index.y + 1, vec2Index.x, 103);
			break;
		case 103:
			cMap2D->SetMapInfo(vec2Index.y + 1, vec2Index.x, 0);
			break;
		}
		if (shovelequip = true)
		{
			breakinterval = 0.1f;
		}
		else
		{
			breakinterval = 0.2f;
		}
	}
}
/*
	!CODE CHANGES END!
*/     