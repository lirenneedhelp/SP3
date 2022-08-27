/**
 CBowProjectile
 @brief A class which represents the enemy object
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "BowProjectile.h"

#include <iostream>
using namespace std;

// Include Shader Manager
#include "RenderControl\ShaderManager.h"
// Include Mesh Builder
#include "Primitives/MeshBuilder.h"

// Include GLEW
#include <GL/glew.h>

// Include ImageLoader
#include "System\ImageLoader.h"

// Include the Map2D as we will use it to check the player's movements and actions
#include "Map2D.h"
// Include math.h
#include <math.h>

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CBowProjectile::CBowProjectile(void)
	: bIsActive(false)
	, cMap2D(NULL)
	, cSettings(NULL)
	, cPlayer2D(NULL)
	, quadMesh(NULL)
	, arrowAnimation(NULL)
{
	transform = glm::mat4(1.0f);	// make sure to initialize matrix to identity matrix first

	// Initialise vecIndex
	vec2Index = glm::i32vec2(0);

	// Initialise vecNumMicroSteps
	i32vec2NumMicroSteps = glm::i32vec2(0);

	// Initialise vec2UVCoordinate
	vec2UVCoordinate = glm::vec2(0.0f);

	i32vec2Destination = glm::i32vec2(0, 0);	// Initialise the iDestination
	i32vec2Direction = glm::i32vec2(0, 0);		// Initialise the iDirection
}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CBowProjectile::~CBowProjectile(void)
{
	// Delete the quadMesh
	if (quadMesh)
	{
		delete quadMesh;
		quadMesh = NULL;
	}

	cPlayer2D = NULL;

	// We won't delete this since it was created elsewhere
	cMap2D = NULL;

	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

/**
  @brief Initialise this instance without reading from the map (won't be necessary)
  */
bool CBowProjectile::Init(void)
{
	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();

	// Get the handler to the CMap2D instance
	cMap2D = CMap2D::GetInstance();

	// By default, microsteps should be zero
	i32vec2NumMicroSteps = glm::i32vec2(0, 0);
	// By default, microsteps should be zero
	i32vec2NumMicroSteps = glm::i32vec2(0, 0);

	//i32vec2Direction = glm::i32vec2(0, 0);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//CS: Create the Quad Mesh using the mesh builder
	quadMesh = CMeshBuilder::GenerateQuad(glm::vec4(1, 1, 1, 1), cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);

	// Load the bullet texture
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/arrow.png", true);
	if (iTextureID == 0)
	{
		cout << "Unable to load Image/arrow.png" << endl;
		return false;
	}

	arrowAnimation = CMeshBuilder::GenerateSpriteAnimation(2, 4, cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
	arrowAnimation->AddAnimation("Left", 0, 3);
	arrowAnimation->AddAnimation("Right", 4, 7);


	//CS: Init the color to white
	runtimeColour = glm::vec4(1.0, 1.0, 1.0, 1.0);

	// Set the Physics to fall status by default
	cPhysics2D.Init();
	cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);

	arrowStorage = CScene2D::GetInstance()->getLiveArrowVector();

	arrowDamage = 20;

	amountOfCharge = cPlayer2D->returnCharge();

	maxDistance = amountOfCharge * 5.0f;

	if (i32vec2Direction.x < 0)
	{
		arrowDestination = vec2Index.x - maxDistance;
	}
	else
	{
		arrowDestination = vec2Index.x + maxDistance;
	}

	hitEnemy = false;

	// If this class is initialised properly, then set the bIsActive to true
	bIsActive = true;

	return true;
}

/**
 @brief Update this instance
 */
void CBowProjectile::Update(const double dElapsedTime)
{
	if (!bIsActive)
		return;
	
	UpdatePosition();
	if (arrowStorage.size() != 0)
	{
		CheckForInteraction();
	}
	arrowAnimation->Update(dElapsedTime);
	

	// Update the UV Coordinates
	vec2UVCoordinate.x = cSettings->ConvertIndexToUVSpace(cSettings->x, vec2Index.x, false, i32vec2NumMicroSteps.x*cSettings->MICRO_STEP_XAXIS);
	vec2UVCoordinate.y = cSettings->ConvertIndexToUVSpace(cSettings->y, vec2Index.y, false, i32vec2NumMicroSteps.y*cSettings->MICRO_STEP_YAXIS);
}

void CBowProjectile::SetPlayer2D(CPlayer2D* cPlayer2D)
{
	this->cPlayer2D = cPlayer2D;

	// Update the enemy's direction
	//UpdateDirection();
}

