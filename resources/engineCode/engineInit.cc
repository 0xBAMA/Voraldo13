#include "engine.h"

void engine::StartMessage () {
	ZoneScoped;
	cout << endl << T_YELLOW << BOLD << "NQADE - Not Quite A Demo Engine" << endl;
	cout << " By Jon Baker ( 2020 - 2022 ) " << RESET << endl;
	cout << "  https://jbaker.graphics/ " << endl << endl;
}

void engine::CreateWindowAndContext () {
	ZoneScoped;
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
	ZoneScoped;
	Tick();
	cout << T_BLUE << "    Populating Menu" << RESET << " .................................. ";
	std::ifstream i( "resources/engineCode/config/menuConfig.json" );
	json j; i >> j;
	for ( auto& element : j[ "Entries" ] ) {
		// construct each menu entry and add
		string entryLabel = element[ "Label" ];
		category_t entryCategory = category_t::none;
		if ( element[ "Category" ] == string( "Shapes" ) )
			entryCategory = category_t::shapes;
		else if ( element[ "Category" ] == string( "Utilities" ) )
			entryCategory = category_t::utilities;
		else if ( element[ "Category" ] == string( "Lighting" ) )
			entryCategory = category_t::lighting;
		else if ( element[ "Category" ] == string( "Settings" ) )
			entryCategory = category_t::settings;
		menu.entries.push_back( menuEntry( entryLabel, entryCategory ) );
	}
	cout << T_GREEN << "done." << T_RED << " ( " << menu.entries.size() << " entries - " << Tock() << " us )" << RESET << endl;
}

