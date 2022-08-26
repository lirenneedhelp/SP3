/**
 CScene2D
 @brief A class which manages the 2D game scene
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "Scene2D.h"
#include <iostream>
using namespace std;

// Include Shader Manager
#include "RenderControl\ShaderManager.h"

#include "System\filesystem.h"

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CScene2D::CScene2D(void)
	: cMap2D(NULL)
	, cPlayer2D(NULL)
	, cKeyboardController(NULL)
	, cGUI_Scene2D(NULL)
	, cGameManager(NULL)
	, cSoundController(NULL)
{
}

/**
 @brief Destructor
 */
CScene2D::~CScene2D(void)
{
	if (cSoundController)
	{
		// We won't delete this since it was created elsewhere
		cSoundController = NULL;
	}

	if (cGameManager)
	{
		cGameManager->Destroy();
		cGameManager = NULL;
	}

	if (cGUI_Scene2D)
	{
		cGUI_Scene2D->Destroy();
		cGUI_Scene2D = NULL;
	}

	if (cKeyboardController)
	{
		// We won't delete this since it was created elsewhere
		cKeyboardController = NULL;
	}

	// Destroy the enemies
	for (int i = 0; i < enemyVector.size(); i++)
	{
		delete enemyVector[i];
		enemyVector[i] = NULL;
	}
	enemyVector.clear();

	if (cPlayer2D)
	{
		cPlayer2D->Destroy();
		cPlayer2D = NULL;
	}

	if (cMap2D)
	{
		cMap2D->Destroy();
		cMap2D = NULL;
	}

	// Clear out all the shaders
	//CShaderManager::GetInstance()->Destroy();
}

