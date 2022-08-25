// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

#include "Options.h"

// Include CGameStateManager
#include "GameStateManager.h"

// Include Mesh Builder
#include "Primitives/MeshBuilder.h"
// Include ImageLoader
#include "System\ImageLoader.h"
// Include Shader Manager
#include "RenderControl\ShaderManager.h"

 // Include shader
#include "RenderControl\shader.h"

// Include CSettings
#include "GameControl/Settings.h"

// Include CKeyboardController
#include "Inputs/KeyboardController.h"

#include "../SoundController/SoundController.h"
#include "../Source/GameControl/Settings.h"
#include "../Application.h"
#include <iostream>
using namespace std;


/**
 @brief Constructor
 */
COptions::COptions(void)
	//: background(NULL)
{

}

/**
 @brief Destructor
 */
COptions::~COptions(void)
{

}

/**
 @brief Init this class instance
 */
bool COptions::Init(void)
{
	cout << "COptions::Init()\n" << endl;

	CShaderManager::GetInstance()->Use("Shader2D");
	//CShaderManager::GetInstance()->activeShader->setInt("texture1", 0);

	cPlayer2D = CPlayer2D::GetInstance();
	cSoundController = CSoundController::GetInstance();
	/*masterVolume = cSoundController->returnMasterVolume() * 100;
	playerVolume = cSoundController->returnVolume(3) * 100;
	portalVolume = cSoundController->returnVolume(4) * 100;
	bgmVolume = cSoundController->returnVolume(6) * 100;*/
	// Load the images for buttons
	CImageLoader* il = CImageLoader::GetInstance();
	BackButtonData.fileName = "Image\\GUI\\tickbutton.png";
    BackButtonData.textureID = il->LoadTextureGetID(BackButtonData.fileName.c_str(), false);

	selectedSize = 0;

	return true;
}

/**
 @brief Update this class instance
 */
bool COptions::Update(const double dElapsedTime)
{
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoScrollbar;
	window_flags |= ImGuiWindowFlags_NoResize;
	//window_flags |= ImGuiWindowFlags_MenuBar;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	window_flags |= ImGuiWindowFlags_NoNav;
    

	float buttonWidth = 25;
	float buttonHeight = 25;

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{
		static float f = 0.0f;
		static int counter = 0;
		static const char* selection[]{ "Windowed","Fullscreen" };
		static const char* current_item = NULL;
	


		// Create a window called "Hello, world!" and append into it.
		ImGui::Begin("Main Menu", NULL, window_flags);
		ImGui::SetWindowPos(ImVec2(CSettings::GetInstance()->iWindowWidth/6.0, 
			CSettings::GetInstance()->iWindowHeight/4.0));				// Set the top-left of the window at (10,10)
		ImGui::SetWindowSize(ImVec2(CSettings::GetInstance()->iWindowWidth / 2, CSettings::GetInstance()->iWindowHeight / 2));

		//Added rounding for nicer effect
		ImGuiStyle& style = ImGui::GetStyle();
		style.FrameRounding = 200.0f;
		// Display the FPS
		ImGui::TextColored(ImVec4(1, 1, 1, 1), "In-Game Menu: %f", 1 / dElapsedTime);
		if (ImGui::ImageButton((ImTextureID)BackButtonData.textureID,
			ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
		{
			// Reset the CKeyboardController
			CKeyboardController::GetInstance()->Reset();

			CGameStateManager::GetInstance()->SetPauseGameState("OptionsState"); // Since it alr turns pointer into nullptr I didn't feel the need to pass in a nullptr in there
			CGameStateManager::GetInstance()->SetPauseGameState("PauseState");
		}
		ImGui::BeginChild("Master Volume");
		{
			ImGui::SliderFloat("", &masterVolume, 0.0f, 100.0f, "Master Fx:%.1f");
			//cSoundController->setMasterVolume(masterVolume / 100);
			ImGui::BeginChild("Jump Volume");
			{
				ImGui::SliderFloat("", &playerVolume, 0.0f, 100.0f, "Jump Fx:%.1f");
				//cSoundController->setVolume(playerVolume / 100, 3);
				ImGui::BeginChild("Portal Volume");
				{
					ImGui::SliderFloat("", &portalVolume, 0.0f, 100.0f, "Portal Fx:%.1f");
					//cSoundController->setVolume(portalVolume / 100, 4);
					ImGui::BeginChild("Player Colour");
					{
						static float color[] = { 1.f,0.f,0.f,1.f };
						ImGui::ColorEdit4("Edit", color);
						ImGui::ColorPicker4("Pick", color);
						cPlayer2D->setPlayerRuntimeColor(glm::vec4(color[0], color[1], color[2], color[3]));

					}
					ImGui::EndChild();

				}
				ImGui::EndChild();
			}
			ImGui::EndChild();
		}
		ImGui::EndChild();

	ImGui::End();
	}
	if (masterVolume == 0)
	{
		/*cSoundController->setVolume(0, 3);
		cSoundController->setVolume(0, 4);
		cSoundController->setVolume(0, 5);*/

	}
	/*if (ImGui::BeginCombo("Window Size", current_item))
				{
					for (int i = 0; i < IM_ARRAYSIZE(selection); ++i)
					{
						bool is_selected = (current_item == selection[i]);
						if (ImGui::Selectable(selection[i], is_selected))
						{
							current_item = selection[i];
						}
						if (is_selected)
						{
							ImGui::SetItemDefaultFocus();
						}

					}
					ImGui::EndCombo();
					if (current_item == selection[0])
					{
						CSettings::GetInstance()->iWindowWidth = 800;
						CSettings::GetInstance()->iWindowHeight = 600;

					}
					else if (current_item == selection[1])
					{
						Application::GetInstance()->setFullScreen();

						cout << Application::GetInstance()->GetWindowWidth() << endl;

					}

				}*/
	return true;
}

/**
 @brief Render this class instance
 */
void COptions::Render(void)
{
	// Clear the screen and buffer
	glClearColor(0.0f, 0.55f, 1.00f, 1.00f);

	//cout << "COptions::Render()\n" << endl;
}

/**
 @brief Destroy this class instance
 */
void COptions::Destroy(void)
{
	// cout << "COptions::Destroy()\n" << endl;
}