void engine::DisplaySetup () {
	ZoneScoped;
	Tick();
	cout << endl << T_BLUE << "    Setting up Display" << RESET << " ............................... ";

	// have to have this - OpenGL core spec requires a VAO bound when calling glDrawArrays
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

std::vector<uint8_t> engine::BayerData ( int dimension ) {
	ZoneScoped;
	if ( dimension == 4 ) {
		std::vector<uint8_t> pattern4 = {
			0,  8,  2,  10,	/* values begin scaled to the range 0..15 */
			12, 4,  14, 6,	/* so they need to be rescaled by 16 */
			3,  11, 1,  9,
			15, 7,  13, 5 };

		for ( auto &x : pattern4 )
			x *= 16;

		return pattern4;
	} else if ( dimension == 8 ) {
		std::vector<uint8_t> pattern8 = {
			0, 32,  8, 40,  2, 34, 10, 42,   /* 8x8 Bayer ordered dithering  */
			48, 16, 56, 24, 50, 18, 58, 26,  /* pattern. Each input pixel */
			12, 44,  4, 36, 14, 46,  6, 38,  /* starts scaled to the 0..63 range */
			60, 28, 52, 20, 62, 30, 54, 22,  /* before looking in this table */
			3, 35, 11, 43,  1, 33,  9, 41,   /* to determine the action. */
			51, 19, 59, 27, 49, 17, 57, 25,
			15, 47,  7, 39, 13, 45,  5, 37,
			63, 31, 55, 23, 61, 29, 53, 21 };

		for ( auto &x : pattern8 )
			x *= 4;

		return pattern8;
	} else {
		return std::vector<uint8_t>{ 0 };
	}
}

void engine::SetupTextures () {
	ZoneScoped;
	Tick();
	cout << T_BLUE << "    Creating Textures and Bindsets" << RESET << " ................... ";

	// color blocks ( front and back )
	size_t numBytesBlock = BLOCKDIM * BLOCKDIM * BLOCKDIM * 4;
	std::vector<uint8_t> zeroes;
	zeroes.resize( numBytesBlock, 0 );
	std::vector<float> ones;
	ones.resize( numBytesBlock, 1.0f );
	std::vector<uint8_t> initialXOR;
	initialXOR.reserve( numBytesBlock );
	for ( uint32_t x = 0; x < BLOCKDIM; x++ ) {
		for ( uint32_t y = 0; y < BLOCKDIM; y++ ) {
			for ( uint32_t z = 0; z < BLOCKDIM; z++ ) {
				for ( int c = 0; c < 4; c++ ) {
					initialXOR.push_back( x ^ y ^ z );
				}
			}
		}
	}

	GLuint displayTexture;
	GLuint accumulatorTexture;
	GLuint blueNoiseTexture;
	GLuint bayer4, bayer8;
	GLuint tridentImage;
	GLuint colorTextures[ 2 ];
	GLuint maskTextures[ 2 ];
	GLuint lightTexture;
	GLuint loadBuffer;

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
	// make sure that these are complete textures, apparently some drivers require these to be set
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, blueNoiseImage.width, blueNoiseImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &blueNoiseImage.data.data()[ 0 ] );
	textures[ "Blue Noise" ] = blueNoiseTexture;

	// bayer patterns
	glActiveTexture( GL_TEXTURE5 );
	glBindTexture( GL_TEXTURE_2D, bayer4 );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_R8, 4, 4, 0, GL_RED, GL_UNSIGNED_BYTE, &BayerData( 4 )[0] );
	textures[ "Bayer4" ] = bayer4;

	glActiveTexture( GL_TEXTURE6 );
	glBindTexture( GL_TEXTURE_2D, bayer8 );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_R8, 8, 8, 0, GL_RED, GL_UNSIGNED_BYTE, &BayerData( 8 )[0] );
	textures[ "Bayer8" ] = bayer8;

	// create the image for the trident
	Image initialT( trident.blockDimensions.x * 8, trident.blockDimensions.y * 16 );
	glGenTextures( 1, &tridentImage );
	glActiveTexture( GL_TEXTURE7 );
	glBindTexture( GL_TEXTURE_2D, tridentImage );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, initialT.width, initialT.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &initialT.data.data()[ 0 ] );
	trident.PassInImage( tridentImage );
	textures[ "Trident" ] = tridentImage;

	glGenTextures( 2, &colorTextures[ 0 ] );
	glActiveTexture( GL_TEXTURE8 );
	glBindTexture( GL_TEXTURE_3D, colorTextures[ 0 ] );
	glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT );
	glTexImage3D( GL_TEXTURE_3D, 0, GL_RGBA8, BLOCKDIM, BLOCKDIM, BLOCKDIM, 0, GL_RGBA, GL_UNSIGNED_BYTE, &initialXOR.data()[ 0 ] );
	textures[ "Color Block Front" ] = colorTextures[ 0 ];

	glActiveTexture( GL_TEXTURE9 );
	glBindTexture( GL_TEXTURE_3D, colorTextures[ 1 ] );
	glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT );
	glTexImage3D( GL_TEXTURE_3D, 0, GL_RGBA8, BLOCKDIM, BLOCKDIM, BLOCKDIM, 0, GL_RGBA, GL_UNSIGNED_BYTE, &zeroes.data()[ 0 ] );
	textures[ "Color Block Back" ] = colorTextures[ 1 ];

	// mask blocks ( front and back - can this be consolidated? not sure if two are needed )
	glGenTextures( 2, &maskTextures[ 0 ] );
	glActiveTexture( GL_TEXTURE10 );
	glBindTexture( GL_TEXTURE_3D, maskTextures[ 0 ] );
	glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT );
	glTexImage3D( GL_TEXTURE_3D, 0, GL_R8UI, BLOCKDIM, BLOCKDIM, BLOCKDIM, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, &zeroes.data()[ 0 ] );
	textures[ "Mask Block Front" ] = maskTextures[ 0 ];

	glActiveTexture( GL_TEXTURE11 );
	glBindTexture( GL_TEXTURE_3D, maskTextures[ 1 ] );
	glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT );
	glTexImage3D( GL_TEXTURE_3D, 0, GL_R8UI, BLOCKDIM, BLOCKDIM, BLOCKDIM, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, &zeroes.data()[ 0 ] );
	textures[ "Mask Block Back" ] = maskTextures[ 1 ];

	// lighting data ( can probably get away with just the one buffer, tbd )
	// also, do I need 16-bit floats or can I get away with less ( GL_R11F_G11F_B10F or GL_RGB9_E5? )
	glGenTextures( 1, &lightTexture );
	glActiveTexture( GL_TEXTURE12 );
	glBindTexture( GL_TEXTURE_3D, lightTexture );
	glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT );
	glTexImage3D( GL_TEXTURE_3D, 0, GL_RGBA16F, BLOCKDIM, BLOCKDIM, BLOCKDIM, 0, GL_RGBA, GL_FLOAT, &ones.data()[ 0 ] );
	textures[ "Lighting Block" ] = lightTexture;

	// loadbuffer for VAT + load/save
	glGenTextures( 1, &loadBuffer );
	glActiveTexture( GL_TEXTURE13 );
	glBindTexture( GL_TEXTURE_3D, loadBuffer );
	glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameterf( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT );
	glTexImage3D( GL_TEXTURE_3D, 0, GL_RGBA8, BLOCKDIM, BLOCKDIM, BLOCKDIM, 0, GL_RGBA, GL_UNSIGNED_BYTE, &zeroes.data()[ 0 ] );
	textures[ "Loadbuffer" ] = loadBuffer;

