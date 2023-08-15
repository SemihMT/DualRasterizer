#include "pch.h"

#if defined(_DEBUG)
#include "vld.h"
#endif

#undef main
#include "Renderer.h"
#include <Windows.h> // For colored text on Windows


using namespace dae;

void ShutDown(SDL_Window* pWindow)
{
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
}

// Function to set console text color
void SetConsoleTextColor(int color) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, color);
}

void DisplayControlsOverview()
{
	// Set colors
	const int headingColor = 14;  // Yellow
	const int controlColor = 11;  // Light Cyan
	const int instructionColor = 10;  // Light Green

	// Print Controls Overview
	SetConsoleTextColor(headingColor);
	std::cout << "Welcome to the Rendering Application Controls Overview:\n";

	SetConsoleTextColor(controlColor);
	std::cout << "- Press ";
	SetConsoleTextColor(instructionColor);
	std::cout << "F1";
	SetConsoleTextColor(controlColor);
	std::cout << ": Toggle between DirectX and Software rendering modes.\n";

	std::cout << "- Press ";
	SetConsoleTextColor(instructionColor);
	std::cout << "F2";
	SetConsoleTextColor(controlColor);
	std::cout << ": Enable/Disable Mesh Rotation.\n";

	std::cout << "- Press ";
	SetConsoleTextColor(instructionColor);
	std::cout << "F3";
	SetConsoleTextColor(controlColor);
	std::cout << ": Toggle FireFX mesh (Hardware mode only).\n";

	std::cout << "- Press ";
	SetConsoleTextColor(instructionColor);
	std::cout << "F4";
	SetConsoleTextColor(controlColor);
	std::cout << ": Cycle through SampleStates (point, linear, anisotropic) for rendering (Anisotropic is Hardware mode only).\n";

	std::cout << "- Press ";
	SetConsoleTextColor(instructionColor);
	std::cout << "F5";
	SetConsoleTextColor(controlColor);
	std::cout << ": Cycle through Shading modes (Observed Area, Diffuse, Specular, Combined) for rendering (Software mode only).\n";

	std::cout << "- Press ";
	SetConsoleTextColor(instructionColor);
	std::cout << "F6";
	SetConsoleTextColor(controlColor);
	std::cout << ": Toggle NormalMap usage (Software mode only).\n";

	std::cout << "- Press ";
	SetConsoleTextColor(instructionColor);
	std::cout << "F7";
	SetConsoleTextColor(controlColor);
	std::cout << ": Toggle DepthBuffer Visualization (Software mode only).\n";

	std::cout << "- Press ";
	SetConsoleTextColor(instructionColor);
	std::cout << "F8";
	SetConsoleTextColor(controlColor);
	std::cout << ": Toggle BoundingBox Visualization (Software mode only).\n";

	std::cout << "- Press ";
	SetConsoleTextColor(instructionColor);
	std::cout << "F9";
	SetConsoleTextColor(controlColor);
	std::cout << ": Cycle through Cull modes (backface, frontface, none).\n";

	std::cout << "- Press ";
	SetConsoleTextColor(instructionColor);
	std::cout << "F10";
	SetConsoleTextColor(controlColor);
	std::cout << ": Toggle the use of Uniform ClearColor.\n";

	std::cout << "- Press ";
	SetConsoleTextColor(instructionColor);
	std::cout << "F11";
	SetConsoleTextColor(controlColor);
	std::cout << ": Toggle FPS Printing.\n";

	SetConsoleTextColor(instructionColor);
	std::cout << "\nInstructions:\n";
	SetConsoleTextColor(controlColor);
	std::cout << "- Use the specified function keys to control various rendering aspects of the application.\n";
	SetConsoleTextColor(controlColor);
	std::cout << "- In Hardware mode, F1, F2, F3, F4, F9, F10 and F11 controls are available.\n";
	SetConsoleTextColor(controlColor);
	std::cout << "- In Software mode, F1, F2, F5, F6, F7, F8, F9, F10, and F11 controls are available.\n";
	SetConsoleTextColor(instructionColor);
	std::cout << "- The console will display messages indicating the current state or mode after each control is triggered.\n";

	// Reset console text color to default
	SetConsoleTextColor(7);  // White
}

