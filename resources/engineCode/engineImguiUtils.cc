#include "engine.h"

void engine::QuitConf ( bool *open ) {
	if ( *open ) {
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos( center, 0, ImVec2( 0.5f, 0.5f ) );
		ImGui::SetNextWindowSize( ImVec2( 225, 45 ) );
		ImGui::OpenPopup( "Quit Confirm" );
		if ( ImGui::BeginPopupModal( "Quit Confirm", NULL, ImGuiWindowFlags_NoDecoration ) ) {
			ImGui::Text( "Are you sure you want to quit?" );
			ImGui::Text( "  " );
			ImGui::SameLine();
			// button to cancel -> set this window's bool to false
			if ( ImGui::Button( " Cancel " ) )
				*open = false;
			ImGui::SameLine();
			ImGui::Text("      ");
			ImGui::SameLine();
			// button to quit -> set pquit to true
			if ( ImGui::Button( " Quit " ) )
				pQuit = true;
		}
	}
}

static void HelpMarker ( const char *desc ) {
	ImGui::TextDisabled( "(?)" );
	if ( ImGui::IsItemHovered() ) {
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos( ImGui::GetFontSize() * 35.0f );
		ImGui::TextUnformatted( desc );
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

static void OrangeText ( const char *string ) {
	ImGui::Separator();
	ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.75, 0.35, 0.1, 1.0 ) );
	ImGui::TextUnformatted( string );
	ImGui::PopStyleColor();
}

// this needs a formatting pass
void engine::MenuLayout( bool* p_open ) {
	const auto flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar;
	if ( ImGui::Begin( "Menu layout", p_open, flags ) ) {
		if ( ImGui::BeginMenuBar() ) {
			if ( ImGui::BeginMenu( "File" ) ) {
				if ( ImGui::MenuItem( "Close" ) )
					*p_open = false;

				ImGui::EndMenu();
			} // else if ( ImGui::MenuItem( "Swap Blocks" ) ) {
			// 	cout << "swapping blocks" << endl; // this is interesting, menu with no contents used as button
			// 	ImGui::EndMenu(); // some shitty artifacts so maybe don't do this
			// }
			ImGui::EndMenuBar();
		}

// =============================================================================
// Left Side shows all the menu entries from menu.entries array
// =============================================================================
		{
			ImGui::BeginChild( "TreeView", ImVec2( 185, 0 ), true );
			int current = 0;

			#define COLLAPSING_SECTION(labelString,x) \
				if ( ImGui::CollapsingHeader( labelString ) ) { \
					while ( menu.entries[ current ].category == x ) { \
						std::string label = std::string( "  " ) + menu.entries[ current ].label; \
						if ( ImGui::Selectable( label.c_str(), currentlySelectedMenuItem == current ) ) { \
							currentlySelectedMenuItem = current; \
						} \
						current++; \
					} \
				} else { /* if collapsed, bump current to compensate */ \
					while ( menu.entries[ current ].category == x ) { \
						current++; \
					} \
				}

			COLLAPSING_SECTION( "Shapes", category_t::shapes );
			COLLAPSING_SECTION( "Utilities", category_t::utilities );
			COLLAPSING_SECTION( "Lighting", category_t::lighting );
			COLLAPSING_SECTION( "Settings", category_t::settings );

			#undef COLLAPSING_SECTION // only needed in this scope
			ImGui::EndChild();
		}
		ImGui::SameLine();


/* =============================================================================
	Right Side
		At this stage, the currently selected option is currentlySelectedMenuItem - this can
	be used to index into the menu.entries[] array, to get any menu layout
	information that will be relevant for this particular menu entry.

		Special reserve value -1 used for a welcome splash screen, shown on startup.
============================================================================= */
		{
			ImGui::BeginGroup();
			ImGui::BeginChild("Contents", ImVec2( 0, -ImGui::GetFrameHeightWithSpacing() ) );
			#define isPicked(x) menu.entries[currentlySelectedMenuItem].label==string(x)
			if ( currentlySelectedMenuItem == -1 ) {
				MenuSplash();
			} else if ( isPicked( "AABB" ) ) {
				MenuAABB();
			} else if ( isPicked( "Cylinder/Tube" ) ) {
				MenuCylinderTube();
			} else if ( isPicked( "Ellipsoid" ) ) {
				MenuEllipsoid();
			} else if ( isPicked( "Grid" ) ) {
				MenuGrid();
			} else if ( isPicked( "Heightmap" ) ) {
				MenuHeightmap();
			} else if ( isPicked( "Icosahedron" ) ) {
				MenuIcosahedron();
			} else if ( isPicked( "Noise" ) ) {
				MenuNoise();
			} else if ( isPicked( "Sphere" ) ) {
				MenuSphere();
			} else if ( isPicked( "Triangle" ) ) {
				MenuTriangle();
			} else if ( isPicked( "User Shader" ) ) {
				MenuUserShader();
			} else if ( isPicked( "VAT" ) ) {
				MenuVAT();
			} else if ( isPicked( "Spaceship Generator" ) ) {
				MenuSpaceship();
			} else if ( isPicked( "Letters" ) ) {
				MenuLetters();
			} else if ( isPicked( "XOR" ) ) {
				MenuXOR();
			} else if ( isPicked( "Clear" ) ) {
				MenuClearBlock();
			} else if ( isPicked( "Masking" ) ) {
				MenuMasking();
			} else if ( isPicked( "Blur" ) ) {
				MenuBlur();
			} else if ( isPicked( "Shift/Trim" ) ) {
				MenuShiftTrim();
			} else if ( isPicked( "Load/Save" ) ) {
				MenuLoadSave();
			} else if ( isPicked( "Limiter/Compressor" ) ) {
				MenuLimiterCompressor();
			} else if ( isPicked( "Copy/Paste" ) ) {
				MenuCopyPaste();
			} else if ( isPicked( "Logging" ) ) {
				MenuLogging();
			} else if ( isPicked( "Screenshot" ) ) {
				MenuScreenshot();
			} else if ( isPicked( "Clear Levels" ) ) {
				MenuClearLightLevels();
			} else if ( isPicked( "Point Light" ) ) {
				MenuPointLight();
			} else if ( isPicked( "Cone Light" ) ) {
				MenuConeLight();
			} else if ( isPicked( "Directional Light" ) ) {
				MenuDirectionalLight();
			} else if ( isPicked( "Fake GI" ) ) {
				MenuFakeGI();
			} else if ( isPicked( "Ambient Occlusion" ) ) {
				MenuAmbientOcclusion();
			} else if ( isPicked( "Mash" ) ) {
				MenuLightMash();
			} else if ( isPicked( "Application Settings" ) ) {
				MenuApplicationSettings();
			} else if ( isPicked( "Rendering Settings" ) ) {
				MenuRenderingSettings();
			} else if ( isPicked( "Post Processing" ) ) {
				MenuPostProcessingSettings();
			}
			#undef isPicked
			ImGui::EndChild();
			ImGui::EndGroup();
		}
	}
	ImGui::End();
}
void engine::MenuSplash () {
	// splash - add any more information to present upon opening
	// maybe learn how to use the imgui draw lists? tbd, something with an eye could be cool
	OrangeText( " Welcome To Voraldo 13" );
}