/*==============================================================================
other textures, tbd
	> heightmap - diamond square initially
	> noise buffer - some placeholder noise, maybe for now - or maybe just use loadBuffer? tbd
	> copy/paste buffer ( how is this going to happen, what does the interface look like? )
==============================================================================*/

// configure bindsets
// two variants will be required for any that need to switch front and back buffers, since we can't get at
// the texture handle after it's in the bindset - use std::swap to switch them when required in SwapBlocks()
	bindSets[ "Rendering" ] = bindSet( {
		binding( 0, textures[ "Blue Noise" ], GL_RGBA8UI ),
		binding( 1, textures[ "Accumulator" ], GL_RGBA16F ),
		binding( 2, textures[ "Color Block Front" ], GL_RGBA8UI ),
		binding( 3, textures[ "Lighting Block" ], GL_RGBA16F )
	} );

	bindSets[ "Rendering Back Set" ] = bindSet( {
		binding( 0, textures[ "Blue Noise" ], GL_RGBA8UI ),
		binding( 1, textures[ "Accumulator" ], GL_RGBA16F ),
		binding( 2, textures[ "Color Block Back" ], GL_RGBA8UI ),
		binding( 3, textures[ "Lighting Block" ], GL_RGBA16F )
	} );

	// only one set required for postprocessing
	bindSets[ "Postprocessing" ] = bindSet( {
		binding( 0, textures[ "Blue Noise" ], GL_RGBA8UI ),
		binding( 1, textures[ "Accumulator" ], GL_RGBA16F ),
		binding( 2, textures[ "Display Texture" ], GL_RGBA8UI )
	} );

	bindSets[ "Basic Operation" ] = bindSet( {
		binding( 0, textures[ "Color Block Front" ], GL_RGBA8UI ),
		binding( 1, textures[ "Color Block Back" ], GL_RGBA8UI ),
		binding( 2, textures[ "Mask Block Front" ], GL_R8UI ),
		binding( 3, textures[ "Mask Block Back" ], GL_R8UI )
	} );

	bindSets[ "Basic Operation Back Set" ] = bindSet( {
		binding( 0, textures[ "Color Block Back" ], GL_RGBA8UI ),
		binding( 1, textures[ "Color Block Front" ], GL_RGBA8UI ),
		binding( 2, textures[ "Mask Block Back" ], GL_R8UI ),
		binding( 3, textures[ "Mask Block Front" ], GL_R8UI )
	} );

