/**
 CGUI_Scene2D
 @brief A class which manages the GUI for Scene2D
 By: Toh Da Jun
 Date: May 2021
 */
#include "GUI_Scene2D.h"
#include "Scene2D.h"
#include "../GameStateManagement/GameStateManager.h"

#include <iostream>
using namespace std;

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CGUI_Scene2D::CGUI_Scene2D(void)
	: cSettings(NULL)
	, m_fProgressBar(0.0f)
	, cInventoryManager(NULL)
	, cInventoryItem(NULL)
{
}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CGUI_Scene2D::~CGUI_Scene2D(void)
{
	if (cInventoryManager)
	{
		cInventoryManager->Destroy();
		cInventoryManager = NULL;
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// We won't delete this since it was created elsewhere
	cSettings = NULL;
}

/**
  @brief Initialise this instance
  */
bool CGUI_Scene2D::Init(void)
{
	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();

	// Store the CFPSCounter singleton instance here
	cFPSCounter = CFPSCounter::GetInstance();

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(CSettings::GetInstance()->pWindow, true);
	const char* glsl_version = "#version 330";
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Define the window flags
	window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoBackground;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoCollapse;

	//// Show the mouse pointer
	//glfwSetInputMode(CSettings::GetInstance()->pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	inventoryHotbar = 9;
	inventorySize = 24;
	// Initialise the cInventoryManager
	cInventoryManager = CInventoryManager::GetInstance();

	// Initialise the vector
	storePlayerItem.clear();

	// Initialise the enemyHealth vector
	enemyHealth.clear();


	CImageLoader* il = CImageLoader::GetInstance();
	emptyInventoryTextureID = il->LoadTextureGetID("Image\\GUI\\itemhotbar.png", false);

	emptyInventorySlot.fileName = "Image\\GUI\\itemhotbar.png";
	emptyInventorySlot.itemID = -1;
	emptyInventorySlot.active = false;
	emptyInventorySlot.noOfItems = 0;

	for (int i = 0; i < inventoryHotbar; ++i)
	{
		emptyInventorySlot.textureID = il->LoadTextureGetID("Image\\GUI\\itemhotbar.png", false);
		emptyInventorySlot.slotID = i;
		playerInventory.push_back(emptyInventorySlot);
	}
	cInventoryItem = cInventoryManager->Add("deerUI", "Image\\GUI\\blooddeerGUI.png", 0, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	cInventoryItem = cInventoryManager->Add("enemyUI", "Image\\GUI\\enemyGUI.png", 0, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);
	currentSlotID = -1;

	// Store the keyboard controller singleton instance here
	cKeyboardController = CKeyboardController::GetInstance();
	// Reset all keys since we are starting a new game
	cKeyboardController->Reset();

	return true;
}

/**
 @brief Update this instance
 */
void CGUI_Scene2D::Update(const double dElapsedTime)
{
//	cout << playerInventory.size() << " , " << storePlayerItem.size() << endl;
	// Calculate the relative scale to our default windows width
	const float relativeScale_x = cSettings->iWindowWidth / 800.0f;
	const float relativeScale_y = cSettings->iWindowHeight / 600.0f;

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// If the OpenGL window is minimised, then don't render the IMGUI widgets
	if ((relativeScale_x == 0.0f) || (relativeScale_y == 0.0f))
	{
		return;
	}

	// Create an invisible window which covers the entire OpenGL window
	ImGui::Begin("Invisible window", NULL, window_flags);
	ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
	ImGui::SetWindowSize(ImVec2((float)cSettings->iWindowWidth, (float)cSettings->iWindowHeight));
	ImGui::SetWindowFontScale(1.5f * relativeScale_y);

	// Display the FPS
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "FPS: %d", cFPSCounter->GetFrameRate());

	// Render the Health
	ImGuiWindowFlags healthWindowFlags = ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoScrollbar;
	ImGui::Begin("Health", NULL, healthWindowFlags);
	ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * 0.03f, cSettings->iWindowHeight * 0.03f));
	ImGui::SetWindowSize(ImVec2(100.0f * relativeScale_x, 25.0f * relativeScale_y));
	ImGui::SetWindowFontScale(1.5f * relativeScale_y);
	cInventoryItem = cInventoryManager->GetItem("Health");
	ImGui::Image((void*)(intptr_t)cInventoryItem->GetTextureID(),
		ImVec2(cInventoryItem->vec2Size.x * relativeScale_x, cInventoryItem->vec2Size.y * relativeScale_y),
		ImVec2(0, 1), ImVec2(1, 0));
	ImGui::SameLine();
	ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
	ImGui::ProgressBar(cInventoryItem->GetCount() /
		(float)cInventoryItem->GetMaxCount(), ImVec2(100.0f * relativeScale_x, 20.0f * relativeScale_y));
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::End();

	// Render the Enemy Health	
	enemyHealth = CScene2D::GetInstance()->returnEnemyVector();
	enemyHealth = CPlayer2D::GetInstance()->returnNearestEnemy(enemyHealth);

	if (enemyHealth.size() > 0)
	{
		ImGui::Begin("Enemy_Health", NULL, healthWindowFlags);


		ImGui::SetWindowPos(ImVec2(enemyHealth.front()->vec2Index.x * 25 * relativeScale_x - 10.f, cSettings->iWindowHeight - 50.0f - (enemyHealth.front()->vec2Index.y * 25 * relativeScale_y)));
		//cout << enemyHealth[i]->vec2Index.x * 25 * relativeScale_x << ", " << cSettings->iWindowHeight - (enemyHealth[i]->vec2Index.y * 25 * relativeScale_y)  << endl;

		ImGui::SetWindowSize(ImVec2(10.0f * relativeScale_x, 2.0f * relativeScale_y));
		ImGui::SetWindowFontScale(1.0f * relativeScale_y);

		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
		ImGui::ProgressBar(enemyHealth.front()->health /
			enemyHealth.front()->maxHealth, ImVec2(50.0f * relativeScale_x, 13.0f * relativeScale_y));
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		ImGui::End();
	}



	// Render the Lives
	ImGuiWindowFlags livesWindowFlags = ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoScrollbar;
	ImGui::Begin("Lives", NULL, livesWindowFlags);
	ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * 0.85f, cSettings->iWindowHeight * 0.03f));
	ImGui::SetWindowSize(ImVec2(100.0f * relativeScale_x, 25.0f * relativeScale_y));
	cInventoryItem = cInventoryManager->GetItem("Lives");
	ImGui::Image((void*)(intptr_t)cInventoryItem->GetTextureID(),
		ImVec2(cInventoryItem->vec2Size.x * relativeScale_x, cInventoryItem->vec2Size.y * relativeScale_y),
		ImVec2(0, 1), ImVec2(1, 0));
	ImGui::SameLine();
	ImGui::SetWindowFontScale(1.5f * relativeScale_y);
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d / %d",
		cInventoryItem->GetCount(), cInventoryItem->GetMaxCount());
	ImGui::End();

	ImGui::Begin("Level_Indicator", NULL, livesWindowFlags);
	ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * 0.4, cSettings->iWindowHeight * 0.03f));
	ImGui::SetWindowSize(ImVec2(100.0f * relativeScale_x, 25.0f * relativeScale_y));
	cInventoryItem = cInventoryManager->GetItem("Lives");
	ImGui::SetWindowFontScale(2.0f * relativeScale_y);
	ImGui::TextColored(ImVec4(1, 1, 0, 1), " Level %d",
		CMap2D::GetInstance()->GetCurrentLevel() + 1);
	ImGui::End();


	ImGui::Begin("Enemies_Left", NULL, livesWindowFlags);
	ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * 0.79f, cSettings->iWindowHeight * 0.2f));
	ImGui::SetWindowSize(ImVec2(100.0f * relativeScale_x, 25.0f * relativeScale_y));
	cInventoryItem = cInventoryManager->GetItem("enemyUI");
	ImGui::Image((void*)(intptr_t)cInventoryItem->GetTextureID(),
		ImVec2(cInventoryItem->vec2Size.x * relativeScale_x, cInventoryItem->vec2Size.y * relativeScale_y),
		ImVec2(0, 1), ImVec2(1, 0));
	ImGui::SameLine();
	ImGui::SetWindowFontScale(1.5f * relativeScale_y);
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d / %d Slain",
		 CPlayer2D::GetInstance()->getPlayerKills(), CScene2D::GetInstance()->getTotalEnemies());
	ImGui::End();

	ImGui::Begin("MiniBoss", NULL, livesWindowFlags);
	ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * 0.8f, cSettings->iWindowHeight * 0.3f));
	ImGui::SetWindowSize(ImVec2(100.0f * relativeScale_x, 25.0f * relativeScale_y));
	ImGui::SetWindowFontScale(1.5f * relativeScale_y);
	cInventoryItem = cInventoryManager->GetItem("deerUI");
	ImGui::Image((void*)(intptr_t)cInventoryItem->GetTextureID(),
		ImVec2(cInventoryItem->vec2Size.x * relativeScale_x, cInventoryItem->vec2Size.y * relativeScale_y),
		ImVec2(0, 1), ImVec2(1, 0));
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d Incoming",
		CScene2D::GetInstance()->getMiniBossQuantity());
	ImGui::End();
	
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoScrollbar;
	//window_flags |= ImGuiWindowFlags_MenuBar;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	window_flags |= ImGuiWindowFlags_NoNav;
	window_flags |= ImGuiWindowFlags_NoBackground;

	//ImGui::ShowStyleEditor();
	ImGui::Begin("Inventory", NULL, window_flags);
	ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
	ImGui::SetWindowSize(ImVec2(cSettings->iWindowWidth, cSettings->iWindowHeight));
	ImGui::SetCursorPos(ImVec2(cSettings->iWindowWidth / 9 * 3, cSettings->iWindowHeight * 0.92f));
	int move_from = -1, move_to = -1;
	//ImGui::ShowDemoWindow();

	for (int i = 0; i < inventoryHotbar; ++i)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(20 * relativeScale_x, 20 * relativeScale_y));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1, 1, 1, 1));
		if (playerInventory[i].active == true)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 1, 1, 1));

			if (ImGui::ImageButton((ImTextureID)playerInventory[i].textureID, ImVec2(20 * relativeScale_x, 20 * relativeScale_y), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
			{
			
				cout << playerInventory[i].slotID << endl;
				updateButtonActivity(i);

			}
			ImGui::PopStyleColor();
		}
		else
		{
			if (ImGui::ImageButton((ImTextureID)playerInventory[i].textureID, ImVec2(20 * relativeScale_x, 20 * relativeScale_y), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
			{
				cout << playerInventory[i].slotID << endl;
				updateButtonActivity(i);

			}

		}
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();
		ImGui::SameLine();


	}
	ImGui::End();
	

	

	

	ImGui::End();
}


/**
 @brief Set up the OpenGL display environment before rendering
 */
void CGUI_Scene2D::PreRender(void)
{
}

/**
 @brief Render this instance
 */
void CGUI_Scene2D::Render(void)
{
	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

/**
 @brief PostRender Set up the OpenGL display environment after rendering.
 */
void CGUI_Scene2D::PostRender(void)
{
}

void CGUI_Scene2D::updateInventory(CInventoryItem* item, int item_ID)
{
	storePlayerItem.push_back(item);
	for (int i = 0; i < storePlayerItem.size(); ++i)
	{
		if (storePlayerItem[i]->GetTextureID() != playerInventory[i].textureID) // check if the slot has no items
		{
			playerInventory[i].textureID = storePlayerItem[i]->GetTextureID();
			playerInventory[i].itemID = item_ID;
		}
		
		else  // Check if inventory alr has an item
		{
			
		    continue;
			
		}
	}
}

int CGUI_Scene2D::updateSelection(void)
{
	for (int i = 0; i < playerInventory.size(); ++i)
	{
		if (playerInventory[i].active == true)
		{
			for (unsigned j = 0; j < TOTAL_NUM; ++j)
			{
				if (playerInventory[i].itemID == j)
				{
					playerInventory[i].noOfItems = storePlayerItem[i]->GetCount();
					if (playerInventory[i].noOfItems == 0)
					{
						//ButtonData temp = playerInventory[i];
						playerInventory[i].textureID = emptyInventorySlot.textureID;
						playerInventory[i].itemID = emptyInventorySlot.itemID;
						reshuffleInventory();
						storePlayerItem.erase(storePlayerItem.begin() + i);					
						
					}
					//cout << playerInventory[i].noOfItems << endl;
 					return playerInventory[i].itemID;
				}
				else
				{
					continue;
				}

			}
		}
	}
	return -1;
}

// Updates the selected Inventory Slot
void CGUI_Scene2D::updateButtonActivity(unsigned index)
{
	for (int i = 0; i < playerInventory.size(); ++i)
	{
		if (i != index)
		{
			playerInventory[i].active = false;
			//cout << "Button " << i << " is inactive\n";
		}
		else
		{
			//cout << "Button " << i << " is active\n";
			playerInventory[i].active = true;
		}
	}
}

void CGUI_Scene2D::reshuffleInventory(void)
{
	for (int i = 0; i < playerInventory.size(); i++)
	{
		if (playerInventory[i].itemID != -1)
		{
			continue;
		}
		else // Means inventory slot is empty
		{
			if (i != playerInventory.size() - 1)
			{
				if (playerInventory[i + 1].itemID != -1) // check if neighbour is empty
				{
					ButtonData temp = playerInventory[i + 1];
					playerInventory[i + 1] = playerInventory[i];
					playerInventory[i + 1].active = false;
					playerInventory[i] = temp;
					playerInventory[i].active = true;
					
				}
				else
				{
					break;
				}
			}
		}
	}
}

int CGUI_Scene2D::getQuantity(void)
{
	for (int i = 0; i < inventorySize; i++)
	{
		if (playerInventory[i].active)
		{
			return storePlayerItem[i]->GetCount();
		}
	}
	return -1;
	
}