/**
@brief Init Initialise this instance
*/ 
bool CScene2D::Init(void)
{
	// Include Shader Manager
	CShaderManager::GetInstance()->Use("Shader2D");
	//CShaderManager::GetInstance()->activeShader->setInt("texture1", 0);

	// Create and initialise the Map 2D
	cMap2D = CMap2D::GetInstance();
	// Set a shader to this class
	cMap2D->SetShader("Shader2D");
	// Initialise the instance
	if (cMap2D->Init(8, 24, 32) == false)
	{
		cout << "Failed to load CMap2D" << endl;
		return false;
	}
	// Load the map into an array
	if (cMap2D->LoadMap("Maps/DM2213_Map_Level_01.csv") == false)
	{
		// The loading of a map has failed. Return false
		return false;
	}
	// Load the map into an array
	if (cMap2D->LoadMap("Maps/DM2213_Map_Level_02.csv", 1) == false)
	{
		// The loading of a map has failed. Return false
		return false;
	}
	// Load the map into an array
	if (cMap2D->LoadMap("Maps/DM2213_Map_Level_cave.csv", 2) == false)
	{
		// The loading of a map has failed. Return false
		return false;
	}
	// Load the map into an array
	if (cMap2D->LoadMap("Maps/DM2213_Map_Level_forest.csv", 3) == false)
	{
		// The loading of a map has failed. Return false
		return false;
	}
	// Load the map into an array
	if (cMap2D->LoadMap("Maps/DM2213_Map_Level_hills.csv", 4) == false)
	{
		// The loading of a map has failed. Return false
		return false;
	}
	// Load the map into an array
	if (cMap2D->LoadMap("Maps/DM2213_Map_Level_islands.csv", 5) == false)
	{
		// The loading of a map has failed. Return false
		return false;
	}
	// Load the map into an array
	if (cMap2D->LoadMap("Maps/DM2213_Map_Level_plains.csv", 6) == false)
	{
		// The loading of a map has failed. Return false
		return false;
	}
	// Load the map into an array
	if (cMap2D->LoadMap("Maps/DM2213_Map_Level_valley.csv", 7) == false)
	{
		// The loading of a map has failed. Return false
		return false;
	}

	// Activate diagonal movement
	cMap2D->SetDiagonalMovement(false);

	// Load Scene2DColour into ShaderManager
	CShaderManager::GetInstance()->Use("Shader2D_Colour");
	//CShaderManager::GetInstance()->activeShader->setInt("texture1", 0);
	
	// Create and initialise the CPlayer2D
	cPlayer2D = CPlayer2D::GetInstance();
	// Pass shader to cPlayer2D
	cPlayer2D->SetShader("Shader2D_Colour");
	// Initialise the instance
	if (cPlayer2D->Init() == false)
	{
		cout << "Failed to load CPlayer2D" << endl;
		return false;
	}

	//Create and initialise the CEnemyProjectile
	liveBullets.clear();

	//Create and initialise the CBowProjectile
	liveArrows.clear();

	// Create and initialise the CEnemy2D
	enemyVector.clear();
	while (true)
	{
		CEnemy2D* cEnemy2D = new CEnemy2D();
		// Pass shader to cEnemy2D
		cEnemy2D->SetShader("Shader2D_Colour");
		// Initialise the instance
		if (cEnemy2D->Init() == true)
		{
			cEnemy2D->SetPlayer2D(cPlayer2D);
			enemyVector.push_back(cEnemy2D);
		}
		else
		{
			// Break out of this loop if the enemy has all been loaded
			break;
		}
	}
	while (true)
	{
		CWoodCrawler* cWoodCrawler = new CWoodCrawler();
		cWoodCrawler->SetShader("Shader2D_Colour");
		
		if (cWoodCrawler->Init() == true)
		{
			cWoodCrawler->SetPlayer2D(cPlayer2D);
			cWoodCrawler->setHP(10000);
			cWoodCrawler->setMaxHP(10000);
			enemyVector.push_back(cWoodCrawler);
		}
		else
		{
			delete cWoodCrawler;
			cWoodCrawler = NULL;
			// Break out of this loop if the enemy has all been loaded
			break;
		}


	}
	while (true)
	{
		CGlutton* cGlutton = new CGlutton();
		cGlutton->SetShader("Shader2D_Colour");

		if (cGlutton->Init() == true)
		{
			cGlutton->SetPlayer2D(cPlayer2D);
			cGlutton->setHP(60);
			cGlutton->setMaxHP(60);
			enemyVector.push_back(cGlutton);
		}
		else
		{
			delete cGlutton;
			cGlutton = NULL;
			// Break out of this loop if the enemy has all been loaded
			break;
		}


	}
	while (true)
	{
		CBloodDeer* cBloodDeer = new CBloodDeer();
		cBloodDeer->SetShader("Shader2D_Colour");

		if (cBloodDeer->Init() == true)
		{
			cBloodDeer->SetPlayer2D(cPlayer2D);
			cBloodDeer->setHP(300);
			cBloodDeer->setMaxHP(300);
			enemyVector.push_back(cBloodDeer);
		}
		else
		{
			delete cBloodDeer;
			cBloodDeer = NULL;
			// Break out of this loop if the enemy has all been loaded
			break;
		}
	}

	// Store the keyboard controller singleton instance here
	cKeyboardController = CKeyboardController::GetInstance();

	// Store the cGUI_Scene2D singleton instance here
	cGUI_Scene2D = CGUI_Scene2D::GetInstance();
	cGUI_Scene2D->Init();

	// Game Manager
	cGameManager = CGameManager::GetInstance();
	cGameManager->Init();

	bgColor = glm::vec3(0.0f, 0, 1.0f);

	day = dayToNight = true;

	dayCounter = 0.0f;


	enemySpawnRate = 15.f;

	enemySpawnTimeCounter = enemySpawnRate;
	
	// Load the sounds into CSoundController
	cSoundController = CSoundController::GetInstance();
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\creepy-night.ogg"), 1, true); //background sound for night 
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\Theme4.ogg"), 2, true); // background sound for day
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\jump.ogg"), 3, true);
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\item-equip.ogg"), 4, true); //sound effect for equipping an item
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\potionpickup.ogg"), 5, true); //sound effect for potions
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\hurt_dying.ogg"), 6, true); //sound effect for death
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\ough.ogg"), 7, true); //sound effect for getting hit
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\punch.ogg"), 8, true); //eound effect for punching
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\weaponattack.ogg"), 9, true); //sound effect for sword attack
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\wood-creak.ogg"), 10, true); //sound effect for chest
	cSoundController->LoadSound(FileSystem::getPath("Sounds\\bow.ogg"), 11, true); //sound effect for chest

	cSoundController->PlaySoundByID(2);


	return true;
}

