/**
 CGlutton
 @brief A class which represents the enemy object
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "Glutton.h"

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
CGlutton::CGlutton(void)
	: bIsActive(false)
	, cMap2D(NULL)
	, cSettings(NULL)
	, cPlayer2D(NULL)
	, sCurrentFSM(FSM::IDLE)
	, iFSMCounter(0)
	, quadMesh(NULL)
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
CGlutton::~CGlutton(void)
{
	// Delete the quadMesh
	if (quadMesh)
	{
		delete quadMesh;
		quadMesh = NULL;
	}

	// We won't delete this since it was created elsewhere
	cPlayer2D = NULL;

	// We won't delete this since it was created elsewhere
	cMap2D = NULL;

	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

/**
  @brief Initialise this instance
  */
bool CGlutton::Init(void)
{
	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();

	// Get the handler to the CMap2D instance
	cMap2D = CMap2D::GetInstance();
	// Find the indices for the player in arrMapInfo, and assign it to cPlayer2D
	unsigned int uiRow = -1;
	unsigned int uiCol = -1;
	if (cMap2D->FindValue(302, uiRow, uiCol) == false)
		return false;	// Unable to find the start position of the player, so quit this game

	// Erase the value of the player in the arrMapInfo
	cMap2D->SetMapInfo(uiRow, uiCol, 0);

	// Set the start position of the Player to iRow and iCol
	vec2Index = glm::i32vec2(uiCol, uiRow);

	reachedOtherside = false;
	// By default, microsteps should be zero
	i32vec2NumMicroSteps = glm::i32vec2(0, 0);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//CS: Create the Quad Mesh using the mesh builder
	quadMesh = CMeshBuilder::GenerateQuad(glm::vec4(1, 1, 1, 1), cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);

	// Load the enemy2D texture
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/glutton.png", true);
	if (iTextureID == 0)
	{
		cout << "Unable to load Image/glutton.png" << endl;
		return false;
	}

	gluttonAnimatedSprites = CMeshBuilder::GenerateSpriteAnimation(7, 3, cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
	gluttonAnimatedSprites->AddAnimation("Left", 0, 2);
	gluttonAnimatedSprites->AddAnimation("Right", 3, 5);
	gluttonAnimatedSprites->AddAnimation("shootLeft", 6, 8);
	gluttonAnimatedSprites->AddAnimation("shootRight", 9, 11);
	gluttonAnimatedSprites->AddAnimation("attackLeft", 12, 14);
	gluttonAnimatedSprites->AddAnimation("attackRight", 15, 17);
	gluttonAnimatedSprites->AddAnimation("Idle", 18, 20);

	gluttonAnimatedSprites->PlayAnimation("Idle", -1, 5);

	//CS: Init the color to white
	runtimeColour = glm::vec4(1.0, 1.0, 1.0, 1.0);

	// Set the Physics to fall status by default
	cPhysics2D.Init();
	cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);

	shotInterval = 1.5f;

	// If this class is initialised properly, then set the bIsActive to true
	bIsActive = true;

	return true;
}

bool CGlutton::Init2(void)
{
	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();

	// Get the handler to the CMap2D instance
	cMap2D = CMap2D::GetInstance();

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//CS: Create the Quad Mesh using the mesh builder
	quadMesh = CMeshBuilder::GenerateQuad(glm::vec4(1, 1, 1, 1), cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);

	// Load the enemy2D texture
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/glutton.png", true);
	if (iTextureID == 0)
	{
		cout << "Unable to load Image/glutton.png" << endl;
		return false;
	}
	destination = glm::vec2(0, 0);
	gluttonAnimatedSprites = CMeshBuilder::GenerateSpriteAnimation(7, 3, cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
	gluttonAnimatedSprites->AddAnimation("Left", 0, 2);
	gluttonAnimatedSprites->AddAnimation("Right", 3, 5);
	gluttonAnimatedSprites->AddAnimation("shootLeft", 6, 8);
	gluttonAnimatedSprites->AddAnimation("shootRight", 9, 11);
	gluttonAnimatedSprites->AddAnimation("attackLeft", 12, 14);
	gluttonAnimatedSprites->AddAnimation("attackRight", 15, 17);
	gluttonAnimatedSprites->AddAnimation("Idle", 18, 20);

	gluttonAnimatedSprites->PlayAnimation("Idle", -1, 5);



	//CS: Init the color to white
	runtimeColour = glm::vec4(1.0, 1.0, 1.0, 1.0);

	// Set the Physics to fall status by default
	cPhysics2D.Init();
	cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);

	shotInterval = 1.0f;

	wallDist = 0.0f;

	// If this class is initialised properly, then set the bIsActive to true
	bIsActive = true;

	return true;
}

