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
	// access to textures, shaders, bindSets by string label
	unordered_map< string, GLuint > textures;
	unordered_map< string, GLuint > shaders;
	unordered_map< string, bindSet > bindSets;
	void SwapBlocks();

//==============================================================================
	// application handles + basic data
	windowHandlerWrapper windowHandler;
	// text renderer framework
	layerManager textRenderer;
	// tracks and visualizes current block orientation
	orientTrident trident;
	// contains the menu entries ( labels + interface layout blocks )
	menuContainer menu;
	// render settings
	renderState render;
	// tonemapping parameters + adjustment
	colorGradeParameters tonemap;

	void setColorMipmapFlag();
	void setLightMipmapFlag();
	bool mipmapFlagColor = true;
	bool mipmapFlagLight = true;
	void genColorMipmap();
	void genLightMipmap();

//==============================================================================
	// initialization
	void Init ();
	void StartMessage ();
	void CreateWindowAndContext ();
	void DisplaySetup ();
	void ShaderCompile ();
	void ImguiSetup ();
	void MenuPopulate ();
	void SetupTextures ();
	void ReportStartupStats ();

//==============================================================================
	// main loop functions
	void BlitToScreen ();
	void HandleEvents ();
	void ClearColorAndDepth ();
	void ComputePasses ();
	void SendRaymarchParameters ();
	void Raymarch ();
	void SendTonemappingParameters ();
	void Tonemap ();
	void TridentAndTiming ();
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
	bool wantCapturePostprocessScreenshot = false;
	float postprocessScreenshotScaleFactor = 1.0f;
	void OrangeText ( const char *string );
	void ColorPickerHelper ( bool& draw, int& mask, glm::vec4& color );
	ImFont * defaultFont;
	ImFont * titleFont;

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

	void newHeightmapPerlin();
	void newHeightmapDiamondSquare();
	void newHeightmapXOR();
	void newHeightmapAND();

	void CapturePostprocessScreenshot ();
	void SendUniforms ( json j );
	void AddToLog ( json j );
	void BlockDispatch ();

	void updateSavesList();
	std::vector<string> savesList;
	bool hasEnding ( std::string fullString, std::string ending );
	bool hasPNG ( std::string filename );

	// json adder helper functions
	void AddBool ( json& j, string label, bool value );
	void AddInt ( json& j, string label, int value );
	void AddFloat ( json& j, string label, float value );
	void AddIvec3 ( json& j, string label, glm::ivec3 value );
	void AddVec3 ( json& j, string label, glm::vec3 value );
	void AddVec4 ( json& j, string label, glm::vec4 value );

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
