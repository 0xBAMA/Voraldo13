#include "engine.h"

void engine::StartMessage () {
	cout << endl << T_YELLOW << BOLD << "NQADE - Not Quite A Demo Engine" << endl;
	cout << " By Jon Baker ( 2020 - 2022 ) " << RESET << endl;
	cout << "  https://jbaker.graphics/ " << endl << endl;
}

void engine::CreateWindowAndContext () {
	Tick();
	cout << T_BLUE << "    Initializing SDL2" << RESET << " ................................ ";
	windowHandler.PreInit();
	cout << T_GREEN << "done." << T_RED << " ( " << Tock() << " us )" << RESET << endl;

	Tick();
	cout << T_BLUE << "    Creating window" << RESET << " .................................. ";
	windowHandler.Init( 0 );
	cout << T_GREEN << "done." << T_RED << " ( " << Tock() << " us )" << RESET << endl;

	Tick();
	cout << T_BLUE << "    Setting up OpenGL context" << RESET << " ........................ ";
	windowHandler.OpenGLSetup();
	cout << T_GREEN << "done." << T_RED << " ( " << Tock() << " us )" << RESET << endl;
}

void engine::MenuPopulate () {
	// eventually this will come from a json structure containing records of each menu entry
	Tick();
	cout << T_BLUE << "    Populating Menu" << RESET << " .................................. ";

	std::ifstream i( "resources/engineCode/config/menuConfig.json" );
	json j;
	i >> j;

	for ( auto& element : j[ "Entries" ] ) {
		// construct each menu entry and add
		string entryLabel = element[ "Label" ];

		category_t entryCategory = category_t::none;
		if ( element[ "Category" ] == string( "Shapes" ) ) {
			entryCategory = category_t::shapes;
		} else if ( element[ "Category" ] == string( "Utilities" ) ) {
			entryCategory = category_t::utilities;
		} else if ( element[ "Category" ] == string( "Lighting" ) ) {
			entryCategory = category_t::lighting;
		} else if ( element[ "Category" ] == string( "Settings" ) ) {
			entryCategory = category_t::settings;
		}

		menu.entries.push_back( menuEntry( entryLabel, entryCategory ) );
	}
	cout << T_GREEN << "done." << T_RED << " ( " << menu.entries.size() << " entries - " << Tock() << " us )" << RESET << endl;
}

void engine::DisplaySetup () {
	Tick();
	cout << endl << T_BLUE << "    Setting up Display" << RESET << " ............................... ";

	// have to have dummy call to this - OpenGL core spec requires a VAO bound when calling glDrawArrays, otherwise it complains
	glGenVertexArrays( 1, &displayVAO );

	// some info on your current platform
	const GLubyte *renderer = glGetString( GL_RENDERER );	// get renderer string
	const GLubyte *version = glGetString( GL_VERSION );	// version as a string
	const GLubyte *glslVersion = glGetString( GL_SHADING_LANGUAGE_VERSION );	// glsl version as a string
	const GLubyte *vendor = glGetString( GL_VENDOR );		// vendor as a string

	cout << T_GREEN << "done." << T_RED << " ( " << Tock() << " us )" << RESET << endl;


	cout << T_BLUE << "    Platform Info :" << RESET << endl;
	cout << T_RED << "      Vendor : " << T_CYAN << vendor << RESET << endl;
	cout << T_RED << "      Renderer : " << T_CYAN << renderer << RESET << endl;
	cout << T_RED << "      OpenGL version supported : " << T_CYAN << version << RESET << endl;
	cout << T_RED << "      GLSL version supported : " << T_CYAN << glslVersion << RESET << endl << endl;
}