/**
 @brief Update this instance
 */
void CGlutton::Update(const double dElapsedTime)
{
	if (!bIsActive)
		return;

	switch (sCurrentFSM)
	{
	case IDLE:
		gluttonAnimatedSprites->AddAnimation("Idle", -1, 5);
		if (iFSMCounter > iMaxFSMCounter)
		{
			sCurrentFSM = PATROL;
			iFSMCounter = 0;
			//cout << "Switching to Patrol State" << endl;
		}
		iFSMCounter++;
		break;
	case PATROL:
		if (iFSMCounter > iMaxFSMCounter)
		{
			sCurrentFSM = IDLE;
			iFSMCounter = 0;
			//cout << "Switching to Idle State" << endl;
		}
		else if (cPhysics2D.CalculateDistance(vec2Index, cPlayer2D->vec2Index) < 25.0f)
		{
			sCurrentFSM = TRACE;
			iFSMCounter = 0;
			//cout << "Switching to TRACE State" << endl;
		}
		else if (checkForWall())
		{
			sCurrentFSM = JUMP_OVER_WALL;
			iFSMCounter = 0;
			//cout << "Switching to JUMP_OVER_WALL State" << endl;

		}
		
		else
		{
			// Patrol around
			// Update the Enemy2D's position for patrol
			UpdatePosition();
		}
		iFSMCounter++;
		break;
	case TRACE:
		if (cPhysics2D.CalculateDistance(vec2Index, cPlayer2D->vec2Index) >= 10.0f || vec2Index.y != cPlayer2D->vec2Index.y)
		{
			// Calculate a path to the player
			//cMap2D->PrintSelf();
			//cout << "StartPos: " << vec2Index.x << ", " << vec2Index.y << endl;
			//cout << "TargetPos: " << cPlayer2D->vec2Index.x << ", " 
			//		<< cPlayer2D->vec2Index.y << endl;
			auto path = cMap2D->PathFind(	vec2Index, 
											cPlayer2D->vec2Index, 
											heuristic::euclidean, 
											10);
			//cout << "=== Printing out the path ===" << endl;

			// Calculate new destination
			bool bFirstPosition = true;
			for (const auto& coord : path)
			{
				//std::cout << coord.x << "," << coord.y << "\n";
				if (bFirstPosition == true)
				{
					// Set a destination
					i32vec2Destination = coord;
					// Calculate the direction between enemy2D and this destination
					i32vec2Direction = i32vec2Destination - vec2Index;
					bFirstPosition = false;
				}
				else
				{
					if ((coord - i32vec2Destination) == i32vec2Direction)
					{
						// Set a destination
						i32vec2Destination = coord;
					}
					else
						break;
				}
			}

			//cout << "i32vec2Destination : " << i32vec2Destination.x 
			//		<< ", " << i32vec2Destination.y << endl;
			//cout << "i32vec2Direction : " << i32vec2Direction.x 
			//		<< ", " << i32vec2Direction.y << endl;
			//system("pause");

			// Attack
			// Update direction to move towards for attack
			//UpdateDirection();

			// Update the Enemy2D's position for attack
			UpdatePosition();
		}
		else
		{
			
			if (checkForWall())
			{
				sCurrentFSM = JUMP_OVER_WALL;
				iFSMCounter = 0;
				//cout << "TRACE TO JUMP_OVER_WALL\n";

			}
			else
			{
				sCurrentFSM = SHOOT;
				iFSMCounter = 0;
				//cout << "TRACE to SHOOT\n";
			}
						
			
			
			iFSMCounter++;
		}
		break;
	case SHOOT:
	{
		if (vec2Index.y == cPlayer2D->vec2Index.y)
		{
			shotInterval -= dElapsedTime;
			if (shotInterval <= 0)
			{
				// Checks whether enemy is facing left or right
				if (vec2Index.x - cPlayer2D->vec2Index.x > 0)  
				{
					gluttonAnimatedSprites->PlayAnimation("shootLeft", -1, 1);
				}
				else
				{
					gluttonAnimatedSprites->PlayAnimation("shootRight", -1, 1);

				}
				//cout << "Shot Bullet\n";
				CEnemyProjectile* cEnemyProjectile = new CEnemyProjectile(); // Create new Projectile
				cEnemyProjectile->SetShader("Shader2D_Colour");
				cEnemyProjectile->Seti32vec2Index(vec2Index.x, vec2Index.y);
				cEnemyProjectile->seti32vec2Direction(cPlayer2D->vec2Index.x, vec2Index.x);
				cEnemyProjectile->SetPlayer2D(cPlayer2D);
				CScene2D::GetInstance()->pushBullet(cEnemyProjectile);

				if (cEnemyProjectile->Init() == true)   // Initialise the Projectile
				{
					//cout << "SPAWNED\n";

				}

				shotInterval = 1.5f;
				
			}
			
		}
		else
		{		
			sCurrentFSM = IDLE;
			iFSMCounter = 0;
			//cout << "SHOOT TO IDLE\n";
			
		}
		
		
		
	}
	break;
	case JUMP_OVER_WALL:
	{
		if (vec2Index.y == cPlayer2D->vec2Index.y)
		{
			// Calculate a path to the player
			auto path = cMap2D->PathFind(vec2Index,
				destination,
				heuristic::euclidean,
				3);
			//cout << "=== Printing out the path ===" << endl;

			// Calculate new destination
			bool bFirstPosition = true;
			for (const auto& coord : path)
			{
				//std::cout << coord.x << "," << coord.y << "\n";
				if (bFirstPosition == true)
				{
					// Set a destination
					i32vec2Destination = coord;
					// Calculate the direction between enemy2D and this destination
					i32vec2Direction = i32vec2Destination - vec2Index;
					bFirstPosition = false;
				}
				else
				{
					if ((coord - i32vec2Destination) == i32vec2Direction)
					{
						// Set a destination
						i32vec2Destination = coord;
					}
					else
						break;
				}
			}
			UpdatePosition();

			/*	if (!checkForWall())
				{
					sCurrentFSM = SHOOT;
					iFSMCounter = 0;
				}
				else if (iFSMCounter > iMaxFSMCounter)
				{
					sCurrentFSM = PATROL;
					iFSMCounter = 0;
				}
				iFSMCounter++;*/
		}
		else if (vec2Index == destination)
		{
			sCurrentFSM = SHOOT;
			iFSMCounter = 0;
			destination = glm::vec2(0, 0);
		}
		else
		{
			sCurrentFSM = PATROL;
			iFSMCounter = 0;
		}
		
		

	}
	break;
	

	default:
		break;
	}

	// Update Jump or Fall
	UpdateJumpFall(dElapsedTime);
	gluttonAnimatedSprites->Update(dElapsedTime);

	// Update the UV Coordinates
	vec2UVCoordinate.x = cSettings->ConvertIndexToUVSpace(cSettings->x, vec2Index.x, false, i32vec2NumMicroSteps.x*cSettings->MICRO_STEP_XAXIS);
	vec2UVCoordinate.y = cSettings->ConvertIndexToUVSpace(cSettings->y, vec2Index.y, false, i32vec2NumMicroSteps.y*cSettings->MICRO_STEP_YAXIS);
}

