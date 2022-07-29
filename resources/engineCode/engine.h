#ifndef ENGINE
#define ENGINE
#include "includes.h"

class engine {
public:
	engine()  { Init(); }
	~engine() { Quit(); }

	bool MainLoop (); // called from main

private:
	// application handles + basic data
	// windowHandler w;
	SDL_Window * window;
	SDL_GLContext GLcontext;
	int totalScreenWidth;
	int totalScreenHeight;
	ImVec4 clearColor;

	// text renderer framework
	layerManager textRenderer;

	// OpenGL data
	GLuint blueNoiseTexture;
	GLuint accumulatorTexture;
	GLuint displayTexture;
	GLuint displayShader;
	GLuint displayVAO;
	GLuint dummyDrawShader;
	GLuint tonemapShader;

	// tracks and visualizes current block orientation
	orientTrident trident;

	// contains the menu entries ( labels + interface layout blocks )
	menuContainer menu;

	// tonemapping parameters + adjustment
	colorGradeParameters tonemap;
	void TonemapControlsWindow ();
	void SendTonemappingParameters ();

	// initialization
	void Init ();
	void StartMessage ();
	void CreateWindowAndContext ();
	void DisplaySetup ();
	void ComputeShaderCompile ();
	void ImguiSetup ();
	void MenuPopulate ();

	// main loop functions
	void BlitToScreen ();
	void HandleEvents ();
	void ClearColorAndDepth ();
	void ComputePasses ();
	void ImguiPass ();
	void ImguiFrameStart ();
	void ImguiFrameEnd ();
	void DrawTextEditor ();
	void MenuLayout ( bool* open );
	void QuitConf ( bool* open );

	// shutdown procedures
	void ImguiQuit ();
	void SDLQuit ();
	void Quit ();

	// program flags
	bool quitConfirm = false;
	bool pQuit = false;
};
#endif