/**
@brief Update Update this instance
*/
bool CScene2D::Update(const double dElapsedTime)
{
	// Call the cPlayer2D's update method before Map2D as we want to capture the inputs before map2D update
	cPlayer2D->Update(dElapsedTime);
	
	//enemyProjectile->Update(dElapsedTime);
	
	// Call all the cEnemy2D's update method before Map2D 
	// as we want to capture the updates before map2D update
	for (int i = 0; i < enemyVector.size(); i++)
	{
		enemyVector[i]->Update(dElapsedTime);
	}

	if (liveBullets.size() > 0)
	{
		for (int j = 0; j < liveBullets.size(); j++)
		{
			liveBullets[j]->Update(dElapsedTime);
		}
	}
	if (liveArrows.size() > 0)
	{
		for (int k = 0; k < liveArrows.size(); k++)
		{
			liveArrows[k]->Update(dElapsedTime);
		}
	}
	 
	// Call the Map2D's update method
	cMap2D->Update(dElapsedTime);

	// Get keyboard updates
	if (cKeyboardController->IsKeyReleased(GLFW_KEY_F6))
	{
		// Save the current game to a save file
		// Make sure the file is open
		try {
			if (cMap2D->SaveMap("Maps/DM2213_Map_Level_01_SAVEGAMEtest.csv") == false)
			{
				throw runtime_error("Unable to save the current game to a file");
			}
		}
		catch (runtime_error e)
		{
			cout << "Runtime error: " << e.what();
			return false;
		}
	}

	// Call the cGUI_Scene2D's update method
	cGUI_Scene2D->Update(dElapsedTime);

	// Check if the game should go to the next level
	if (cGameManager->bLevelCompleted == true)
	{
		cMap2D->SetCurrentLevel(cMap2D->GetCurrentLevel()+1);
		cPlayer2D->Reset();
		cGameManager->bLevelCompleted = false;
		enemySpawnRate -= cMap2D->GetCurrentLevel() - 1;

		// Destroy the enemies
		for (int i = 0; i < enemyVector.size(); i++)
		{
			delete enemyVector[i];
			enemyVector[i] = NULL;
		}
		enemyVector.clear();
	}

	// Check if the game has been won by the player
	if (cGameManager->bPlayerWon == true)
	{
		// End the game and switch to Win screen
	}
	// Check if the game should be ended
	else if (cGameManager->bPlayerLost == true)
	{
		cSoundController->PlaySoundByID(6);
		return false;
	}
	dayCounter += dElapsedTime;

	if (day == true)
	{
		//cSoundController->PlaySoundByID(2);
	}
	else
	{
		//cSoundController->PlaySoundByID(1);
	}

	if (!day)
	{

		enemySpawnTimeCounter -= dElapsedTime;
	}
	else
	{
		enemySpawnTimeCounter = enemySpawnRate;
	}

	if (enemySpawnTimeCounter <= 0)
	{ 
		while (true)
		{
			srand((unsigned)time(NULL));

			float randcol = rand() % (CSettings::GetInstance()->NUM_TILES_XAXIS - 1); // no of col
			float randrow = rand() % (CSettings::GetInstance()->NUM_TILES_YAXIS - 1); // no of rows
			//cout << randcol << " , " << randrow << endl;
			if (cMap2D->GetMapInfo(randrow, randcol) != 0)
			{
				continue;
			}
			else
			{
				float randEnemy = rand() % 2 + 1;
				cout << randEnemy << endl;
				if (randEnemy == 1)
				{
					CWoodCrawler* cWoodCrawler = new CWoodCrawler();
					cWoodCrawler->SetShader("Shader2D_Colour");
					cWoodCrawler->Seti32vec2Index(randcol, randrow);
					if (cWoodCrawler->Init2() == true)
					{
						cWoodCrawler->SetPlayer2D(cPlayer2D);
						cWoodCrawler->setHP(60);
						cWoodCrawler->setMaxHP(60);
						enemyVector.push_back(cWoodCrawler);
					}
				}
				else if (randEnemy == 2)
				{
					CGlutton* cGlutton = new CGlutton();
					cGlutton->SetShader("Shader2D_Colour");
					cGlutton->Seti32vec2Index(randcol, randrow);
					if (cGlutton->Init2() == true)
					{
						cGlutton->SetPlayer2D(cPlayer2D);
						cGlutton->setHP(60);
						cGlutton->setMaxHP(60);
						enemyVector.push_back(cGlutton);
					}
				}
				enemySpawnTimeCounter = enemySpawnRate;

				break;
			}
		}
	}

	return true;
}