/**
 @brief Set up the OpenGL display environment before rendering
 */
void CGlutton::PreRender(void)
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
void CGlutton::Render(void)
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
	gluttonAnimatedSprites->Render();
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);

}

/**
 @brief PostRender Set up the OpenGL display environment after rendering.
 */
void CGlutton::PostRender(void)
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
void CGlutton::Seti32vec2Index(const int iIndex_XAxis, const int iIndex_YAxis)
{
	this->vec2Index.x = iIndex_XAxis;
	this->vec2Index.y = iIndex_YAxis;
}

/**
@brief Set the number of microsteps of the enemy2D
@param iNumMicroSteps_XAxis A const int variable storing the current microsteps in the X-axis
@param iNumMicroSteps_YAxis A const int variable storing the current microsteps in the Y-axis
*/
void CGlutton::Seti32vec2NumMicroSteps(const int iNumMicroSteps_XAxis, const int iNumMicroSteps_YAxis)
{
	this->i32vec2NumMicroSteps.x = iNumMicroSteps_XAxis;
	this->i32vec2NumMicroSteps.y = iNumMicroSteps_YAxis;
}

/**
 @brief Set the handle to cPlayer to this class instance
 @param cPlayer2D A CPlayer2D* variable which contains the pointer to the CPlayer2D instance
 */
