#pragma once

/**
 COptions
 @brief This class is derived from CGameState. It will introduce the game to the player.
 By: Toh Da Jun
 Date: July 2021
 */

#include "GameStateBase.h"

#include "Primitives/Mesh.h"
#include "../Scene2D/BackgroundEntity.h"

#include <string>

 // Include IMGUI
 // Important: GLEW and GLFW must be included before IMGUI
#ifndef IMGUI_ACTIVE
#include "GUI\imgui.h"
#include "GUI\backends\imgui_impl_glfw.h"
#include "GUI\backends\imgui_impl_opengl3.h"
#include "../SoundController/SoundController.h"
#include "../App/Source/Scene2D/Player2D.h"
#define IMGUI_ACTIVE
#endif

class COptions : public CGameStateBase
{
public:
	// Constructor
	COptions(void);
	// Destructor
	~COptions(void);

	// Init this class instance
	virtual bool Init(void);
	// Update this class instance
	virtual bool Update(const double dElapsedTime);
	// Render this class instance
	virtual void Render(void);
	// Destroy this class instance
	virtual void Destroy(void);

protected:
	struct ButtonData
	{
		std::string fileName;
		unsigned textureID;
	};

	float masterVolume;
	float playerVolume;
	float portalVolume;
	float bgmVolume;
	int selectedSize;
	ButtonData BackButtonData;
	CSoundController* cSoundController;
	CPlayer2D* cPlayer2D;
};