void engine::MenuAABB () {
	OrangeText( "AABB" );
	ImGui::Separator();
	ImGui::Indent( 16.0f );
	ImGui::Unindent( 16.0f );
}

void engine::MenuCylinderTube () {
	OrangeText( "Cylinder/Tube" );
	ImGui::Separator();
	ImGui::Indent( 16.0f );
	ImGui::Unindent( 16.0f );
}

void engine::MenuEllipsoid () {
	OrangeText( "Ellipsoid" );
	ImGui::Separator();
	ImGui::Indent( 16.0f );
	ImGui::Unindent( 16.0f );
}

void engine::MenuGrid () {
	OrangeText( "Regular Grid" );
	ImGui::Separator();
	ImGui::Indent( 16.0f );
	ImGui::Unindent( 16.0f );
}

void engine::MenuHeightmap () {
	OrangeText( "Heightmap" );
	ImGui::Separator();
	ImGui::Indent( 16.0f );
	ImGui::Unindent( 16.0f );
}

void engine::MenuIcosahedron () {
	OrangeText( "Icosahedron" );
	ImGui::Separator();
	ImGui::Indent( 16.0f );
	ImGui::Unindent( 16.0f );
}

void engine::MenuNoise () {
	OrangeText( "Noise" );
	ImGui::Separator();
	ImGui::Indent( 16.0f );
	ImGui::Unindent( 16.0f );
}

void engine::MenuSphere () {
	OrangeText( "Sphere" );
	ImGui::Separator();
	ImGui::Indent( 16.0f );
	ImGui::Unindent( 16.0f );
}

void engine::MenuTriangle () {
	OrangeText( "Triangle" );
	ImGui::Separator();
	ImGui::Indent( 16.0f );
	ImGui::Unindent( 16.0f );
}

void engine::MenuUserShader () {
	OrangeText( "User Shader" );
	ImGui::Separator();
	ImGui::Indent( 16.0f );
	ImGui::Unindent( 16.0f );
}