void CBowProjectile::setBulletVector(vector<CEntity2D*>& newBulletVector)
{
	arrowStorage = newBulletVector;
}

vector<CEntity2D*>& CBowProjectile::getBulletVector(void)
{
	// TODO: insert return statement here
	return arrowStorage;
}

void CBowProjectile::setMaxDistance(float charge)
{
	maxDistance *= charge;
}

/**
 @brief Set up the OpenGL display environment before rendering
 */
void CBowProjectile::PreRender(void)
{
	if (!bIsActive)
		return;

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);

	// Activate blending mode
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Activate the shader
	CShaderManager::GetInstance()->Use(sShaderName);
}

/**
 @brief Render this instance
 */
void CBowProjectile::Render(void)
{
	if (!bIsActive)
		return;

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

	// Get the texture to be rendered
	glBindTexture(GL_TEXTURE_2D, iTextureID);

	// Render the tile
	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(VAO);
	arrowAnimation->Render();
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);

}

/**
 @brief PostRender Set up the OpenGL display environment after rendering.
 */
void CBowProjectile::PostRender(void)
{
	if (!bIsActive)
		return;

	// Disable blending
	glDisable(GL_BLEND);
}

/**
@brief Set the indices of the enemy2D
@param iIndex_XAxis A const int variable which stores the index in the x-axis
@param iIndex_YAxis A const int variable which stores the index in the y-axis
*/
void CBowProjectile::Seti32vec2Index(const int iIndex_XAxis, const int iIndex_YAxis)
{
	this->vec2Index.x = iIndex_XAxis;
	this->vec2Index.y = iIndex_YAxis;
}

void CBowProjectile::seti32vec2Direction(const int direction)
{
	this->i32vec2Direction.x = direction;
	this->i32vec2Direction.y = 0;
}

/**
@brief Set the number of microsteps of the enemy2D
@param iNumMicroSteps_XAxis A const int variable storing the current microsteps in the X-axis
@param iNumMicroSteps_YAxis A const int variable storing the current microsteps in the Y-axis
*/
void CBowProjectile::Seti32vec2NumMicroSteps(const int iNumMicroSteps_XAxis, const int iNumMicroSteps_YAxis)
{
	this->i32vec2NumMicroSteps.x = iNumMicroSteps_XAxis;
	this->i32vec2NumMicroSteps.y = iNumMicroSteps_YAxis;
}



/**
 @brief Constraint the enemy2D's position within a boundary
 @param eDirection A DIRECTION enumerated data type which indicates the direction to check
 */
void CBowProjectile::Constraint(DIRECTION eDirection)
{
	if (eDirection == LEFT)
	{
		if (vec2Index.x < 0)
		{
			vec2Index.x = 0;
			i32vec2NumMicroSteps.x = 0;
		}
	}
	else if (eDirection == RIGHT)
	{
		if (vec2Index.x >= (int)cSettings->NUM_TILES_XAXIS - 1)
		{
			vec2Index.x = ((int)cSettings->NUM_TILES_XAXIS) - 1;
			i32vec2NumMicroSteps.x = 0;
		}
	}
	else if (eDirection == UP)
	{
		if (vec2Index.y >= (int)cSettings->NUM_TILES_YAXIS - 1)
		{
			vec2Index.y = ((int)cSettings->NUM_TILES_YAXIS) - 1;
			i32vec2NumMicroSteps.y = 0;
		}
	}
	else if (eDirection == DOWN)
	{
		if (vec2Index.y < 0)
		{
			vec2Index.y = 0;
			i32vec2NumMicroSteps.y = 0;
		}
	}
	else
	{
		cout << "CBowProjectile::Constraint: Unknown direction." << endl;
	}
}

/**
 @brief Check if a position is possible to move into
 @param eDirection A DIRECTION enumerated data type which indicates the direction to check
 */