void CGlutton::SetPlayer2D(CPlayer2D* cPlayer2D)
{
	this->cPlayer2D = cPlayer2D;

	// Update the enemy's direction
	UpdateDirection();
}

bool CGlutton::checkForWall(void)
{
	for (float i = 0; i <= 10.f; ++i)
	{
		if (i32vec2Direction.x < 0)
		{
			
			if (vec2Index.x - i == cPlayer2D->vec2Index.x)
			{
				return false;
			}
			else if (cMap2D->GetMapInfo(vec2Index.y, vec2Index.x - i) >= 100)
			{
				wallDist = -(i + 2);
				destination = glm::vec2(vec2Index.x + wallDist, vec2Index.y);
				return true;
			}
			else
			{
				continue;
			}
		}
		else if (i32vec2Direction.x > 0)
		{
			if (vec2Index.x + i == cPlayer2D->vec2Index.x)
			{
				return false;
			}
			else if (cMap2D->GetMapInfo(vec2Index.y, vec2Index.x + i) >= 100)
			{
				wallDist = i + 2;
				destination = glm::vec2(vec2Index.x + wallDist, vec2Index.y);
				return true;
			}
			else
			{
				continue;
			}
		}
		
	}
	return false;
}

float CGlutton::getHP(void)
{
	return health;
}

void CGlutton::setHP(float newHealth)
{
	health = newHealth;
}

float CGlutton::getMaxHP(void)
{
	return maxHealth;
}

void CGlutton::setMaxHP(float maxHealth)
{
	this->maxHealth = maxHealth;
}


/**
 @brief Constraint the enemy2D's position within a boundary
 @param eDirection A DIRECTION enumerated data type which indicates the direction to check
 */
void CGlutton::Constraint(DIRECTION eDirection)
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
		cout << "CGlutton::Constraint: Unknown direction." << endl;
	}
}

/**
 @brief Check if a position is possible to move into
 @param eDirection A DIRECTION enumerated data type which indicates the direction to check
 */
bool CGlutton::CheckPosition(DIRECTION eDirection)
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
		cout << "CGlutton::CheckPosition: Unknown direction." << endl;
	}

	return true;
}

// Check if the enemy2D is in mid-air
bool CGlutton::IsMidAir(void)
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

// Update Jump or Fall
void CGlutton::UpdateJumpFall(const double dElapsedTime)
{
	if (cPhysics2D.GetStatus() == CPhysics2D::STATUS::JUMP)
	{
		// Update the elapsed time to the physics engine
		cPhysics2D.AddElapsedTime((float)dElapsedTime);
		// Call the physics engine update method to calculate the final velocity and displacement
		cPhysics2D.Update();
		// Get the displacement from the physics engine
		glm::vec2 v2Displacement = cPhysics2D.GetDisplacement();

		// Store the current vec2Index.y
		int iIndex_YAxis_OLD = vec2Index.y;

		int iDisplacement_MicroSteps = (int)(v2Displacement.y / cSettings->MICRO_STEP_YAXIS); //DIsplacement divide by distance for 1 microstep
		if (vec2Index.y < (int)cSettings->NUM_TILES_YAXIS)
		{
			i32vec2NumMicroSteps.y += iDisplacement_MicroSteps;
			if (i32vec2NumMicroSteps.y > cSettings->NUM_STEPS_PER_TILE_YAXIS)
			{
				i32vec2NumMicroSteps.y -= cSettings->NUM_STEPS_PER_TILE_YAXIS;
				if (i32vec2NumMicroSteps.y < 0)
					i32vec2NumMicroSteps.y = 0;
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
			if (CheckPosition(UP) == false)
			{
				// Align with the row
				i32vec2NumMicroSteps.y = 0;
				// Set the Physics to fall status
				cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
				break;
			}
		}

		// If the player is still jumping and the initial velocity has reached zero or below zero, 
		// then it has reach the peak of its jump
		if ((cPhysics2D.GetStatus() == CPhysics2D::STATUS::JUMP) && (cPhysics2D.GetInitialVelocity().y <= 0.0f))
		{
			// Set status to fall
			cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
		}
	}
	else if (cPhysics2D.GetStatus() == CPhysics2D::STATUS::FALL)
	{
		// Update the elapsed time to the physics engine
		cPhysics2D.AddElapsedTime((float)dElapsedTime);
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
			i32vec2NumMicroSteps.y -= fabs(iDisplacement_MicroSteps);
			if (i32vec2NumMicroSteps.y < 0)
			{
				i32vec2NumMicroSteps.y = ((int)cSettings->NUM_STEPS_PER_TILE_YAXIS) - 1;
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
				i32vec2NumMicroSteps.y = 0;
				break;
			}
		}
	}
}