void engine::MenuVAT () {
	OrangeText( "Voxel Automata Terrain" );
	ImGui::Separator();
	ImGui::Indent( 16.0f );
	ImGui::Unindent( 16.0f );
}

void engine::MenuSpaceship () {
	OrangeText( "Spaceship Generator" );
	ImGui::Separator();
	ImGui::Indent( 16.0f );
	ImGui::Unindent( 16.0f );
}

void engine::MenuLetters () {
	OrangeText( "Letters" );
	ImGui::Separator();
	ImGui::Indent( 16.0f );
	ImGui::Unindent( 16.0f );
}

void engine::MenuXOR () {
	OrangeText( "XOR" );
	ImGui::Separator();
	ImGui::Indent( 16.0f );
	ImGui::Unindent( 16.0f );
}

void engine::MenuClearBlock () {
	OrangeText( "Clear Block" );
	ImGui::Separator();
	ImGui::Indent( 16.0f );
	ImGui::Unindent( 16.0f );
}

void engine::MenuMasking () {
	OrangeText( "Masking Operations" );
	ImGui::Separator();
	ImGui::Indent( 16.0f );
	ImGui::Unindent( 16.0f );
}

void engine::MenuBlur () {
	OrangeText( "Blur" );
	ImGui::Separator();
	ImGui::Indent( 16.0f );
	ImGui::Unindent( 16.0f );
}

void engine::MenuShiftTrim () {
	OrangeText( "Shift/Trim" );
	ImGui::Separator();
	ImGui::Indent( 16.0f );
	ImGui::Unindent( 16.0f );
}

void engine::MenuLoadSave () {
	OrangeText( "Load/Save" );
	ImGui::Separator();
	ImGui::Indent( 16.0f );
	ImGui::Unindent( 16.0f );
}

void engine::MenuLimiterCompressor () {
	OrangeText( "Limiter/Compressor" );
	ImGui::Separator();
	ImGui::Indent( 16.0f );
	ImGui::Unindent( 16.0f );
}

void engine::MenuCopyPaste () {
	OrangeText( "Copy/Paste" );
	ImGui::Separator();
	ImGui::Indent( 16.0f );
	ImGui::Unindent( 16.0f );
}

void engine::MenuLogging () {
	OrangeText( "Logging" );
	ImGui::Separator();
	ImGui::Indent( 16.0f );
	ImGui::Unindent( 16.0f );
}

void engine::MenuScreenshot () {
	OrangeText( "Screenshot" );
	ImGui::Separator();
	ImGui::Indent( 16.0f );
	ImGui::Unindent( 16.0f );
}

void engine::MenuClearLightLevels () {
	OrangeText( "Clear Light Levels" );
	ImGui::Separator();
	ImGui::Indent( 16.0f );
	ImGui::Unindent( 16.0f );
}

void engine::MenuPointLight () {
	OrangeText( "Point Light" );
	ImGui::Separator();
	ImGui::Indent( 16.0f );
	ImGui::Unindent( 16.0f );
}

void engine::MenuConeLight () {
	OrangeText( "Cone Light" );
	ImGui::Separator();
	ImGui::Indent( 16.0f );
	ImGui::Unindent( 16.0f );
}

void engine::MenuDirectionalLight () {
	OrangeText( "Directional Light" );
	ImGui::Separator();
	ImGui::Indent( 16.0f );
	ImGui::Unindent( 16.0f );
}

void engine::MenuFakeGI () {
	OrangeText( "Fake Global Illumination" );
	ImGui::Separator();
	ImGui::Indent( 16.0f );
	ImGui::Unindent( 16.0f );
}

void engine::MenuAmbientOcclusion () {
	OrangeText( "Ambient Occlusion" );
	ImGui::Separator();
	ImGui::Indent( 16.0f );
	ImGui::Unindent( 16.0f );
}

void engine::MenuLightMash () {
	OrangeText( "Light Mash" );
	ImGui::Separator();
	ImGui::Indent( 16.0f );
	ImGui::Unindent( 16.0f );
}

void engine::MenuApplicationSettings() {
	OrangeText( " Application Settings" );
	ImGui::Separator();
	ImGui::Indent( 16.0f );

	// add settings for window
	// change which screen it's displayed on
		// toggle fullscreen/windowed
		// can you change bordered/borderless at runtime?
			// if not, maybe de-init and re-init

	// maybe look at doing some more versions of this? might be interesting
	const char* tridentModesList[] = { "Fractal", "Spherical" };
	ImGui::Combo( "Trident Mode", &trident.modeSelect, tridentModesList, IM_ARRAYSIZE( tridentModesList ) );
	ImGui::Checkbox( "Show Trident", &render.showTrident );
	ImGui::Checkbox( "Show Timing", &render.showTiming );

	// etc
	ImGui::Unindent( 16.0f );
}