bool CBowProjectile::CheckPosition(DIRECTION eDirection)
{
	if (eDirection == LEFT)
	{
		// If the new position is fully within a row, then check this row only
		if (i32vec2NumMicroSteps.y == 0)
		{
			// If the grid is not accessible, then return false
			if (cMap2D->GetMapInfo(vec2Index.y, vec2Index.x) >= 100)
			{
				return false;
			}
		}
		// If the new position is between 2 rows, then check both rows as well
		else if (i32vec2NumMicroSteps.y != 0)
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
			i32vec2NumMicroSteps.x = 0;
			return true;
		}

		// If the new position is fully within a row, then check this row only
		if (i32vec2NumMicroSteps.y == 0)
		{
			// If the grid is not accessible, then return false
			if (cMap2D->GetMapInfo(vec2Index.y, vec2Index.x + 1) >= 100)
			{
				return false;
			}
		}
		// If the new position is between 2 rows, then check both rows as well
		else if (i32vec2NumMicroSteps.y != 0)
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
			i32vec2NumMicroSteps.y = 0;
			return true;
		}

		// If the new position is fully within a column, then check this column only
		if (i32vec2NumMicroSteps.x == 0)
		{
			// If the grid is not accessible, then return false
			if (cMap2D->GetMapInfo(vec2Index.y + 1, vec2Index.x) >= 100)
			{
				return false;
			}
		}
		// If the new position is between 2 columns, then check both columns as well
		else if (i32vec2NumMicroSteps.x != 0)
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
		if (i32vec2NumMicroSteps.x == 0)
		{
			// If the grid is not accessible, then return false
			if (cMap2D->GetMapInfo(vec2Index.y, vec2Index.x) >= 100)
			{
				return false;
			}
		}
		// If the new position is between 2 columns, then check both columns as well
		else if (i32vec2NumMicroSteps.x != 0)
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
		cout << "CBowProjectile::CheckPosition: Unknown direction." << endl;
	}

	return true;
}

bool CBowProjectile::CheckPos(DIRECTION eDirection)
{
	if (eDirection == LEFT)
	{
		if ((cMap2D->GetMapInfo(vec2Index.y, vec2Index.x) >= 100) ||
			(cMap2D->GetMapInfo(vec2Index.y, vec2Index.x - 1) >= 100) ||
			vec2Index.x < 0)
		{
			if (i32vec2NumMicroSteps.x == 0)
				return true;
		}
		else
		{
			return false;
		}
	}
	else if (eDirection == RIGHT)
	{
		if ((cMap2D->GetMapInfo(vec2Index.y, vec2Index.x) >= 100) ||
			(cMap2D->GetMapInfo(vec2Index.y, vec2Index.x + 1) >= 100) ||
			vec2Index.x >= (int)cSettings->NUM_TILES_XAXIS - 1)
		{
			if (i32vec2NumMicroSteps.x == 0)
				return true;
		}
		else
		{
			return false;
		}
	}
	
	
	return false;
}

// Check if the enemy2D is in mid-air
bool CBowProjectile::IsMidAir(void)
{
	// if the player is at the bottom row, then he is not in mid-air for sure
	if (vec2Index.y == 0)
		return false;

	// Check if the tile below the player's current position is empty
	if ((i32vec2NumMicroSteps.x == 0) &&
		(cMap2D->GetMapInfo(vec2Index.y - 1, vec2Index.x) == 0))
	{
		return true;
	}

	return false;
}


/**
 @brief Let enemy2D interact with the player.
 */
bool CBowProjectile::InteractWithEnemies(void)
{
	vector<CEntity2D*>enemyList = CScene2D::GetInstance()->returnEnemyVector();
	for (int enemyIndex = 0; enemyIndex != enemyList.size(); ++enemyIndex)
	{
		// Check if the enemy2D is within 1.5 indices of the player2D
		if (((vec2Index.x >= enemyList[enemyIndex]->vec2Index.x - 0.5) &&
			(vec2Index.x <= enemyList[enemyIndex]->vec2Index.x + 0.5))
			&&
			(vec2Index.y == enemyList[enemyIndex]->vec2Index.y))
		{
			cout << "HEADSHOT!\n";
			hitEnemy = true;
			enemyList[enemyIndex]->health -= static_cast<int>(arrowDamage * amountOfCharge);
			std::cout << enemyList[enemyIndex]->health << endl;
			if (enemyList[enemyIndex]->health <= 0)
			{
				enemyList[enemyIndex]->~CEntity2D();
				enemyList.erase(enemyList.begin() + enemyIndex);
				CScene2D::GetInstance()->setNewEnemyVector(enemyList);
				cPlayer2D->addPlayerKills(1);
			}
			// Since the player has been caught, then reset the FSM
			return true;
		}
	}
	
	return false;
}


/**
 @brief Flip horizontal direction. For patrol use only
 */
void CBowProjectile::FlipHorizontalDirection(void)
{
	i32vec2Direction.x *= -1;
}

