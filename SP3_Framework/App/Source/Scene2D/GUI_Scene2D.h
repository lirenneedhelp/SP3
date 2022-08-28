/**
 CGUI_Scene2D
 @brief A class which manages the GUI for Scene2D
 By: Toh Da Jun
 Date: May 2021
 */
#pragma once

// Include SingletonTemplate
#include "DesignPatterns/SingletonTemplate.h"

// Include CEntity2D
#include "Primitives/Entity2D.h"

// FPS Counter
#include "TimeControl\FPSCounter.h"

// Include CInventoryManager
#include "InventoryManager.h"

// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

// Include IMGUI
// Important: GLEW and GLFW must be included before IMGUI
#ifndef IMGUI_ACTIVE
#include "GUI\imgui.h"
#include "GUI\backends\imgui_impl_glfw.h"
#include "GUI\backends\imgui_impl_opengl3.h"
#define IMGUI_ACTIVE
#endif

#include "GameControl/Settings.h"
// Include ImageLoader
#include "System\ImageLoader.h"
#include <vector>
#include "Inputs/KeyboardController.h"

#include <string>
using namespace std;

class CGUI_Scene2D : public CSingletonTemplate<CGUI_Scene2D>, public CEntity2D
{
	friend CSingletonTemplate<CGUI_Scene2D>;
public:
	// Init
	bool Init(void);

	// Update
	void Update(const double dElapsedTime);

	// PreRender
	void PreRender(void);

	// Render
	void Render(void);

	// PostRender
	void PostRender(void);

	// Update the Inventory
	void updateInventory(CInventoryItem* item, int item_ID);

	int updateSelection(void);

	void updateButtonActivity(unsigned index); // If one is clicked set the rest to inactive

	void reshuffleInventory(void);

	int getQuantity(void);

protected:
	enum ITEM_ID
	{
		SWORD_ID,
		BOW_ID,
		SPEAR_ID,
		WOOD_ID,
		DIRT_ID,
		AXE_ID,
		SHOVEL_ID,
		TOTAL_NUM,
	};
	// Constructor
	CGUI_Scene2D(void);

	// Destructor
	virtual ~CGUI_Scene2D(void);

	CKeyboardController* cKeyboardController;

	bool releaseButton;

	// FPS Control
	CFPSCounter* cFPSCounter;

	// Flags for IMGUI
	ImGuiWindowFlags window_flags;
	float m_fProgressBar;

	CSettings* cSettings;

	// The handler containing the instance of CInventoryManager
	CInventoryManager* cInventoryManager;
	// The handler containing the instance of CInventoryItem
	CInventoryItem* cInventoryItem;
	struct ButtonData
	{
		std::string fileName;
		unsigned textureID;
		unsigned slotID;
		int itemID;
		unsigned noOfItems;
		bool active;
	};
	vector<CInventoryItem*> storePlayerItem;

	vector<ButtonData> playerInventory;
	ButtonData emptyInventorySlot;

	int currentSlotID;
	int inventoryHotbar; // Size of the hotbar

	int inventorySize; // Size of overall Inventory

	vector <CEntity2D*> enemyHealth; // Stores the enemy health

	int emptyInventoryTextureID;


};
