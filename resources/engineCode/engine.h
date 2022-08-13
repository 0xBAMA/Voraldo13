#ifndef ENGINE
#define ENGINE
#include "includes.h"

class engine {
public:
	engine()  { Init(); }
	~engine() { Quit(); }

	bool MainLoop (); // called from main

private:
// OpenGL data
	GLuint displayVAO;
	// access to textures, shaders by string label
	unordered_map< string, GLuint > textures;
	unordered_map< string, GLuint > shaders;

//==============================================================================
	// application handles + basic data
	windowHandlerWrapper windowHandler;
	// text renderer framework
	layerManager textRenderer;
	// tracks and visualizes current block orientation
	orientTrident trident;
	// contains the menu entries ( labels + interface layout blocks )
	menuContainer menu;

	// render settings, will probably end up with more, tbd
	renderSettings render;
	// tonemapping parameters + adjustment
	colorGradeParameters tonemap;
	void SendTonemappingParameters ();

//==============================================================================
	// initialization
	void Init ();
	void StartMessage ();
	void CreateWindowAndContext ();
	void DisplaySetup ();
	void ShaderCompile ();
	void ImguiSetup ();
	void MenuPopulate ();
	void CreateTextures ();
	void ReportStartupStats ();

//==============================================================================
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

//==============================================================================
	// imgui menu helpers
	int currentlySelectedMenuItem = -1;
	void MenuSplash ();

//==============================================================================
	// arguments to operation functions tbd
	void MenuAABB ();				float OperationAABB ();
	void MenuCylinderTube ();		float OperationCylinderTube ();
	void MenuEllipsoid ();			float OperationEllipsoid ();
	void MenuGrid ();				float OperationGrid ();
	void MenuHeightmap ();			float OperationHeightmap ();
	void MenuIcosahedron ();		float OperationIcosahedron ();
	void MenuNoise ();				float OperationNoise ();
	void MenuSphere ();				float OperationSphere ();
	void MenuTriangle ();			float OperationTriangle ();
	void MenuUserShader ();			float OperationUserShader ();
	void MenuVAT ();				float OperationVAT ();
	void MenuSpaceship ();			float OperationSpaceship ();
	void MenuLetters ();			float OperationLetters ();
	void MenuXOR ();				float OperationXOR ();
	void MenuClearBlock ();			float OperationClearBlock ();
	void MenuMasking ();			float OperationMasking ();
	void MenuBlur ();				float OperationBlur ();
	void MenuShiftTrim ();			float OperationShiftTrim ();
	void MenuLoadSave ();			float OperationLoadSave ();
	void MenuLimiterCompressor ();	float OperationLimiterCompressor ();
	void MenuCopyPaste ();			float OperationCopyPaste ();
	void MenuLogging ();			float OperationLogging ();
	void MenuScreenshot ();			float OperationScreenshot ();
	void MenuClearLightLevels ();	float OperationClearLightLevels ();
	void MenuPointLight ();			float OperationPointLight ();
	void MenuConeLight ();			float OperationConeLight ();
	void MenuDirectionalLight ();	float OperationDirectionalLight ();
	void MenuFakeGI ();				float OperationFakeGI ();
	void MenuAmbientOcclusion ();	float OperationAmbientOcclusion ();
	void MenuLightMash ();			float OperationLightMash ();

	void MenuApplicationSettings ();
	void MenuRenderingSettings ();
	void MenuPostProcessingSettings ();

//==============================================================================
	// probably refactor these into some other utilites header or something
	glm::vec3 GetColorForTemperature( float temperature ); // 6500.0 is white
	std::vector<uint8_t> BayerData ( int dimension );

//==============================================================================
	// shutdown procedures
	void ImguiQuit ();
	void SDLQuit ();
	void Quit ();

	// program state flags
	bool quitConfirm = false;
	bool pQuit = false;
};
#endif
