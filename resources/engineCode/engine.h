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
	windowHandlerWrapper windowHandler;

	// text renderer framework
	layerManager textRenderer;

	// OpenGL data
	ImVec4 clearColor; // imgui type, for the picker
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
	void SendTonemappingParameters ();

	// render settings, will probably end up with more, tbd
	renderSettings render;

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

	// imgui menu helpers ... will be quite a few of these
	int currentlySelectedMenuItem = -1;
	void MenuSplash ();
	void MenuAABB ();
	void MenuCylinderTube ();
	void MenuEllipsoid ();
	void MenuGrid ();
	void MenuHeightmap ();
	void MenuIcosahedron ();
	void MenuNoise ();
	void MenuSphere ();
	void MenuTriangle ();
	void MenuUserShader ();
	void MenuVAT ();
	void MenuSpaceship ();
	void MenuLetters ();
	void MenuXOR ();
	void MenuClearBlock ();
	void MenuMasking ();
	void MenuBlur ();
	void MenuShiftTrim ();
	void MenuLoadSave ();
	void MenuLimiterCompressor ();
	void MenuCopyPaste ();
	void MenuLogging ();
	void MenuScreenshot ();
	void MenuClearLightLevels ();
	void MenuPointLight ();
	void MenuConeLight ();
	void MenuDirectionalLight ();
	void MenuFakeGI ();
	void MenuAmbientOcclusion ();
	void MenuLightMash ();
	void MenuApplicationSettings ();
	void MenuRenderingSettings ();
	void MenuPostProcessingSettings ();

	// shutdown procedures
	void ImguiQuit ();
	void SDLQuit ();
	void Quit ();

	// program flags
	bool quitConfirm = false;
	bool pQuit = false;
};
#endif