// this pair is for the data masking
	bindSets[ "Basic Operation With Lighting" ] = bindSet( {
		binding( 0, textures[ "Color Block Front" ], GL_RGBA8UI ),
		binding( 1, textures[ "Color Block Back" ], GL_RGBA8UI ),
		binding( 2, textures[ "Mask Block Front" ], GL_R8UI ),
		binding( 3, textures[ "Mask Block Back" ], GL_R8UI ),
		binding( 4, textures[ "Lighting Block" ], GL_RGBA16F )
	} );

	bindSets[ "Basic Operation With Lighting Back Set" ] = bindSet( {
		binding( 0, textures[ "Color Block Back" ], GL_RGBA8UI ),
		binding( 1, textures[ "Color Block Front" ], GL_RGBA8UI ),
		binding( 2, textures[ "Mask Block Back" ], GL_R8UI ),
		binding( 3, textures[ "Mask Block Front" ], GL_R8UI ),
		binding( 4, textures[ "Lighting Block" ], GL_RGBA16F )
	} );

	bindSets[ "Lighting Operation" ] = bindSet( {
		binding( 0, textures[ "Color Block Back" ], GL_RGBA8UI ),
		binding( 1, textures[ "Color Block Front" ], GL_RGBA8UI ),
		binding( 2, textures[ "Mask Block Back" ], GL_R8UI ),
		binding( 3, textures[ "Mask Block Front" ], GL_R8UI ),
		binding( 4, textures[ "Lighting Block" ], GL_RGBA16F ),
		binding( 5, textures[ "Blue Noise" ], GL_RGBA8UI )
	} );

	bindSets[ "Lighting Operation Back Set" ] = bindSet( {
		binding( 0, textures[ "Color Block Front" ], GL_RGBA8UI ),
		binding( 1, textures[ "Color Block Back" ], GL_RGBA8UI ),
		binding( 2, textures[ "Mask Block Front" ], GL_R8UI ),
		binding( 3, textures[ "Mask Block Back" ], GL_R8UI ),
		binding( 4, textures[ "Lighting Block" ], GL_RGBA16F ),
		binding( 5, textures[ "Blue Noise" ], GL_RGBA8UI )
	} );

	// other sets - some operations will reqiure a different configuration
		// heightmap needs heightmap

	cout << T_GREEN << "done." << T_RED << " ( " << Tock() << " us )" << RESET << endl;
}

void engine::ShaderCompile () {
	ZoneScoped;
	Tick();
	cout << T_BLUE << "    Compiling Shaders" << RESET << " ................................ ";

	const string base( "resources/engineCode/shaders/" );

	// main display blit
	shaders[ "Display" ] = regularShader( base + "blit.vs.glsl", base + "blit.fs.glsl" ).shaderHandle;

	// something to put data in the accumulator texture
	shaders[ "Dummy Draw" ] = computeShader( base + "dummyDraw.cs.glsl" ).shaderHandle;

	// operations
	shaders[ "AABB" ] = computeShader( base + "operations/AABB.cs.glsl" ).shaderHandle;
	shaders[ "Ambient Occlusion" ] = computeShader( base + "lighting/ambientOcclusion.cs.glsl" ).shaderHandle;
	shaders[ "Box Blur" ] = computeShader( base + "operations/boxBlur.cs.glsl" ).shaderHandle;
	shaders[ "Gaussian Blur" ] = computeShader( base + "operations/gaussBlur.cs.glsl" ).shaderHandle;
	shaders[ "Clear" ] = computeShader( base + "operations/clear.cs.glsl" ).shaderHandle;
	shaders[ "Cylinder" ] = computeShader( base + "operations/cylinder.cs.glsl" ).shaderHandle;
	shaders[ "Data Mask" ] = computeShader( base + "operations/dataMask.cs.glsl" ).shaderHandle;
	shaders[ "Ellipsoid" ] = computeShader( base + "operations/ellipsoid.cs.glsl" ).shaderHandle;
	shaders[ "Grid" ] = computeShader( base + "operations/grid.cs.glsl" ).shaderHandle;
	shaders[ "Light Clear" ] = computeShader( base + "lighting/clear.cs.glsl" ).shaderHandle;
	shaders[ "Light Mash" ] = computeShader( base + "lighting/mash.cs.glsl" ).shaderHandle;
	shaders[ "Mask Invert" ] = computeShader( base + "operations/maskInvert.cs.glsl" ).shaderHandle;
	shaders[ "Mask Clear" ] = computeShader( base + "operations/maskClear.cs.glsl" ).shaderHandle;
	shaders[ "Sphere" ] = computeShader( base + "operations/sphere.cs.glsl" ).shaderHandle;
	shaders[ "Triangle" ] = computeShader( base + "operations/triangle.cs.glsl" ).shaderHandle;

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

	cout << T_GREEN << "done." << T_RED << " ( " << Tock() << " us )" << RESET << endl;

}