void engine::CreateTextures () {
	Tick();
	cout << T_BLUE << "    Creating Textures" << RESET << " ................................ ";
	GLuint displayTexture;
	GLuint accumulatorTexture;
	GLuint blueNoiseTexture;

	// create the image textures
	Image initial( WIDTH * std::max( SSFACTOR, 1.0 ), HEIGHT * std::max( SSFACTOR, 1.0 ) );
	glGenTextures( 1, &accumulatorTexture );
	glActiveTexture( GL_TEXTURE3 );
	glBindTexture( GL_TEXTURE_2D, accumulatorTexture );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F, WIDTH * SSFACTOR, HEIGHT * SSFACTOR, 0, GL_RGBA, GL_UNSIGNED_BYTE, &initial.data.data()[ 0 ] );
	textures[ "Accumulator" ] = accumulatorTexture;

	glGenTextures( 1, &displayTexture );
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, displayTexture );
	bool linearFilter = true;
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, linearFilter ? GL_LINEAR : GL_NEAREST );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, linearFilter ? GL_LINEAR : GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, &initial.data.data()[ 0 ] );
	textures[ "Display Texture" ] = displayTexture;

	// blue noise image on the GPU
	Image blueNoiseImage{ "resources/noise/blueNoise.png", LODEPNG };
	glGenTextures( 1, &blueNoiseTexture );
	glActiveTexture( GL_TEXTURE4 );
	glBindTexture( GL_TEXTURE_2D, blueNoiseTexture );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, blueNoiseImage.width, blueNoiseImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &blueNoiseImage.data.data()[ 0 ] );
	textures[ "Blue Noise" ] = blueNoiseTexture;

	// create the image for the trident
	GLuint tridentImage;
	Image initialT( trident.blockDimensions.x * 8, trident.blockDimensions.y * 16, true );
	glGenTextures( 1, &tridentImage );
	glActiveTexture( GL_TEXTURE5 );
	glBindTexture( GL_TEXTURE_2D, tridentImage );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, initialT.width, initialT.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &initialT.data.data()[ 0 ] );
	trident.PassInImage( tridentImage );
	textures[ "Trident" ] = tridentImage;




	cout << T_GREEN << "done." << T_RED << " ( " << Tock() << " us )" << RESET << endl;
}

void engine::ShaderCompile () {
	Tick();
	cout << T_BLUE << "    Compiling Shaders" << RESET << " ................................ ";

	const string base( "resources/engineCode/shaders/" );

	// main display blit
	shaders[ "Display" ] = regularShader( base + "blit.vs.glsl", base + "blit.fs.glsl" ).shaderHandle;

	// something to put data in the accumulator texture
	shaders[ "Dummy Draw" ] = computeShader( base + "dummyDraw.cs.glsl" ).shaderHandle;

	// color adjustments
	shaders[ "Tonemap" ] = computeShader( base + "tonemap.cs.glsl" ).shaderHandle;

	// basic image based raymarch
	shaders[ "Raymarch" ] = computeShader( base + "raymarch.cs.glsl" ).shaderHandle;

	// initialize the text renderer
	shaders[ "Font Renderer" ] = computeShader( "resources/fonts/fontRenderer/font.cs.glsl" ).shaderHandle;
	textRenderer.Init( WIDTH, HEIGHT, shaders[ "Font Renderer" ] );
	// get the base point to draw the gizmo/widget/trident from the text renderer
	trident.basePt = textRenderer.basePt;

	// orientation trident shaders
	shaders[ "Trident Raymarch" ] = computeShader( base + "tridentGenerate.cs.glsl" ).shaderHandle;
	shaders[ "Trident Blit" ] = computeShader( base + "tridentCopy.cs.glsl" ).shaderHandle;
	trident.PassInShaders( shaders[ "Trident Raymarch" ], shaders[ "Trident Blit" ] );

		// ...

	cout << T_GREEN << "done." << T_RED << " ( " << Tock() << " us )" << RESET << endl;

}