int main(int argc, char* args[])
{
	//Unreferenced parameters
	(void)argc;
	(void)args;
	
	//Create window + surfaces
	SDL_Init(SDL_INIT_VIDEO);

	const uint32_t width = 640;
	const uint32_t height = 480;

	SDL_Window* pWindow = SDL_CreateWindow(
		"DualRasterizer - Teke Semih 2DAE08",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width, height, 0);

	if (!pWindow)
		return 1;

	//Initialize "framework"
	const auto pTimer = new Timer();
	const auto pRenderer = new Renderer(pWindow);

	//Start loop
	pTimer->Start();
	float printTimer = 0.f;
	bool isLooping = true;

	const std::string DX11RenderingMode {"DirectX 11"};
	const std::string SoftwareRenderingMode {"Software"};
	bool printFPS{ true };

	std::cout << "Dual Rasterizer - Semih Teke 2DAE08\nExtra Feature: Linear Filtering in Software mode (F4)\n";
	DisplayControlsOverview();

	while (isLooping)
	{
		//--------- Get input events ---------
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				isLooping = false;
				break;
			case SDL_KEYUP:
				//Test for a key

				//Rendering
				// 
				// 
				// --------------------
				// Shared
				// --------------------
				// 
				// 
				//Toggle DirectX - Software
				if (e.key.keysym.scancode == SDL_SCANCODE_F1)
				{
					pRenderer->ToggleRenderMode();
					//Print message, mention current state
					std::cout << "\n\nRendering in " << pRenderer->GetRenderingMode() << " mode.\n\n";
				}
				//Toggle Rotation
				if (e.key.keysym.scancode == SDL_SCANCODE_F2)
				{
					pRenderer->ToggleMeshRotation();
					std::cout << "\n\nEnable Mesh Rotation: " << std::boolalpha << pRenderer->GetRotateMeshes() << "\n\n";
				}
				//Cycle Cull modes (backface, frontface, none)
				if (e.key.keysym.scancode == SDL_SCANCODE_F9)
				{
					pRenderer->CycleCullMode();
					std::cout << "\n\nCullmode updated, current Cullmode = " << pRenderer->GetVehicleMesh()->GetCullModeName() << "\n\n";
				}
				//Toggle uniform ClearColor
				if (e.key.keysym.scancode == SDL_SCANCODE_F10)
				{
					pRenderer->ToggleUniformClearColor();
					std::cout << "\n\nUse UniformClearColor: " << std::boolalpha << pRenderer->GetUseUniformClearColor() << "\n\n";
				}
				//Toggle Print FPS
				if (e.key.keysym.scancode == SDL_SCANCODE_F11)
				{
					printFPS = !printFPS;
					std::cout << "\n\nFPS Printing: " << (printFPS ? "on\n\n" : "off\n\n");
				}
				//
				//
				// --------------------
				// Hardware only
				// --------------------
				//
				//Toggle FireFX mesh
				if (e.key.keysym.scancode == SDL_SCANCODE_F3)
				{
					if (pRenderer->GetRenderingMode() == DX11RenderingMode)
						pRenderer->GetFireMesh()->ToggleIsEnabled();

					std::cout << std::boolalpha << "\n\nFireMesh enabled: " << pRenderer->GetFireMesh()->GetIsEnabled() << "\n\n";
				}
				//Cycle SampleStates (point, linear, anisotropic)
				if (e.key.keysym.scancode == SDL_SCANCODE_F4)
				{
					bool usingDX{ pRenderer->GetRenderingMode() == "DirectX 11" ? true : false };
					pRenderer->GetVehicleMesh()->UpdateFilterMode(usingDX);
					pRenderer->GetFireMesh()->UpdateFilterMode(usingDX);

					std::cout << "\n\nCurrent Filter Mode: " << pRenderer->GetVehicleMesh()->GetFilterModeName() << "\n\n";
				}

				//
				//
				// --------------------
				// Software only
				// --------------------
				//
				//Cycle Shading modes
				if (e.key.keysym.scancode == SDL_SCANCODE_F5)
				{
					pRenderer->CycleShadingMode();
					std::cout << "\n\nCurrent shading mode: ";
					switch (pRenderer->GetShadingMode()) {
					case Renderer::LightingMode::ObservedArea:
						std::cout << "Observed Area\n\n";
						break;
					case Renderer::LightingMode::Diffuse:
						std::cout << "Diffuse\n\n";
						break;
					case Renderer::LightingMode::Specular:
						std::cout << "Specular\n\n";
						break;
					case Renderer::LightingMode::Combined:
						std::cout << "Combined\n\n";
						break;
					}
				}
				//Toggle NormalMap
				if (e.key.keysym.scancode == SDL_SCANCODE_F6)
				{
					pRenderer->ToggleNormalMap();
					std::cout << "\n\nUse NormalMap: " << std::boolalpha << pRenderer->GetUseNormalMap() << "\n\n";
				}
				//Toggle DepthBuffer Visualization
				if (e.key.keysym.scancode == SDL_SCANCODE_F7)
				{
					pRenderer->ToggleDepthBufferVis();
					std::cout << "\n\nUse Depth Buffer Visualization: " << std::boolalpha << pRenderer->GetUseDepthBufferVis() << "\n\n";
				}
				//Toggle BoundingBox Visualization
				if (e.key.keysym.scancode == SDL_SCANCODE_F8)
				{
					pRenderer->ToggleBBVis();
					std::cout << "\n\nUse BoundingBox Visualization: " << std::boolalpha << pRenderer->GetUseBBVis() << "\n\n";
				}
				break;
			default:;
			}
		}

		//--------- Update ---------
		pRenderer->Update(pTimer);

		//--------- Render ---------
		pRenderer->Render();

		//--------- Timer ---------
		pTimer->Update();
		printTimer += pTimer->GetElapsed();
		if (printFPS && printTimer >= 1.f)
		{
			printTimer = 0.f;
			std::cout << "dFPS: " << pTimer->GetdFPS() << std::endl;
		}
	}
	pTimer->Stop();

	//Shutdown "framework"
	delete pRenderer;
	delete pTimer;

	ShutDown(pWindow);
	return 0;
}