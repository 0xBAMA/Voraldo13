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

// this needs a formatting pass
void engine::MenuLayout( bool* p_open ) {

	// ImGui::SetNextWindowSize( ImVec2( 500, 440 ), ImGuiCond_FirstUseEver );
	auto windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar;
	if ( ImGui::Begin( "Menu layout", p_open, windowFlags ) ) {

		// I'd like to have a reason to have a menu
		if ( ImGui::BeginMenuBar() ) {
			if ( ImGui::BeginMenu( "File" ) ) {
				if ( ImGui::MenuItem( "Close" ) ) *p_open = false;
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
		#define COLLAPSING_SECTION(label,x) \
			if ( ImGui::CollapsingHeader( label, flags ) ) { \
				ImGui::Indent( 16.0f ); \
				while ( menu.entries[ current ].category == x ) { \
					if ( ImGui::Selectable( menu.entries[ current ].label.c_str(), currentlySelected == current ) ) { \
						currentlySelected = current; \
					} \
					current++; \
				} \
				ImGui::Unindent( 16.0f ); \
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
			if ( !menu.entries[ currentlySelected ].requiresSpecialHandling ) {
				// parse list of layout elements
			} else {
				// if blablah
					// do the imgui layout
			}


			ImGui::BeginGroup();
			ImGui::BeginChild("Contents", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
			ImGui::Text("MyObject: %d", currentlySelected);
			// ImGui::Separator();
			// if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None)) {
			// 	if (ImGui::BeginTabItem("Description")) {
			// 		ImGui::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. ");
			// 		ImGui::EndTabItem();
			// 	}
			// 	if (ImGui::BeginTabItem("Details")) {
			// 		ImGui::Text("ID: 0123456789");
			// 		ImGui::EndTabItem();
			// 	}
			// 	ImGui::EndTabBar();
			// }
			ImGui::EndChild();
			if (ImGui::Button("Revert")) {}
			ImGui::SameLine();
			if (ImGui::Button("Save")) {}
			ImGui::EndGroup();
		}
	}
	ImGui::End();
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


// this will eventually move to the settings section of the main menu
void engine::TonemapControlsWindow () {
	ImGui::Begin( "Tonemapping Controls", NULL, 0 );

	const char* tonemapModesList[] = {
		"None (Linear)",
		"ACES (Narkowicz 2015)",
		"Unreal Engine 3",
		"Unreal Engine 4",
		"Uncharted 2",
		"Gran Turismo",
		"Modified Gran Turismo",
		"Rienhard",
		"Modified Rienhard",
		"jt",
		"robobo1221s",
		"robo",
		"reinhardRobo",
		"jodieRobo",
		"jodieRobo2",
		"jodieReinhard",
		"jodieReinhard2"
	};
	ImGui::Combo( "Tonemapping Mode", &tonemap.tonemapMode, tonemapModesList, IM_ARRAYSIZE( tonemapModesList ) );
	ImGui::SliderFloat( "Gamma", &tonemap.gamma, 0.0f, 3.0f );
	ImGui::SliderFloat( "Color Temperature", &tonemap.colorTemp, 1000.0f, 40000.0f, "%.2f", ImGuiSliderFlags_Logarithmic );

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