void engine::MenuRenderingSettings () {
	OrangeText( " Rendering Settings" );
	ImGui::Separator();
	ImGui::Indent( 16.0f );
	ImGui::SliderFloat( "Alpha Correction Power", &render.alphaCorrectionPower, 0.0f, 4.0f );
	ImGui::SliderFloat( "Jitter Amount", &render.jitterAmount, 0.0f, 2.0f );
	ImGui::SliderFloat( "Perspective", &render.perspective, -4.0f, 4.0f );
	ImGui::SliderFloat( "Scale", &render.scaleFactor, 0.0f, 40.0f );
	ImGui::SliderFloat( "Blend Factor", &render.blendFactor, 0.0f, 1.0f );
	ImGui::SliderInt( "Volume Steps", &render.volumeSteps, 0, 1400 );
	// picker for render mode shader
	ImGui::SliderInt( "History Frames", &render.numFramesHistory, 0, 14 );
	ImGui::Unindent( 16.0f );
}

void engine::MenuPostProcessingSettings () {
	OrangeText( "Post Process Settings" );
	ImGui::Separator();
	ImGui::Indent( 16.0f );

	const char* tonemapModesList[] = { "None (Linear)", "ACES (Narkowicz 2015)", "Unreal Engine 3",
		"Unreal Engine 4", "Uncharted 2", "Gran Turismo", "Modified Gran Turismo", "Rienhard",
		"Modified Rienhard", "jt", "robobo1221s", "robo", "reinhardRobo", "jodieRobo", "jodieRobo2",
		"jodieReinhard", "jodieReinhard2" };

	ImGui::SliderFloat( "Gamma", &tonemap.gamma, 0.0f, 3.0f );
	ImGui::SliderFloat( "Color Temperature", &tonemap.colorTemp, 1000.0f, 40000.0f, "%.2f", ImGuiSliderFlags_Logarithmic );
	ImGui::Combo( "Tonemapping Mode", &tonemap.tonemapMode, tonemapModesList, IM_ARRAYSIZE( tonemapModesList ) );

	// add dither controls
		// methodology picker - quantize, palette
		// color space picker ( quantize or distance metric for palette )
		// if mode is quantize, give control over bit depth + quantize method ( exponential / bitcrush )
		// if mode is palette, give a picker for the palette

	ImGui::Unindent( 16.0f );
}


void engine::DrawTextEditor () {
	ImGui::Begin( "Editor", NULL, 0 );
	static TextEditor editor;
	// static auto lang = TextEditor::LanguageDefinition::CPlusPlus();
	static auto lang = TextEditor::LanguageDefinition::GLSL();
	editor.SetLanguageDefinition( lang );

	auto cpos = editor.GetCursorPosition();
	// editor.SetPalette( TextEditor::GetLightPalette() );
	editor.SetPalette( TextEditor::GetDarkPalette() );
	// editor.SetPalette( TextEditor::GetRetroBluePalette() );

	static bool loaded = false;
	static const char *fileToEdit = "resources/engineCode/shaders/blit.vs.glsl";
	if ( !loaded ) {
		std::ifstream t ( fileToEdit );
		editor.SetLanguageDefinition( lang );
		if ( t.good() ) {
			editor.SetText( std::string( ( std::istreambuf_iterator< char >( t ) ), std::istreambuf_iterator< char >() ) );
			loaded = true;
		}
	}

	// add dropdown for different shaders?
	ImGui::Text( "%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1,
							cpos.mColumn + 1, editor.GetTotalLines(),
							editor.IsOverwrite() ? "Ovr" : "Ins",
							editor.CanUndo() ? "*" : " ",
							editor.GetLanguageDefinition().mName.c_str(), fileToEdit );

	editor.Render( "Editor" );
	HelpMarker( "dummy helpmarker to get rid of unused warning" );
	ImGui::End();
}

void engine::ImguiFrameStart () {
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame( windowHandler.window );
	ImGui::NewFrame();
}

void engine::ImguiFrameEnd () {
	// get it ready to put on the screen
	ImGui::Render();

	// put imgui data into the framebuffer
	ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );

	// platform windows ( pop out windows )
	ImGuiIO &io = ImGui::GetIO();
	if ( io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable ) {
		SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
		SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		SDL_GL_MakeCurrent( backup_current_window, backup_current_context );
	}
}