/**
 @brief PreRender Set up the OpenGL display environment before rendering
 */
void CScene2D::PreRender(void)
{
	// Reset the OpenGL rendering environment
	glLoadIdentity();

	// Clear the screen and buffer
	glClearColor(bgColor.x,bgColor.y,bgColor.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Enable 2D texture rendering
	glEnable(GL_TEXTURE_2D);
}

/**
 @brief Render Render this instance
 */
void CScene2D::Render(void)
{
	// Call the Map2D's PreRender()
	cMap2D->PreRender();
	// Call the Map2D's Render()
	cMap2D->Render();
	// Call the Map2D's PostRender()
	cMap2D->PostRender();
	for (int i = 0; i < enemyVector.size(); i++)
	{
		// Call the CEnemy2D's PreRender()
		enemyVector[i]->PreRender();
		// Call the CEnemy2D's Render()
		enemyVector[i]->Render();
		// Call the CEnemy2D's PostRender()
		enemyVector[i]->PostRender();
	}

	if (liveBullets.size() > 0)
	{
		for (int i = 0; i < liveBullets.size(); i++)
		{
			// Call the CEnemy2D's PreRender()
			liveBullets[i]->PreRender();
			// Call the CEnemy2D's Render()
			liveBullets[i]->Render();
			// Call the CEnemy2D's PostRender()
			liveBullets[i]->PostRender();
		}
	}
	if (liveArrows.size() > 0)
	{
		for (int i = 0; i < liveArrows.size(); i++)
		{
			// Call the CEnemy2D's PreRender()
			liveArrows[i]->PreRender();
			// Call the CEnemy2D's Render()
			liveArrows[i]->Render();
			// Call the CEnemy2D's PostRender()
			liveArrows[i]->PostRender();
		}
	}

	// Call the CPlayer2D's PreRender()
	cPlayer2D->PreRender();
	// Call the CPlayer2D's Render()
	cPlayer2D->Render();
	// Call the CPlayer2D's PostRender()
	cPlayer2D->PostRender();

	// Call the cGUI_Scene2D's PreRender()
	cGUI_Scene2D->PreRender();
	// Call the cGUI_Scene2D's Render()
	cGUI_Scene2D->Render();
	// Call the cGUI_Scene2D's PostRender()
	cGUI_Scene2D->PostRender();
}

/**
 @brief PostRender Set up the OpenGL display environment after rendering.
 */
void CScene2D::PostRender(void)
{
}

bool CScene2D::getTime(void)
{
	return day;
}

void CScene2D::setTime(bool time)
{
	day = time;
}

bool CScene2D::checkTimeTransition(void)
{
	return dayToNight;
}

void CScene2D::setTimeTransition(bool transition)
{
	dayToNight = transition;
}

float CScene2D::getDuration(void)
{
	return dayCounter;
}

void CScene2D::resetDuration(void)
{
	dayCounter = 0.0f;
}

vector<CEntity2D*> CScene2D::returnEnemyVector(void)
{
	return enemyVector;
}

void CScene2D::setNewEnemyVector(vector<CEntity2D*> &newList)
{
	enemyVector = newList;
}

void CScene2D::pushArrow(CEntity2D* arrow)
{
	liveArrows.push_back(arrow);
}

void CScene2D::setLiveBulletVector(vector<CEntity2D*>& vectorOfBullets)
{
	liveBullets = vectorOfBullets;
}

void CScene2D::pushBullet(CEntity2D* bullet)
{
	liveBullets.push_back(bullet);
}

vector<CEntity2D*> CScene2D::getLiveBulletVector(void)
{
	return liveBullets;
}

void CScene2D::setLiveArrowVector(vector<CEntity2D*>& vectorOfArrows)
{
	liveArrows = vectorOfArrows;
}

vector<CEntity2D*> CScene2D::getLiveArrowVector(void)
{
	return liveArrows;
}
