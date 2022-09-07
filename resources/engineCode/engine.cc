#include "engine.h"
#include "../debug/debug.h"

// initialization of OpenGL, etc
void engine::Init () {
	ZoneScoped;
	StartMessage();
	CreateWindowAndContext();
	GlDebugEnable();
	DisplaySetup();
	ShaderCompile();
	SetupTextures();
	ImguiSetup();
	LoadPalette();
	MenuPopulate();
	ReportStartupStats();
}

// terminate ImGUI
void engine::ImguiQuit () {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

// called from destructor
void engine::Quit () {
	ImguiQuit();
	windowHandler.Kill();
}
