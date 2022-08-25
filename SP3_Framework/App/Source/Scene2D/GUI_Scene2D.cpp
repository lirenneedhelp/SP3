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
	
	for (int i = 0; i < inventoryHotbar; ++i)
	{
		emptyInventorySlot.fileName = "Image\\GUI\\itemhotbar.png";
		emptyInventorySlot.textureID = il->LoadTextureGetID(emptyInventorySlot.fileName.c_str(), false);
		emptyInventorySlot.slotID = i;
		emptyInventorySlot.itemID = -1;
		emptyInventorySlot.active = false;
		playerInventory.push_back(emptyInventorySlot);
	}
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
		//ImGuiDragDropFlags src_flags = 0;
		//src_flags |= ImGuiDragDropFlags_SourceNoDisableHover;     // Keep the source displayed as hovered
		//src_flags |= ImGuiDragDropFlags_SourceNoHoldToOpenOthers; // Because our dragging is local, we disable the feature of opening foreign treenodes/tabs while dragging
		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(20 * relativeScale_x, 20 * relativeScale_y));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1, 1, 1, 1));

		if (playerInventory[i].active == true)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 1, 1, 1));

			if (ImGui::ImageButton((ImTextureID)playerInventory[i].textureID, ImVec2(20 * relativeScale_x, 20 * relativeScale_y), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
			{
			
				cout << playerInventory[i].slotID << endl;
				playerInventory[i].active = true;
				updateButtonActivity(i);
				

				//src_flags |= ImGuiDragDropFlags_SourceNoPreviewTooltip; // Hide the tooltip
				//if (ImGui::BeginDragDropSource(src_flags))
				//{
				//	if (!(src_flags & ImGuiDragDropFlags_SourceNoPreviewTooltip))
				//		ImGui::Text("Moving \"%d\"", playerInventory[i].textureID);
				//	ImGui::SetDragDropPayload("DND_DEMO_NAME", &i, sizeof(int));
				//	ImGui::EndDragDropSource();
				//	
				//}
				//if (ImGui::BeginDragDropTarget())
				//{
				//	ImGuiDragDropFlags target_flags = 0;
				//	target_flags |= ImGuiDragDropFlags_AcceptBeforeDelivery;    // Don't wait until the delivery (release mouse button on a target) to do something
				//	target_flags |= ImGuiDragDropFlags_AcceptNoDrawDefaultRect; // Don't display the yellow rectangle
				//	if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_NAME", target_flags))
				//	{
				//		move_from = *(const int*)payload->Data;
				//		move_to = i;
				//	}
				//	ImGui::EndDragDropTarget();
				//}

			}
			ImGui::PopStyleColor();
		}
		else
		{
			if (ImGui::ImageButton((ImTextureID)playerInventory[i].textureID, ImVec2(20 * relativeScale_x, 20 * relativeScale_y), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
			{
				cout << playerInventory[i].slotID << endl;
				playerInventory[i].active = true;
				updateButtonActivity(i);
				//src_flags |= ImGuiDragDropFlags_SourceNoPreviewTooltip; // Hide the tooltip
				//if (ImGui::BeginDragDropSource(src_flags))
				//{
				//	if (!(src_flags & ImGuiDragDropFlags_SourceNoPreviewTooltip))
				//		ImGui::Text("Moving \"%d\"", playerInventory[i].textureID);
				//	ImGui::SetDragDropPayload("DND_DEMO_NAME", &i, sizeof(int));
				//	ImGui::EndDragDropSource();
				//	
				//}
				//if (ImGui::BeginDragDropTarget())
				//{
				//	ImGuiDragDropFlags target_flags = 0;
				//	target_flags |= ImGuiDragDropFlags_AcceptBeforeDelivery;    // Don't wait until the delivery (release mouse button on a target) to do something
				//	target_flags |= ImGuiDragDropFlags_AcceptNoDrawDefaultRect; // Don't display the yellow rectangle
				//	if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_NAME", target_flags))
				//	{
				//		move_from = *(const int*)payload->Data;
				//		move_to = i;
				//	}
				//	ImGui::EndDragDropTarget();
				//}

			}

		}
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();
		ImGui::SameLine();


	}
	//if (move_from != -1 && move_to != -1)
	//{
	//	// Reorder items
	//	int copy_dst = (move_from < move_to) ? move_from : move_to + 1;
	//	int copy_src = (move_from < move_to) ? move_from + 1 : move_to;
	//	int copy_count = (move_from < move_to) ? move_to - move_from : move_from - move_to;
	//    unsigned tmp = playerInventory[move_from].textureID;
	//	//printf("[%05d] move %d->%d (copy %d..%d to %d..%d)\n", ImGui::GetFrameCount(), move_from, move_to, copy_src, copy_src + copy_count - 1, copy_dst, copy_dst + copy_count - 1);
	//	memmove(&playerInventory[copy_dst].textureID, &playerInventory[copy_src].textureID, (size_t)copy_count * sizeof(unsigned));
	//	playerInventory[move_to].textureID = tmp;
	//	ImGui::SetDragDropPayload("DND_DEMO_NAME", &move_to, sizeof(int)); // Update payload immediately so on the next frame if we move the mouse to an earlier item our index payload will be correct. This is odd and showcase how the DnD api isn't best presented in this example.
	//}
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

void CGUI_Scene2D::updateInventory(CInventoryItem* item, unsigned item_ID)
{
	storePlayerItem.push_back(item);
	for (int i = 0; i < storePlayerItem.size(); ++i)
	{
		if (storePlayerItem[i]->GetTextureID() != playerInventory[i].textureID) // Check whether the inventory is empty
		{
			playerInventory[i].textureID = storePlayerItem[i]->GetTextureID();
			playerInventory[i].itemID = item_ID;
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
			continue;
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
