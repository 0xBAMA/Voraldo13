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

	// ImGui::SetNextWindowSize( ImVec2( 500, 440 ), ImGuiCond_FirstUseEver );
	auto windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar;
	if ( ImGui::Begin( "Menu layout", p_open, windowFlags ) ) {

		// I'd like to have a reason to have a menu
		if ( ImGui::BeginMenuBar() ) {
			if ( ImGui::BeginMenu( "File" ) ) {
				if ( ImGui::MenuItem( "Close" ) ) *p_open = false;

				// probably add swapBlocks() here - this would be a nice, since it is globally accessible

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

// =============================================================================
// Left Side
// =============================================================================
		static int currentlySelected = -1;
		{
			ImGui::BeginChild( "TreeView", ImVec2( 185, 0 ), true );

			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;
			int current = 0;

			// remove some redundancy
			#define COLLAPSING_SECTION(labelString,x) \
				if ( ImGui::CollapsingHeader( labelString, flags ) ) { \
					while ( menu.entries[ current ].category == x ) { \
						std::string label = std::string( "  " ) + menu.entries[ current ].label; \
						if ( ImGui::Selectable( label.c_str(), currentlySelected == current ) ) { \
							currentlySelected = current; \
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
		At this stage, the currently selected option is currentlySelected - this can
	be used to index into the menu.entries[] array, to get any menu layout
	information that will be relevant for this particular menu entry.

		Special reserve value -1 used for a welcome splash screen, shown on startup.
============================================================================= */
		{


			ImGui::BeginGroup();
			ImGui::BeginChild("Contents", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
			// ImGui::Text("MyObject: %d", currentlySelected);

			if ( currentlySelected == -1 ) {
				// splash
				OrangeText( " Welcome To Voraldo 13" );


			} else if ( !menu.entries[ currentlySelected ].requiresSpecialHandling ) {
				// parse list of layout elements in the menu entry
				for ( auto& interfaceElement : menu.entries[ currentlySelected ].interfaceElements ) {

				}
			} else {
				// indicates that this section requires special handling / manual layout
				if ( 0 ) {

				} else if ( menu.entries[ currentlySelected ].label == string( "Application" ) ) {
					FillApplicationSettings();
				} else if ( menu.entries[ currentlySelected ].label == string( "Rendering" ) ) {
					FillRenderingSettings();
				} else if ( menu.entries[ currentlySelected ].label == string( "Post Processing" ) ) {
					FillPostProcessingSettings();
				}
			}

			ImGui::EndChild();

			// buttons at the bottom of the page
			if ( ImGui::Button( "Revert" ) ) {

			}
			ImGui::SameLine();
			if ( ImGui::Button( "Save" ) ) {

			}
			ImGui::EndGroup();
		}
	}
	ImGui::End();
}

void engine::FillApplicationSettings() {
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

	// etc
	ImGui::Unindent( 16.0f );
}

void engine::FillRenderingSettings () {
	OrangeText( " Rendering Settings" );
	ImGui::Separator();
	ImGui::Indent( 16.0f );
	ImGui::SliderFloat( "Alpha Correction Power", &render.alphaCorrectionPower, 0.0f, 4.0f );
	ImGui::SliderFloat( "Jitter Amount", &render.jitterAmount, 0.0f, 2.0f );
	ImGui::SliderFloat( "Perspective", &render.perspective, -4.0f, 4.0f );
	ImGui::SliderInt( "Volume Steps", &render.volumeSteps, 0, 1400 );
	// picker for render mode shader
	ImGui::SliderInt( "History Frames", &render.numFramesHistory, 0, 14 );
	ImGui::Unindent( 16.0f );
}

void engine::FillPostProcessingSettings () {
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
	ImGui_ImplSDL2_NewFrame( window );
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