void engine::ImguiSetup () {
	ZoneScoped;
	Tick();
	cout << T_BLUE << "    Configuring dearImGUI" << RESET << " ............................ ";

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();

	// font config
	defaultFont = io.Fonts->AddFontDefault();
	titleFont = io.Fonts->AddFontFromFileTTF( "resources/fonts/ttf/Cinzel-VariableFont_wght.ttf", 26 );

	// enable docking
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	// Setup Platform/Renderer bindings
	ImGui_ImplSDL2_InitForOpenGL( windowHandler.window, windowHandler.GLcontext );
	const char *glslVersion = "#version 430";
	ImGui_ImplOpenGL3_Init( glslVersion );

	// initial value for clear color
	ImVec4 *colors = ImGui::GetStyle().Colors;
	colors[ ImGuiCol_Text ]						= ImVec4( 0.670f, 0.500f, 0.160f, 1.000f );
	colors[ ImGuiCol_TextDisabled ]				= ImVec4( 0.330f, 0.270f, 0.160f, 1.000f );
	colors[ ImGuiCol_WindowBg ]					= ImVec4( 0.100f, 0.050f, 0.000f, 1.000f );
	colors[ ImGuiCol_ChildBg ]					= ImVec4( 0.230f, 0.170f, 0.020f, 0.050f );
	colors[ ImGuiCol_PopupBg ]					= ImVec4( 0.300f, 0.120f, 0.060f, 0.940f );
	colors[ ImGuiCol_Border ]					= ImVec4( 0.250f, 0.180f, 0.090f, 0.330f );
	colors[ ImGuiCol_BorderShadow ]				= ImVec4( 0.330f, 0.150f, 0.020f, 0.170f );
	colors[ ImGuiCol_FrameBg ]					= ImVec4( 0.561f, 0.082f, 0.040f, 0.170f );
	colors[ ImGuiCol_FrameBgHovered ]			= ImVec4( 0.190f, 0.090f, 0.020f, 0.170f );
	colors[ ImGuiCol_FrameBgActive ]			= ImVec4( 0.250f, 0.120f, 0.010f, 0.780f );
	colors[ ImGuiCol_TitleBg ]					= ImVec4( 0.250f, 0.120f, 0.010f, 1.000f );
	colors[ ImGuiCol_TitleBgActive ]			= ImVec4( 0.330f, 0.150f, 0.020f, 1.000f );
	colors[ ImGuiCol_TitleBgCollapsed ]			= ImVec4( 0.250f, 0.120f, 0.010f, 1.000f );
	colors[ ImGuiCol_MenuBarBg ]				= ImVec4( 0.140f, 0.070f, 0.020f, 1.000f );
	colors[ ImGuiCol_ScrollbarBg ]				= ImVec4( 0.130f, 0.100f, 0.080f, 0.530f );
	colors[ ImGuiCol_ScrollbarGrab ]			= ImVec4( 0.250f, 0.120f, 0.010f, 0.780f );
	colors[ ImGuiCol_ScrollbarGrabHovered ]		= ImVec4( 0.330f, 0.150f, 0.020f, 1.000f );
	colors[ ImGuiCol_ScrollbarGrabActive ]		= ImVec4( 0.250f, 0.120f, 0.010f, 0.780f );
	colors[ ImGuiCol_CheckMark ]				= ImVec4( 0.690f, 0.450f, 0.110f, 1.000f );
	colors[ ImGuiCol_SliderGrab ]				= ImVec4( 0.280f, 0.180f, 0.060f, 1.000f );
	colors[ ImGuiCol_SliderGrabActive ]			= ImVec4( 0.360f, 0.220f, 0.060f, 1.000f );
	colors[ ImGuiCol_Button ]					= ImVec4( 0.250f, 0.120f, 0.010f, 0.780f );
	colors[ ImGuiCol_ButtonHovered ]			= ImVec4( 0.330f, 0.150f, 0.020f, 1.000f );
	colors[ ImGuiCol_ButtonActive ]				= ImVec4( 0.250f, 0.120f, 0.010f, 0.780f );
	colors[ ImGuiCol_Header ]					= ImVec4( 0.250f, 0.120f, 0.010f, 0.780f );
	colors[ ImGuiCol_HeaderHovered ]			= ImVec4( 0.330f, 0.150f, 0.020f, 1.000f );
	colors[ ImGuiCol_HeaderActive ]				= ImVec4( 0.250f, 0.120f, 0.010f, 0.780f );
	colors[ ImGuiCol_Separator ]				= ImVec4( 0.280f, 0.180f, 0.060f, 0.370f );
	colors[ ImGuiCol_SeparatorHovered ]			= ImVec4( 0.330f, 0.150f, 0.020f, 0.170f );
	colors[ ImGuiCol_SeparatorActive ]			= ImVec4( 0.420f, 0.180f, 0.060f, 0.170f );
	colors[ ImGuiCol_ResizeGrip ]				= ImVec4( 0.250f, 0.120f, 0.010f, 0.780f );
	colors[ ImGuiCol_ResizeGripHovered ]		= ImVec4( 0.330f, 0.150f, 0.020f, 1.000f );
	colors[ ImGuiCol_ResizeGripActive ]			= ImVec4( 0.250f, 0.120f, 0.010f, 0.780f );
	colors[ ImGuiCol_Tab ]						= ImVec4( 0.250f, 0.120f, 0.010f, 0.780f );
	colors[ ImGuiCol_TabHovered ]				= ImVec4( 0.330f, 0.150f, 0.020f, 1.000f );
	colors[ ImGuiCol_TabActive ]				= ImVec4( 0.340f, 0.140f, 0.010f, 1.000f );
	colors[ ImGuiCol_TabUnfocused ]				= ImVec4( 0.330f, 0.150f, 0.020f, 1.000f );
	colors[ ImGuiCol_TabUnfocusedActive ]		= ImVec4( 0.420f, 0.180f, 0.060f, 1.000f );
	colors[ ImGuiCol_PlotLines ]				= ImVec4( 0.610f, 0.610f, 0.610f, 1.000f );
	colors[ ImGuiCol_PlotLinesHovered ]			= ImVec4( 1.000f, 0.430f, 0.350f, 1.000f );
	colors[ ImGuiCol_PlotHistogram ]			= ImVec4( 0.900f, 0.700f, 0.000f, 1.000f );
	colors[ ImGuiCol_PlotHistogramHovered ]		= ImVec4( 1.000f, 0.600f, 0.000f, 1.000f );
	colors[ ImGuiCol_TextSelectedBg ]			= ImVec4( 0.060f, 0.030f, 0.010f, 0.780f );
	colors[ ImGuiCol_DragDropTarget ]			= ImVec4( 0.640f, 0.420f, 0.090f, 0.900f );
	colors[ ImGuiCol_NavHighlight ]				= ImVec4( 0.640f, 0.420f, 0.090f, 0.900f );
	colors[ ImGuiCol_NavWindowingHighlight ]	= ImVec4( 1.000f, 1.000f, 1.000f, 0.700f );
	colors[ ImGuiCol_NavWindowingDimBg ]		= ImVec4( 0.800f, 0.800f, 0.800f, 0.200f );
	colors[ ImGuiCol_ModalWindowDimBg ]			= ImVec4( 0.800f, 0.800f, 0.800f, 0.350f );

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
	ZoneScoped;
	cout << endl << T_CYAN << "  " << shaders.size() << " shaders." << endl;
	cout << "  " << textures.size() << " textures." << endl;
	cout << T_YELLOW << "  Startup is complete ( total " << TotalTime() << " us )" << RESET << endl << endl;
}