void engine::ImguiSetup () {
	Tick();
	cout << T_BLUE << "    Configuring dearImGUI" << RESET << " ............................ ";

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();

	// enable docking
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	// Setup Platform/Renderer bindings
	ImGui_ImplSDL2_InitForOpenGL( windowHandler.window, windowHandler.GLcontext );
	const char *glslVersion = "#version 430";
	ImGui_ImplOpenGL3_Init( glslVersion );

	// initial value for clear color
	// clearColor = ImVec4( 0.295f, 0.295f, 0.295f, 0.5f );
	render.clearColor = ImVec4( 0.0f, 0.0f, 0.0f, 1.0f );
	// glClearColor( clearColor.x, clearColor.y, clearColor.z, clearColor.w );
	// glClear( GL_COLOR_BUFFER_BIT );
	// SDL_GL_SwapWindow( window ); // show clear color

	// setting custom font, if desired
	// io.Fonts->AddFontFromFileTTF( "resources/fonts/ttf/star_trek/titles/TNG_Title.ttf", 32 );
	// io.Fonts->AddFontFromFileTTF( "resources/fonts/ttf/star_trek/titles/DS9_Credits.ttf", 32 );
	// io.Fonts->AddFontFromFileTTF( "resources/fonts/ttf/star_trek/titles/Montalban.ttf", 32 );
	// io.Fonts->AddFontFromFileTTF( "resources/fonts/ttf/star_trek/text/Beijing.ttf", 32 );
	// io.Fonts->AddFontFromFileTTF( "resources/fonts/ttf/Braciola MS ExB.ttf", 32 );

	ImVec4 *colors = ImGui::GetStyle().Colors;
	colors[ ImGuiCol_Text ]						= ImVec4( 0.67f, 0.50f, 0.16f, 1.00f );
	colors[ ImGuiCol_TextDisabled ]				= ImVec4( 0.33f, 0.27f, 0.16f, 1.00f );
	colors[ ImGuiCol_WindowBg ]					= ImVec4( 0.10f, 0.05f, 0.00f, 1.00f );
	colors[ ImGuiCol_ChildBg ]					= ImVec4( 0.23f, 0.17f, 0.02f, 0.05f );
	colors[ ImGuiCol_PopupBg ]					= ImVec4( 0.30f, 0.12f, 0.06f, 0.94f );
	colors[ ImGuiCol_Border ]					= ImVec4( 0.25f, 0.18f, 0.09f, 0.33f );
	colors[ ImGuiCol_BorderShadow ]				= ImVec4( 0.33f, 0.15f, 0.02f, 0.17f );
	colors[ ImGuiCol_FrameBg ]					= ImVec4( 0.561f, 0.082f, 0.04f, 0.17f );
	colors[ ImGuiCol_FrameBgHovered ]			= ImVec4( 0.19f, 0.09f, 0.02f, 0.17f );
	colors[ ImGuiCol_FrameBgActive ]			= ImVec4( 0.25f, 0.12f, 0.01f, 0.78f );
	colors[ ImGuiCol_TitleBg ]					= ImVec4( 0.25f, 0.12f, 0.01f, 1.00f );
	colors[ ImGuiCol_TitleBgActive ]			= ImVec4( 0.33f, 0.15f, 0.02f, 1.00f );
	colors[ ImGuiCol_TitleBgCollapsed ]			= ImVec4( 0.25f, 0.12f, 0.01f, 1.00f );
	colors[ ImGuiCol_MenuBarBg ]				= ImVec4( 0.14f, 0.07f, 0.02f, 1.00f );
	colors[ ImGuiCol_ScrollbarBg ]				= ImVec4( 0.13f, 0.10f, 0.08f, 0.53f );
	colors[ ImGuiCol_ScrollbarGrab ]			= ImVec4( 0.25f, 0.12f, 0.01f, 0.78f );
	colors[ ImGuiCol_ScrollbarGrabHovered ]		= ImVec4( 0.33f, 0.15f, 0.02f, 1.00f );
	colors[ ImGuiCol_ScrollbarGrabActive ]		= ImVec4( 0.25f, 0.12f, 0.01f, 0.78f );
	colors[ ImGuiCol_CheckMark ]				= ImVec4( 0.69f, 0.45f, 0.11f, 1.00f );
	colors[ ImGuiCol_SliderGrab ]				= ImVec4( 0.28f, 0.18f, 0.06f, 1.00f );
	colors[ ImGuiCol_SliderGrabActive ]			= ImVec4( 0.36f, 0.22f, 0.06f, 1.00f );
	colors[ ImGuiCol_Button ]					= ImVec4( 0.25f, 0.12f, 0.01f, 0.78f );
	colors[ ImGuiCol_ButtonHovered ]			= ImVec4( 0.33f, 0.15f, 0.02f, 1.00f );
	colors[ ImGuiCol_ButtonActive ]				= ImVec4( 0.25f, 0.12f, 0.01f, 0.78f );
	colors[ ImGuiCol_Header ]					= ImVec4( 0.25f, 0.12f, 0.01f, 0.78f );
	colors[ ImGuiCol_HeaderHovered ]			= ImVec4( 0.33f, 0.15f, 0.02f, 1.00f );
	colors[ ImGuiCol_HeaderActive ]				= ImVec4( 0.25f, 0.12f, 0.01f, 0.78f );
	colors[ ImGuiCol_Separator ]				= ImVec4( 0.28f, 0.18f, 0.06f, 0.37f );
	colors[ ImGuiCol_SeparatorHovered ]			= ImVec4( 0.33f, 0.15f, 0.02f, 0.17f );
	colors[ ImGuiCol_SeparatorActive ]			= ImVec4( 0.42f, 0.18f, 0.06f, 0.17f );
	colors[ ImGuiCol_ResizeGrip ]				= ImVec4( 0.25f, 0.12f, 0.01f, 0.78f );
	colors[ ImGuiCol_ResizeGripHovered ]		= ImVec4( 0.33f, 0.15f, 0.02f, 1.00f );
	colors[ ImGuiCol_ResizeGripActive ]			= ImVec4( 0.25f, 0.12f, 0.01f, 0.78f );
	colors[ ImGuiCol_Tab ]						= ImVec4( 0.25f, 0.12f, 0.01f, 0.78f );
	colors[ ImGuiCol_TabHovered ]				= ImVec4( 0.33f, 0.15f, 0.02f, 1.00f );
	colors[ ImGuiCol_TabActive ]				= ImVec4( 0.34f, 0.14f, 0.01f, 1.00f );
	colors[ ImGuiCol_TabUnfocused ]				= ImVec4( 0.33f, 0.15f, 0.02f, 1.00f );
	colors[ ImGuiCol_TabUnfocusedActive ]		= ImVec4( 0.42f, 0.18f, 0.06f, 1.00f );
	colors[ ImGuiCol_PlotLines ]				= ImVec4( 0.61f, 0.61f, 0.61f, 1.00f );
	colors[ ImGuiCol_PlotLinesHovered ]			= ImVec4( 1.00f, 0.43f, 0.35f, 1.00f );
	colors[ ImGuiCol_PlotHistogram ]			= ImVec4( 0.90f, 0.70f, 0.00f, 1.00f );
	colors[ ImGuiCol_PlotHistogramHovered ]		= ImVec4( 1.00f, 0.60f, 0.00f, 1.00f );
	colors[ ImGuiCol_TextSelectedBg ]			= ImVec4( 0.06f, 0.03f, 0.01f, 0.78f );
	colors[ ImGuiCol_DragDropTarget ]			= ImVec4( 0.64f, 0.42f, 0.09f, 0.90f );
	colors[ ImGuiCol_NavHighlight ]				= ImVec4( 0.64f, 0.42f, 0.09f, 0.90f );
	colors[ ImGuiCol_NavWindowingHighlight ]	= ImVec4( 1.00f, 1.00f, 1.00f, 0.70f );
	colors[ ImGuiCol_NavWindowingDimBg ]		= ImVec4( 0.80f, 0.80f, 0.80f, 0.20f );
	colors[ ImGuiCol_ModalWindowDimBg ]			= ImVec4( 0.80f, 0.80f, 0.80f, 0.35f );

	ImGuiStyle &style = ImGui::GetStyle();

	style.TabRounding = 2;
	style.FrameRounding = 2;
	style.WindowPadding.x = 6;
	style.WindowPadding.y = 4;
	style.FramePadding.x = 4;
	style.FramePadding.y = 2;
	style.ScrollbarSize = 10;

	cout << T_GREEN << "done." << T_RED << " ( " << Tock() << " us )" << RESET << endl;
}

void engine::ReportStartupStats () {
	cout << endl << T_CYAN << "  " << shaders.size() << " shaders." << endl;
	cout << "  " << textures.size() << " textures." << endl;
	cout << T_YELLOW << "  Startup is complete ( total " << TotalTime() << " us )" << RESET << endl << endl;
}