/**
 @brief Let enemy2D interact with the player.
 */
bool CGlutton::InteractWithPlayer(void)
{
	glm::i32vec2 i32vec2PlayerPos = cPlayer2D->vec2Index;
	
	// Check if the enemy2D is within 1.5 indices of the player2D
	if (((vec2Index.x >= i32vec2PlayerPos.x - 0.5) && 
		(vec2Index.x <= i32vec2PlayerPos.x + 0.5))
		&& 
		((vec2Index.y >= i32vec2PlayerPos.y - 0.5) &&
		(vec2Index.y <= i32vec2PlayerPos.y + 0.5)))
	{
		// Since the player has been caught, then reset the FSM
	
		iFSMCounter = 0;
		return true;
	}
	return false;
}

/**
 @brief Update the enemy's direction.
 */
void CGlutton::UpdateDirection(void)
{
	// Set the destination to the player
	i32vec2Destination = cPlayer2D->vec2Index;

	// Calculate the direction between enemy2D and player2D
	i32vec2Direction = i32vec2Destination - vec2Index;

	// Calculate the distance between enemy2D and player2D
	float fDistance = cPhysics2D.CalculateDistance(vec2Index, i32vec2Destination);
	if (fDistance >= 0.01f)
	{
		// Calculate direction vector.
		// We need to round the numbers as it is easier to work with whole numbers for movements
		i32vec2Direction.x = (int)round(i32vec2Direction.x / fDistance);
		i32vec2Direction.y = (int)round(i32vec2Direction.y / fDistance);
	}
	else
	{
		// Since we are not going anywhere, set this to 0.
		i32vec2Direction = glm::i32vec2(0);
	}
}

/**
 @brief Flip horizontal direction. For patrol use only
 */
void CGlutton::FlipHorizontalDirection(void)
{
	i32vec2Direction.x *= -1;
}

/**
@brief Update position.
*/
void CGlutton::UpdatePosition(void)
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
			i32vec2NumMicroSteps.x--;
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
			FlipHorizontalDirection();
			vec2Index = i32vec2OldIndex;
			i32vec2NumMicroSteps.x = 0;
		}

		// Check if enemy2D is in mid-air, such as walking off a platform
		if (IsMidAir() == true)
		{
			cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
		}
		gluttonAnimatedSprites->PlayAnimation("Left", -1, 5);

		// Interact with the Player
		//InteractWithPlayer();
	}
	else if (i32vec2Direction.x > 0)
	{
		// Move right
		const int iOldIndex = vec2Index.x;
		if (vec2Index.x < (int)cSettings->NUM_TILES_XAXIS)
		{
			i32vec2NumMicroSteps.x++;

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
			FlipHorizontalDirection();
			//vec2Index = i32vec2OldIndex;
			i32vec2NumMicroSteps.x = 0;
		}

		// Check if enemy2D is in mid-air, such as walking off a platform
		if (IsMidAir() == true)
		{
			cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
		}

		// Interact with the Player
		//InteractWithPlayer();
		gluttonAnimatedSprites->PlayAnimation("Right", -1, 5);

	}

	// if the player is above the enemy2D, then jump to attack
	if (i32vec2Direction.y > 0 && cPlayer2D->vec2Index.y - vec2Index.y < 5.f)
	{
		if (cPhysics2D.GetStatus() == CPhysics2D::STATUS::IDLE)
		{
			cPhysics2D.SetStatus(CPhysics2D::STATUS::JUMP);
			cPhysics2D.SetInitialVelocity(glm::vec2(0.0f, 2.5f));
		}
	}
}