/**
@brief Update position.
*/
void CBowProjectile::UpdatePosition(void)
{
	// Store the old position
	i32vec2OldIndex = vec2Index;

	// if the player is to the left or right of the enemy2D, then jump to attack
	if (i32vec2Direction.x < 0)
	{
		// Move left
		const int iOldIndex = vec2Index.x;
		if (vec2Index.x >= 0)
		{
			i32vec2NumMicroSteps.x -= (0.5f * amountOfCharge);
			if (i32vec2NumMicroSteps.x < 0)
			{
				i32vec2NumMicroSteps.x = ((int)cSettings->NUM_STEPS_PER_TILE_XAXIS) - 1;
				vec2Index.x--;
			}
		}

		// Constraint the enemy2D's position within the screen boundary
		Constraint(LEFT);

		// Find a feasible position for the enemy2D's current position
		if (CheckPosition(LEFT) == false)
		{
			vec2Index = i32vec2OldIndex;
			i32vec2NumMicroSteps.x = 0;
		}

		// Check if enemy2D is in mid-air, such as walking off a platform
		if (IsMidAir() == true)
		{
			cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
		}
		arrowAnimation->PlayAnimation("Left", -1, 5);
		// Interact with the Player
		InteractWithEnemies();
	}
	else if (i32vec2Direction.x > 0)
	{
		// Move right
		const int iOldIndex = vec2Index.x;
		if (vec2Index.x < (int)cSettings->NUM_TILES_XAXIS)
		{
			i32vec2NumMicroSteps.x += (0.5f * amountOfCharge);

			if (i32vec2NumMicroSteps.x >= cSettings->NUM_STEPS_PER_TILE_XAXIS)
			{
				i32vec2NumMicroSteps.x = 0;
				vec2Index.x++;
			}
		}

		// Constraint the enemy2D's position within the screen boundary
		Constraint(RIGHT);

		// Find a feasible position for the enemy2D's current position
		if (CheckPosition(RIGHT) == false)
		{
			//vec2Index = i32vec2OldIndex;
			i32vec2NumMicroSteps.x = 0;
		}

		// Check if enemy2D is in mid-air, such as walking off a platform
		if (IsMidAir() == true)
		{
			cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
		}

		// Interact with the Player
		InteractWithEnemies();
		arrowAnimation->PlayAnimation("Right", -1, 5);

	}

	// if the player is above the enemy2D, then jump to attack
	if (i32vec2Direction.y > 0)
	{
		if (cPhysics2D.GetStatus() == CPhysics2D::STATUS::IDLE)
		{
			cPhysics2D.SetStatus(CPhysics2D::STATUS::JUMP);
			cPhysics2D.SetInitialVelocity(glm::vec2(0.0f, 3.5f));
		}
	}

}

void CBowProjectile::CheckForInteraction(void)
{
	for (int i = 0; i < arrowStorage.size(); i++)
	{
		if (i32vec2Direction.x > 0)
		{
			if (CheckPos(RIGHT))
			{
				arrowStorage[i]->~CEntity2D();
				vector <CEntity2D*> temp = CScene2D::GetInstance()->getLiveArrowVector();
				temp.erase(temp.begin() + i);
				CScene2D::GetInstance()->setLiveArrowVector(temp);
				arrowStorage = temp;
				//cout << arrowStorage.size() << endl;
				break;
			}
			else if (vec2Index.x >= arrowDestination)
			{
				arrowStorage[i]->~CEntity2D();
				vector <CEntity2D*> temp = CScene2D::GetInstance()->getLiveArrowVector();
				temp.erase(temp.begin() + i);
				CScene2D::GetInstance()->setLiveArrowVector(temp);
				arrowStorage = temp;
				//cout << arrowStorage.size() << endl;
				break;
			}
				    
		}
		else if (i32vec2Direction.x < 0)
		{
			if (CheckPos(LEFT))
			{			
				arrowStorage[i]->~CEntity2D();
				vector <CEntity2D*> temp = CScene2D::GetInstance()->getLiveArrowVector();
				temp.erase(temp.begin() + i);
				CScene2D::GetInstance()->setLiveArrowVector(temp);
				arrowStorage = temp;
				//cout << arrowStorage.size() << endl;
				break;	
			}
			else if (vec2Index.x <= arrowDestination)
			{
				arrowStorage[i]->~CEntity2D();
				vector <CEntity2D*> temp = CScene2D::GetInstance()->getLiveArrowVector();
				temp.erase(temp.begin() + i);
				CScene2D::GetInstance()->setLiveArrowVector(temp);
				arrowStorage = temp;
				//cout << arrowStorage.size() << endl;
				break;
			}
			
		}
		
		if (hitEnemy)
		{
			arrowStorage[i]->~CEntity2D();
			vector <CEntity2D*> temp = CScene2D::GetInstance()->getLiveArrowVector();
			temp.erase(temp.begin() + i);
			CScene2D::GetInstance()->setLiveArrowVector(temp);
			arrowStorage = temp;
			//cout << arrowStorage.size() << endl;
			break;
		}

	}
}
