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

void engine::OrangeText ( const char *string ) {
	ImGui::Separator();
	ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.75, 0.35, 0.1, 1.0 ) );
	ImGui::PushFont( titleFont );
	ImGui::TextUnformatted( string );
	ImGui::PopFont();
	ImGui::PopStyleColor();
}

void engine::MenuLayout( bool* p_open ) {
	const auto flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar;
	if ( ImGui::Begin( "Menu layout", p_open, flags ) ) {
		if ( ImGui::BeginMenuBar() ) {
			if ( ImGui::BeginMenu( "File" ) ) {
				if ( ImGui::MenuItem( "Swap Blocks" ) )
					SwapBlocks(); // one step of undo
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
		( Never uses -1 index to reference array because it is checked for first )
============================================================================= */
		{
			ImGui::BeginGroup();
			ImGui::BeginChild("Contents", ImVec2( 0, -ImGui::GetFrameHeightWithSpacing() ) );
			#define isPicked(x) menu.entries[currentlySelectedMenuItem].label==string(x)
			if ( currentlySelectedMenuItem == -1 ) {			MenuSplash();
			} else if ( isPicked( "AABB" ) ) {					MenuAABB();
			} else if ( isPicked( "Cylinder/Tube" ) ) {			MenuCylinderTube();
			} else if ( isPicked( "Ellipsoid" ) ) {				MenuEllipsoid();
			} else if ( isPicked( "Grid" ) ) {					MenuGrid();
			} else if ( isPicked( "Heightmap" ) ) {				MenuHeightmap();
			} else if ( isPicked( "Icosahedron" ) ) {			MenuIcosahedron();
			} else if ( isPicked( "Noise" ) ) {					MenuNoise();
			} else if ( isPicked( "Sphere" ) ) {				MenuSphere();
			} else if ( isPicked( "Triangle" ) ) {				MenuTriangle();
			} else if ( isPicked( "User Shader" ) ) {			MenuUserShader();
			} else if ( isPicked( "VAT" ) ) {					MenuVAT();
			} else if ( isPicked( "Spaceship Generator" ) ) {	MenuSpaceship();
			} else if ( isPicked( "Letters" ) ) {				MenuLetters();
			} else if ( isPicked( "XOR" ) ) {					MenuXOR();
			} else if ( isPicked( "Clear" ) ) {					MenuClearBlock();
			} else if ( isPicked( "Masking" ) ) {				MenuMasking();
			} else if ( isPicked( "Blur" ) ) {					MenuBlur();
			} else if ( isPicked( "Shift/Trim" ) ) {			MenuShiftTrim();
			} else if ( isPicked( "Load/Save" ) ) {				MenuLoadSave();
			} else if ( isPicked( "Limiter/Compressor" ) ) {	MenuLimiterCompressor();
			} else if ( isPicked( "Copy/Paste" ) ) {			MenuCopyPaste();
			} else if ( isPicked( "Logging" ) ) {				MenuLogging();
			} else if ( isPicked( "Screenshot" ) ) {			MenuScreenshot();
			} else if ( isPicked( "Clear Levels" ) ) {			MenuClearLightLevels();
			} else if ( isPicked( "Point Light" ) ) {			MenuPointLight();
			} else if ( isPicked( "Cone Light" ) ) {			MenuConeLight();
			} else if ( isPicked( "Directional Light" ) ) {		MenuDirectionalLight();
			} else if ( isPicked( "Fake GI" ) ) {				MenuFakeGI();
			} else if ( isPicked( "Ambient Occlusion" ) ) {		MenuAmbientOcclusion();
			} else if ( isPicked( "Mash" ) ) {					MenuLightMash();
			} else if ( isPicked( "Application Settings" ) ) {	MenuApplicationSettings();
			} else if ( isPicked( "Rendering Settings" ) ) {	MenuRenderingSettings();
			} else if ( isPicked( "Post Processing" ) ) {		MenuPostProcessingSettings();
			} // add a stats tab, maybe? tbd
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

void engine::ColorPickerHelper ( bool& draw, int& mask, glm::vec4& color ) {
	ImGui::Separator();
	OrangeText("OPTIONS");
	ImGui::Checkbox( "  Draw ", &draw );
	ImGui::SameLine();
	ImGui::InputInt( " Mask ", &mask );
	// bounds check
	mask = std::clamp( mask, 0, 255 );
	ImGui::ColorEdit4( "  Color", ( float * ) &color, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreviewHalf );
}

void TitleText ( const char *string  ) {
	// TODO: add titles in a different font
}

void SetPosBottomRightCorner () {
	ImVec2 windowSize = ImGui::GetWindowSize();
	ImGui::SetCursorPos( ImVec2( windowSize.x - 150, windowSize.y - 18 ) );
}

void engine::MenuAABB () {
	OrangeText( "AABB" );
	ImGui::BeginTabBar( "aabb" );
	if ( ImGui::BeginTabItem( " Controls " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );

		static glm::ivec3 minCoords( 0, 0, 0 );
		static glm::ivec3 maxCoords( 0, 0, 0 );
		static bool draw = true;
		static int mask = 0;
		static glm::vec4 color( 0.0f );

		OrangeText( "EXTENTS" );
		ImGui::SliderInt( "Max X", &maxCoords.x, 0, BLOCKDIM );
		ImGui::SliderInt( "Max Y", &maxCoords.y, 0, BLOCKDIM );
		ImGui::SliderInt( "Max Z", &maxCoords.z, 0, BLOCKDIM );
		ImGui::Separator();
		ImGui::SliderInt( "Min X", &minCoords.x, 0, BLOCKDIM );
		ImGui::SliderInt( "Min Y", &minCoords.y, 0, BLOCKDIM );
		ImGui::SliderInt( "Min Z", &minCoords.z, 0, BLOCKDIM );
		ColorPickerHelper( draw, mask, color );

		SetPosBottomRightCorner();
		if ( ImGui::Button( "Invoke Operation" ) ) {
			// swap the front/back buffers
			SwapBlocks();

			// apply the bindset
			bindSets[ "Basic Operation" ].apply();

			// send the uniforms
			json j;
			j[ "shader" ] = "AABB";
			j[ "bindset" ] = "Basic Operation"; // redundant for now, but relevant for reuse?
			j[ "minCoords" ][ "type" ] = "ivec3";
			j[ "minCoords" ][ "x" ] = minCoords.x;
			j[ "minCoords" ][ "y" ] = minCoords.y;
			j[ "minCoords" ][ "z" ] = minCoords.z;
			j[ "maxCoords" ][ "type" ] = "ivec3";
			j[ "maxCoords" ][ "x" ] = maxCoords.x;
			j[ "maxCoords" ][ "y" ] = maxCoords.y;
			j[ "maxCoords" ][ "z" ] = maxCoords.z;
			j[ "draw" ][ "type" ] = "bool";
			j[ "draw" ][ "x" ] = draw;
			j[ "mask" ][ "type" ] = "int";
			j[ "mask" ][ "x" ] = mask;
			j[ "color" ][ "type" ] = "vec4";
			j[ "color" ][ "x" ] = color.r;
			j[ "color" ][ "y" ] = color.g;
			j[ "color" ][ "z" ] = color.b;
			j[ "color" ][ "w" ] = color.a;
			SendUniforms( j );
			AddToLog( j );

			// dispatch the compute shader
			BlockDispatch();
		}

		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	if ( ImGui::BeginTabItem( " Description " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );

		// TODO: DESCRIPTION OF AABB PRIMITIVE

		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	ImGui::EndTabBar();
}

void engine::MenuCylinderTube () {
	OrangeText( "Cylinder/Tube" );
	ImGui::BeginTabBar( "cylinder/tube" );
	if ( ImGui::BeginTabItem( " Controls " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );

		static glm::vec3 bottomVector ( 0.0f );
		static glm::vec3 topVector ( 0.0f );
		static float innerRadius = 0.0f;
		static float outerRadius = 0.0f;
		static bool draw = true;
		static int mask = 0;
		static glm::vec4 color( 0.0f );

		OrangeText( "Radii" );
		ImGui::SliderFloat( "Inner", &innerRadius, 0.0f, float( BLOCKDIM ), "%.3f" );
		ImGui::SliderFloat( "Outer", &outerRadius, 0.0f, float( BLOCKDIM ), "%.3f" );

		OrangeText( "Top Point" );
		ImGui::SliderFloat( "Top X", &topVector.x, 0.0f, float( BLOCKDIM ), "%.3f" );
		ImGui::SliderFloat( "Top Y", &topVector.y, 0.0f, float( BLOCKDIM ), "%.3f" );
		ImGui::SliderFloat( "Top Z", &topVector.z, 0.0f, float( BLOCKDIM ), "%.3f" );

		OrangeText( "Bottom Point" );
		ImGui::SliderFloat( "Bottom X", &bottomVector.x, 0.0f, float( BLOCKDIM ), "%.3f" );
		ImGui::SliderFloat( "Bottom Y", &bottomVector.y, 0.0f, float( BLOCKDIM ), "%.3f" );
		ImGui::SliderFloat( "Bottom Z", &bottomVector.z, 0.0f, float( BLOCKDIM ), "%.3f" );

		ColorPickerHelper( draw, mask, color );

		SetPosBottomRightCorner();
		if ( ImGui::Button( "Invoke Operation" ) ) {
			// swap the front/back buffers
			SwapBlocks();

			// apply the bindset
			bindSets[ "Basic Operation" ].apply();

			// send the uniforms
			json j;
			j[ "shader" ] = "Cylinder";
			j[ "bindset" ] = "Basic Operation";
			j[ "topVector" ][ "type" ] = "vec3";
			j[ "topVector" ][ "x" ] = topVector.x;
			j[ "topVector" ][ "y" ] = topVector.y;
			j[ "topVector" ][ "z" ] = topVector.z;
			j[ "bottomVector" ][ "type" ] = "vec3";
			j[ "bottomVector" ][ "x" ] = bottomVector.x;
			j[ "bottomVector" ][ "y" ] = bottomVector.y;
			j[ "bottomVector" ][ "z" ] = bottomVector.z;
			j[ "innerRadius" ][ "type" ] = "float";
			j[ "innerRadius" ][ "x" ] = innerRadius;
			j[ "outerRadius" ][ "type" ] = "float";
			j[ "outerRadius" ][ "x" ] = outerRadius;
			j[ "draw" ][ "type" ] = "bool";
			j[ "draw" ][ "x" ] = draw;
			j[ "mask" ][ "type" ] = "int";
			j[ "mask" ][ "x" ] = mask;
			j[ "color" ][ "type" ] = "vec4";
			j[ "color" ][ "x" ] = color.r;
			j[ "color" ][ "y" ] = color.g;
			j[ "color" ][ "z" ] = color.b;
			j[ "color" ][ "w" ] = color.a;
			SendUniforms( j );
			AddToLog( j );

			// dispatch the compute shader
			BlockDispatch();
		}


		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	if ( ImGui::BeginTabItem( " Description " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );

		// TODO: DESCRIPTION OF CYLINDER/TUBE PRIMITIVE
			// CYLINDER IS A TUBE WITH A ZERO INNER RADIUS

		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	ImGui::EndTabBar();
}

void engine::MenuEllipsoid () {
	OrangeText( "Ellipsoid" );
	ImGui::BeginTabBar( "ellipsoid" );
	if ( ImGui::BeginTabItem( " Controls " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );

		static glm::vec3 center ( 0.0f );
		static glm::vec3 rotations ( 0.0f );
		static glm::vec3 radii ( 0.0f );
		static bool draw = true;
		static int mask = 0;
		static glm::vec4 color( 0.0f );

		OrangeText( "Center Point" );
		ImGui::SliderFloat( "Center X", &center.x, 0.0f, float( BLOCKDIM ), "%.3f" );
		ImGui::SliderFloat( "Center Y", &center.y, 0.0f, float( BLOCKDIM ), "%.3f" );
		ImGui::SliderFloat( "Center Z", &center.z, 0.0f, float( BLOCKDIM ), "%.3f" );
		OrangeText( "Rotation" );
		ImGui::SliderFloat( "Rotation About X", &rotations.x, 0.0f, float( BLOCKDIM ), "%.3f" );
		ImGui::SliderFloat( "Rotation About Y", &rotations.y, 0.0f, float( BLOCKDIM ), "%.3f" );
		ImGui::SliderFloat( "Rotation About Z", &rotations.z, 0.0f, float( BLOCKDIM ), "%.3f" );
		OrangeText( "Radii" );
		ImGui::SliderFloat( "X Radius", &radii.x, 0.0f, float( BLOCKDIM ), "%.3f" );
		ImGui::SliderFloat( "Y Radius", &radii.y, 0.0f, float( BLOCKDIM ), "%.3f" );
		ImGui::SliderFloat( "Z Radius", &radii.z, 0.0f, float( BLOCKDIM ), "%.3f" );
		ColorPickerHelper( draw, mask, color );

		SetPosBottomRightCorner();
		if ( ImGui::Button( "Invoke Operation" ) ) {
			// swap the front/back buffers
			SwapBlocks();

			// apply the bindset
			bindSets[ "Basic Operation" ].apply();

			// send the uniforms
			json j;
			j[ "shader" ] = "Ellipsoid";
			j[ "bindset" ] = "Basic Operation";
			j[ "center" ][ "type" ] = "vec3";
			j[ "center" ][ "x" ] = center.x;
			j[ "center" ][ "y" ] = center.y;
			j[ "center" ][ "z" ] = center.z;
			j[ "rotations" ][ "type" ] = "vec3";
			j[ "rotations" ][ "x" ] = rotations.x;
			j[ "rotations" ][ "y" ] = rotations.y;
			j[ "rotations" ][ "z" ] = rotations.z;
			j[ "radii" ][ "type" ] = "vec3";
			j[ "radii" ][ "x" ] = radii.x;
			j[ "radii" ][ "y" ] = radii.y;
			j[ "radii" ][ "z" ] = radii.z;
			j[ "draw" ][ "type" ] = "bool";
			j[ "draw" ][ "x" ] = draw;
			j[ "mask" ][ "type" ] = "int";
			j[ "mask" ][ "x" ] = mask;
			j[ "color" ][ "type" ] = "vec4";
			j[ "color" ][ "x" ] = color.r;
			j[ "color" ][ "y" ] = color.g;
			j[ "color" ][ "z" ] = color.b;
			j[ "color" ][ "w" ] = color.a;
			SendUniforms( j );
			AddToLog( j );

			// dispatch the compute shader
			BlockDispatch();
		}

		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	if ( ImGui::BeginTabItem( " Description " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );

		// TODO: Ellipsoid description

		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	ImGui::EndTabBar();
}

void engine::MenuGrid () {
	OrangeText( "Regular Grid" );
	ImGui::BeginTabBar( "regular grid" );
	if ( ImGui::BeginTabItem( " Controls " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );

		static glm::ivec3 spacing ( 0 );
		static glm::ivec3 offsets ( 0 );
		static glm::ivec3 width ( 0 );
		static glm::vec3 rotation ( 0.0f );
		static bool draw = true;
		static int mask = 0;
		static glm::vec4 color( 0.0f );

		OrangeText( "Rotation" );
		ImGui::SliderFloat( "Rotation About X", &rotation.x, 0.0f, float( BLOCKDIM ), "%.3f" );
		ImGui::SliderFloat( "Rotation About Y", &rotation.y, 0.0f, float( BLOCKDIM ), "%.3f" );
		ImGui::SliderFloat( "Rotation About Z", &rotation.z, 0.0f, float( BLOCKDIM ), "%.3f" );
		OrangeText( "Grid Spacing" );
		ImGui::SliderInt( "X", &spacing.x, 0, BLOCKDIM );
		ImGui::SliderInt( "Y", &spacing.y, 0, BLOCKDIM );
		ImGui::SliderInt( "Z", &spacing.z, 0, BLOCKDIM );
		OrangeText( "Grid Offset" );
		ImGui::SliderInt( "X ", &offsets.x, 0, BLOCKDIM );
		ImGui::SliderInt( "Y ", &offsets.y, 0, BLOCKDIM );
		ImGui::SliderInt( "Z ", &offsets.z, 0, BLOCKDIM );
		OrangeText( "Grid Width" );
		ImGui::SliderInt( "X  ", &width.x, 0, BLOCKDIM );
		ImGui::SliderInt( "Y  ", &width.y, 0, BLOCKDIM );
		ImGui::SliderInt( "Z  ", &width.z, 0, BLOCKDIM );
		ColorPickerHelper( draw, mask, color );

		SetPosBottomRightCorner();
		if ( ImGui::Button( "Invoke Operation" ) ) {
			// swap the front/back buffers
			SwapBlocks();

			// apply the bindset
			bindSets[ "Basic Operation" ].apply();

			// send the uniforms
			json j;
			j[ "shader" ] = "Grid";
			j[ "bindset" ] = "Basic Operation";
			j[ "spacing" ][ "type" ] = "ivec3";
			j[ "spacing" ][ "x" ] = spacing.x;
			j[ "spacing" ][ "y" ] = spacing.y;
			j[ "spacing" ][ "z" ] = spacing.z;
			j[ "offsets" ][ "type" ] = "ivec3";
			j[ "offsets" ][ "x" ] = offsets.x;
			j[ "offsets" ][ "y" ] = offsets.y;
			j[ "offsets" ][ "z" ] = offsets.z;
			j[ "width" ][ "type" ] = "ivec3";
			j[ "width" ][ "x" ] = width.x;
			j[ "width" ][ "y" ] = width.y;
			j[ "width" ][ "z" ] = width.z;
			j[ "rotation" ][ "type" ] = "vec3";
			j[ "rotation" ][ "x" ] = rotation.x;
			j[ "rotation" ][ "y" ] = rotation.y;
			j[ "rotation" ][ "z" ] = rotation.z;
			j[ "draw" ][ "type" ] = "bool";
			j[ "draw" ][ "x" ] = draw;
			j[ "mask" ][ "type" ] = "int";
			j[ "mask" ][ "x" ] = mask;
			j[ "color" ][ "type" ] = "vec4";
			j[ "color" ][ "x" ] = color.r;
			j[ "color" ][ "y" ] = color.g;
			j[ "color" ][ "z" ] = color.b;
			j[ "color" ][ "w" ] = color.a;
			SendUniforms( j );
			AddToLog( j );

			// dispatch the compute shader
			BlockDispatch();
		}

		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	if ( ImGui::BeginTabItem( " Description " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );

		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	ImGui::EndTabBar();
}

void engine::MenuHeightmap () {
	OrangeText( "Heightmap" );
	ImGui::BeginTabBar( "heightmap" );
	if ( ImGui::BeginTabItem( " Controls " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );
		OrangeText( "Currently Unimplemented" );
		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	if ( ImGui::BeginTabItem( " Description " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );

		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	ImGui::EndTabBar();
}

void engine::MenuIcosahedron () {
	OrangeText( "Icosahedron" );
	ImGui::BeginTabBar( "icosahedron" );
	if ( ImGui::BeginTabItem( " Controls " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );
		OrangeText( "Currently Unimplemented" );
		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	if ( ImGui::BeginTabItem( " Description " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );

		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	ImGui::EndTabBar();
}

void engine::MenuNoise () {
	OrangeText( "Noise" );
	ImGui::BeginTabBar( "noise" );
	if ( ImGui::BeginTabItem( " Controls " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );
		OrangeText( "Currently Unimplemented" );
		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	if ( ImGui::BeginTabItem( " Description " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );

		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	ImGui::EndTabBar();
}

void engine::MenuSphere () {
	OrangeText( "Sphere" );
	ImGui::BeginTabBar( "sphere" );
	if ( ImGui::BeginTabItem( " Controls " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );

		static glm::vec3 center ( 0.0f );
		static float radius ( 0.0f );
		static bool draw = true;
		static int mask = 0;
		static glm::vec4 color( 0.0f );

		OrangeText( "Center Point" );
		ImGui::SliderFloat( "Center X", &center.x, 0.0f, float( BLOCKDIM ), "%.3f" );
		ImGui::SliderFloat( "Center Y", &center.y, 0.0f, float( BLOCKDIM ), "%.3f" );
		ImGui::SliderFloat( "Center Z", &center.z, 0.0f, float( BLOCKDIM ), "%.3f" );
		OrangeText( "Radius" );
		ImGui::SliderFloat( "Radius", &radius, 0.0f, float( BLOCKDIM ), "%.3f" );
		ColorPickerHelper( draw, mask, color );

		SetPosBottomRightCorner();
		if ( ImGui::Button( "Invoke Operation" ) ) {
			// swap the front/back buffers
			SwapBlocks();

			// apply the bindset
			bindSets[ "Basic Operation" ].apply();

			// send the uniforms
			json j;
			j[ "shader" ] = "Sphere";
			j[ "bindset" ] = "Basic Operation";
			j[ "center" ][ "type" ] = "vec3";
			j[ "center" ][ "x" ] = center.x;
			j[ "center" ][ "y" ] = center.y;
			j[ "center" ][ "z" ] = center.z;
			j[ "radius" ][ "type" ] = "float";
			j[ "radius" ][ "x" ] = radius;
			j[ "draw" ][ "type" ] = "bool";
			j[ "draw" ][ "x" ] = draw;
			j[ "mask" ][ "type" ] = "int";
			j[ "mask" ][ "x" ] = mask;
			j[ "color" ][ "type" ] = "vec4";
			j[ "color" ][ "x" ] = color.r;
			j[ "color" ][ "y" ] = color.g;
			j[ "color" ][ "z" ] = color.b;
			j[ "color" ][ "w" ] = color.a;
			SendUniforms( j );
			AddToLog( j );

			// dispatch the compute shader
			BlockDispatch();
		}

		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	if ( ImGui::BeginTabItem( " Description " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );

		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	ImGui::EndTabBar();
}

void engine::MenuTriangle () {
	OrangeText( "Triangle" );
	ImGui::BeginTabBar( "triangle" );
	if ( ImGui::BeginTabItem( " Controls " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );

		static glm::vec3 point1 ( 0.0f );
		static glm::vec3 point2 ( 0.0f );
		static glm::vec3 point3 ( 0.0f );
		static float thickness ( 0.0f );
		static bool draw = true;
		static int mask = 0;
		static glm::vec4 color( 0.0f );

		OrangeText( "Point 1" );
		ImGui::SliderFloat( "X1", &point1.x, 0.0f, float( BLOCKDIM ), "%.3f" );
		ImGui::SliderFloat( "Y1", &point1.y, 0.0f, float( BLOCKDIM ), "%.3f" );
		ImGui::SliderFloat( "Z1", &point1.z, 0.0f, float( BLOCKDIM ), "%.3f" );

		OrangeText( "Point 2" );
		ImGui::SliderFloat( "X2", &point2.x, 0.0f, float( BLOCKDIM ), "%.3f" );
		ImGui::SliderFloat( "Y2", &point2.y, 0.0f, float( BLOCKDIM ), "%.3f" );
		ImGui::SliderFloat( "Z2", &point2.z, 0.0f, float( BLOCKDIM ), "%.3f" );

		OrangeText( "Point 3" );
		ImGui::SliderFloat( "X3", &point3.x, 0.0f, float( BLOCKDIM ), "%.3f" );
		ImGui::SliderFloat( "Y3", &point3.y, 0.0f, float( BLOCKDIM ), "%.3f" );
		ImGui::SliderFloat( "Z3", &point3.z, 0.0f, float( BLOCKDIM ), "%.3f" );

		OrangeText( "Thickness" );
		ImGui::SliderFloat( "Thickness", &thickness, 0.0f, float( BLOCKDIM ), "%.3f" );
		ColorPickerHelper( draw, mask, color );

		SetPosBottomRightCorner();
		if ( ImGui::Button( "Invoke Operation" ) ) {
			// swap the front/back buffers
			SwapBlocks();

			// apply the bindset
			bindSets[ "Basic Operation" ].apply();

			// send the uniforms
			json j;
			j[ "shader" ] = "Triangle";
			j[ "bindset" ] = "Basic Operation";
			j[ "point1" ][ "type" ] = "vec3";
			j[ "point1" ][ "x" ] = point1.x;
			j[ "point1" ][ "y" ] = point1.y;
			j[ "point1" ][ "z" ] = point1.z;
			j[ "point2" ][ "type" ] = "vec3";
			j[ "point2" ][ "x" ] = point2.x;
			j[ "point2" ][ "y" ] = point2.y;
			j[ "point2" ][ "z" ] = point2.z;
			j[ "point3" ][ "type" ] = "vec3";
			j[ "point3" ][ "x" ] = point3.x;
			j[ "point3" ][ "y" ] = point3.y;
			j[ "point3" ][ "z" ] = point3.z;
			j[ "thickness" ][ "type" ] = "float";
			j[ "thickness" ][ "x" ] = thickness;
			j[ "draw" ][ "type" ] = "bool";
			j[ "draw" ][ "x" ] = draw;
			j[ "mask" ][ "type" ] = "int";
			j[ "mask" ][ "x" ] = mask;
			j[ "color" ][ "type" ] = "vec4";
			j[ "color" ][ "x" ] = color.r;
			j[ "color" ][ "y" ] = color.g;
			j[ "color" ][ "z" ] = color.b;
			j[ "color" ][ "w" ] = color.a;
			SendUniforms( j );
			AddToLog( j );

			// dispatch the compute shader
			BlockDispatch();
		}

		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	if ( ImGui::BeginTabItem( " Description " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );

		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	ImGui::EndTabBar();
}

void engine::MenuUserShader () {
	OrangeText( "User Shader" );
	ImGui::BeginTabBar( "user shader" );
	if ( ImGui::BeginTabItem( " Controls " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );
		OrangeText( "Currently Unimplemented" );
		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	if ( ImGui::BeginTabItem( " Description " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );

		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	ImGui::EndTabBar();
}

void engine::MenuVAT () {
	OrangeText( "Voxel Automata Terrain" );
	ImGui::BeginTabBar( "VAT" );
	if ( ImGui::BeginTabItem( " Controls " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );
		OrangeText( "Currently Unimplemented" );
		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	if ( ImGui::BeginTabItem( " Description " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );

		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	ImGui::EndTabBar();
}

void engine::MenuSpaceship () {
	OrangeText( "Spaceship Generator" );
	ImGui::BeginTabBar( "spaceship generator" );
	if ( ImGui::BeginTabItem( " Controls " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );
		OrangeText( "Currently Unimplemented" );
		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	if ( ImGui::BeginTabItem( " Description " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );

		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	ImGui::EndTabBar();
}

void engine::MenuLetters () {
	OrangeText( "Letters" );
	ImGui::BeginTabBar( "letters" );
	if ( ImGui::BeginTabItem( " Controls " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );
		OrangeText( "Currently Unimplemented" );
		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	if ( ImGui::BeginTabItem( " Description " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );

		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	ImGui::EndTabBar();
}

void engine::MenuXOR () {
	OrangeText( "XOR" );
	ImGui::BeginTabBar( "xor" );
	if ( ImGui::BeginTabItem( " Controls " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );

		// maybe implement thresholding or something? tbd
		static bool respectMask = true;
		ImGui::Checkbox( "Respect Mask", &respectMask );
		if ( ImGui::Button( "XOR" ) ) {
			SwapBlocks();
			bindSets[ "Basic Operation" ].apply();
			json j;
			j[ "shader" ] = "XOR";
			j[ "bindset" ] = "Basic Operation";
			j[ "respectMask" ][ "type" ] = "bool";
			j[ "respectMask" ][ "x" ] = respectMask;
			SendUniforms( j );
			AddToLog( j );
			BlockDispatch();
		}


		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	if ( ImGui::BeginTabItem( " Description " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );

		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	ImGui::EndTabBar();
}

void engine::MenuClearBlock () {
	OrangeText( "Clear Block" );
	ImGui::BeginTabBar( "clear block" );
	if ( ImGui::BeginTabItem( " Controls " ) ) {
		ImGui::Separator();
		ImGui::Indent( 32.0f );

		static bool respectMask = true;
		static bool draw = false;
		static int mask = 0;
		static glm::vec4 color ( 0.0f );

		ImGui::Text( " " );
		ImGui::Checkbox( "Respect Mask", &respectMask );
		ImGui::Checkbox( "Draw Color", &draw );

		if ( draw ) {
			ImGui::InputInt( " Mask ", &mask );
			mask = std::clamp( mask, 0, 255 );
			ImGui::ColorEdit4( "  Color", ( float * ) &color, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreviewHalf );
		}

		ImGui::Unindent( 16.0f );
		ImGui::Text( " " );
		if ( ImGui::Button( "Invoke Operation" ) ) {
			// swap the front/back buffers
			SwapBlocks();

			// apply the bindset
			bindSets[ "Basic Operation" ].apply();

			// send the uniforms
			json j;
			j[ "shader" ] = "Clear";
			j[ "bindset" ] = "Basic Operation";
			j[ "respectMask" ][ "type" ] = "bool";
			j[ "respectMask" ][ "x" ] = respectMask;
			j[ "draw" ][ "type" ] = "bool";
			j[ "draw" ][ "x" ] = draw;
			j[ "mask" ][ "type" ] = "int";
			j[ "mask" ][ "x" ] = mask;
			j[ "color" ][ "type" ] = "vec4";
			j[ "color" ][ "x" ] = color.r;
			j[ "color" ][ "y" ] = color.g;
			j[ "color" ][ "z" ] = color.b;
			j[ "color" ][ "w" ] = color.a;
			SendUniforms( j );
			AddToLog( j );

			// dispatch the compute shader
			BlockDispatch();
		}

		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	if ( ImGui::BeginTabItem( " Description " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );

		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	ImGui::EndTabBar();
}

void engine::MenuMasking () {
	OrangeText( "Masking Operations" );
	ImGui::BeginTabBar( "masking" );
	if ( ImGui::BeginTabItem( " Controls " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );

		static glm::vec4 color;
		static glm::vec4 variances;
		static glm::vec3 lightValue;
		static glm::vec3 lightVariance;
		static bool useR;
		static bool useG;
		static bool useB;
		static bool useA;
		static bool useLightR;
		static bool useLightG;
		static bool useLightB;
		static int amount;

		// unmask all
		ImGui::Text( " " );
		OrangeText( "Unmask All" );
		if( ImGui::Button( "Unmask All" ) ) {
			SwapBlocks();
			bindSets[ "Basic Operation" ].apply();
			json j;
			j[ "shader" ] = "Mask Clear";
			j[ "bindset" ] = "Basic Operation";
			SendUniforms( j );
			AddToLog( j );
			BlockDispatch();
		}

		// invert mask
		ImGui::Text( " " );
		ImGui::Text( " " );
		OrangeText( "Invert Mask" );
		if( ImGui::Button( "Invert Mask" ) ) {
			SwapBlocks();
			bindSets[ "Basic Operation" ].apply();
			json j;
			j[ "shader" ] = "Mask Invert";
			j[ "bindset" ] = "Basic Operation";
			SendUniforms( j );
			AddToLog( j );
			BlockDispatch();
		}

		ImGui::Text( " " );
		ImGui::Text( " " );
		OrangeText( "Data Based Masking" );
		ImGui::Indent( 16.0f );
		OrangeText( "Color Levels" );
		ImGui::Unindent( 16.0f );
		ImGui::ColorEdit4( "Color", ( float * ) &color, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreviewHalf);
		ImGui::Separator();
		ImGui::Checkbox( "Use Red Channel  ", &useR );
		ImGui::SameLine();
		ImGui::SliderFloat( "Red Spread", &variances.r, 0, 255 );
		ImGui::Checkbox( "Use Green Channel", &useG );
		ImGui::SameLine();
		ImGui::SliderFloat( "Green Spread", &variances.g, 0, 255 );
		ImGui::Checkbox( "Use Blue Channel ", &useB );
		ImGui::SameLine();
		ImGui::SliderFloat( "Blue Spread", &variances.b, 0, 255 );
		ImGui::Checkbox( "Use Alpha Channel", &useA );
		ImGui::SameLine();
		ImGui::SliderFloat( "Alpha Spread", &variances.a, 0, 255 );
		ImGui::Indent( 16.0f );
		OrangeText( "Light Levels" );
		ImGui::Unindent( 16.0f );
		ImGui::ColorEdit3( "Light Color", ( float * ) &lightValue );
		ImGui::Checkbox( "Use Light ( Red )  ", &useLightR );
		ImGui::SameLine();
		ImGui::SliderFloat( "Light Spread ( Red )", &lightVariance.r, 0.0f, 1.0f, "%.3f" );
		ImGui::Checkbox( "Use Light ( Green )", &useLightG );
		ImGui::SameLine();
		ImGui::SliderFloat( "Light Spread ( Green )", &lightVariance.g, 0.0f, 1.0f, "%.3f" );
		ImGui::Checkbox( "Use Light ( Blue ) ", &useLightB );
		ImGui::SameLine();
		ImGui::SliderFloat( "Light Spread ( Blue )", &lightVariance.b, 0.0f, 1.0f, "%.3f" );
		ImGui::Separator();
		ImGui::SliderInt("Mask Amount", &amount, 0, 255);

		if( ImGui::Button( "Mask By Data" ) ) {
			// swap the front/back buffers
			SwapBlocks();

			// apply the bindset
			bindSets[ "Basic Operation With Lighting" ].apply();

			// send the uniforms
			json j;
			j[ "shader" ] = "Data Mask";
			j[ "bindset" ] = "Basic Operation With Lighting";
			j[ "useR" ][ "type" ] = "bool";
			j[ "useR" ][ "x" ] = useR;
			j[ "useG" ][ "type" ] = "bool";
			j[ "useG" ][ "x" ] = useG;
			j[ "useB" ][ "type" ] = "bool";
			j[ "useB" ][ "x" ] = useB;
			j[ "useA" ][ "type" ] = "bool";
			j[ "useA" ][ "x" ] = useA;
			j[ "useLightR" ][ "type" ] = "bool";
			j[ "useLightR" ][ "x" ] = useLightR;
			j[ "useLightG" ][ "type" ] = "bool";
			j[ "useLightG" ][ "x" ] = useLightG;
			j[ "useLightB" ][ "type" ] = "bool";
			j[ "useLightB" ][ "x" ] = useLightB;
			j[ "variances" ][ "type" ] = "vec4";
			j[ "variances" ][ "x" ] = variances.r / 255.0;
			j[ "variances" ][ "y" ] = variances.g / 255.0;
			j[ "variances" ][ "z" ] = variances.b / 255.0;
			j[ "variances" ][ "w" ] = variances.a / 255.0;
			j[ "lightValue" ][ "type" ] = "vec3";
			j[ "lightValue" ][ "x" ] = lightValue.x;
			j[ "lightValue" ][ "y" ] = lightValue.y;
			j[ "lightValue" ][ "z" ] = lightValue.z;
			j[ "lightVariance" ][ "type" ] = "vec3";
			j[ "lightVariance" ][ "x" ] = lightVariance.x;
			j[ "lightVariance" ][ "y" ] = lightVariance.y;
			j[ "lightVariance" ][ "z" ] = lightVariance.z;
			j[ "mask" ][ "type" ] = "int";
			j[ "mask" ][ "x" ] = amount;
			j[ "color" ][ "type" ] = "vec4";
			j[ "color" ][ "x" ] = color.r;
			j[ "color" ][ "y" ] = color.g;
			j[ "color" ][ "z" ] = color.b;
			j[ "color" ][ "w" ] = color.a;
			SendUniforms( j );
			AddToLog( j );

			// dispatch the compute shader
			BlockDispatch();
		}

		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	if ( ImGui::BeginTabItem( " Description " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );

		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	ImGui::EndTabBar();
}

void engine::MenuBlur () {
	OrangeText( "Blur" );
	ImGui::BeginTabBar( "blur" );
	if ( ImGui::BeginTabItem( " Controls " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );

		static bool touchAlpha = true;
		static bool respectMask = false;
		static int radius = 0;

		ImGui::Text( " " );
		ImGui::SliderInt( "Radius", &radius, 0, 5 );
		ImGui::Checkbox( "Touch Alpha", &touchAlpha );
		ImGui::Checkbox( "Respect Mask", &respectMask );
		ImGui::Text( " " );

		// type - box / gaussian
		OrangeText( "Box Kernel" );
		ImGui::Indent( 16.0f );
		if ( ImGui::Button( "Box Blur" ) ) {
			SwapBlocks();
			bindSets[ "Basic Operation" ].apply();
			json j;
			j[ "shader" ] = "Box Blur";
			j[ "bindset" ] = "Basic Operation";
			j[ "touchAlpha" ][ "type" ] = "bool";
			j[ "touchAlpha" ][ "x" ] = touchAlpha;
			j[ "respectMask" ][ "type" ] = "bool";
			j[ "respectMask" ][ "x" ] = respectMask;
			j[ "radius" ][ "type" ] = "int";
			j[ "radius" ][ "x" ] = radius;
			SendUniforms( j );
			AddToLog( j );
			BlockDispatch();
		}
		ImGui::Unindent( 16.0f );
		OrangeText( "Gaussian Kernel" );
		ImGui::Indent( 16.0f );
		if ( ImGui::Button( "Gaussian Blur" ) ) {
			SwapBlocks();
			bindSets[ "Basic Operation" ].apply();
			json j;
			j[ "shader" ] = "Gaussian Blur";
			j[ "bindset" ] = "Basic Operation";
			j[ "touchAlpha" ][ "type" ] = "bool";
			j[ "touchAlpha" ][ "x" ] = touchAlpha;
			j[ "respectMask" ][ "type" ] = "bool";
			j[ "respectMask" ][ "x" ] = respectMask;
			j[ "radius" ][ "type" ] = "int";
			j[ "radius" ][ "x" ] = radius;
			SendUniforms( j );
			AddToLog( j );
			BlockDispatch();
		}
		ImGui::Unindent( 16.0f );
		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	if ( ImGui::BeginTabItem( " Description " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );

		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	ImGui::EndTabBar();
}

void engine::MenuShiftTrim () {
	OrangeText( "Shift/Trim" );
	ImGui::BeginTabBar( "shift/trim" );
	if ( ImGui::BeginTabItem( " Controls " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );

		// these options tbd - v12 did 3 set modes, that might be a cleaner way to do it
		// static bool respectMask = false;
		// static bool carryMask = false;
		static bool loopFaces = true;
		static glm::ivec3 shiftAmount ( 0 );
		static int trimAmount = 0;

		ImGui::Text( " " );
		OrangeText( "Shift" );
		ImGui::Text( " " );
		ImGui::Indent( 16.0f );
		ImGui::Text( "Amount" );
		ImGui::SliderInt( "X", &shiftAmount.x, -BLOCKDIM, BLOCKDIM );
		ImGui::SliderInt( "Y", &shiftAmount.y, -BLOCKDIM, BLOCKDIM );
		ImGui::SliderInt( "Z", &shiftAmount.z, -BLOCKDIM, BLOCKDIM );
		// ImGui::Checkbox( "Respect Mask", &respectMask );
		// ImGui::Checkbox( "Carry Mask", &carryMask );
		ImGui::Checkbox( "Wraparound", &loopFaces );
		ImGui::Text( " " );

		if ( ImGui::Button( "Shift" ) ) {
			SwapBlocks();
			// bindSets[ "Basic Operation With Lighting" ].apply();
			bindSets[ "Basic Operation" ].apply();
			json j;
			j[ "shader" ] = "Shift";
			// j[ "bindset" ] = "Basic Operation With Lighting";
			j[ "bindset" ] = "Basic Operation";
			j[ "shiftAmount" ][ "type" ] = "ivec3";
			j[ "shiftAmount" ][ "x" ] = shiftAmount.x;
			j[ "shiftAmount" ][ "y" ] = shiftAmount.y;
			j[ "shiftAmount" ][ "z" ] = shiftAmount.z;
			j[ "wraparound" ][ "type" ] = "bool";
			j[ "wraparound" ][ "x" ] = loopFaces;
			SendUniforms( j );
			AddToLog( j );
			BlockDispatch();
		}

		ImGui::Unindent( 16.0f );
		OrangeText( "Trim" );
		ImGui::Text( " " );
		ImGui::Indent( 16.0f );
		ImGui::SliderInt( "Amount", &trimAmount, 0, BLOCKDIM / 4 );
		ImGui::Text( " " );

		if ( ImGui::Button( "Trim" ) ) {
			// shift by n pixels up
			// shift by -2n pixels
			// shift by n pixels up
				// no wrapping, so data is lost on faces
					// maybe optionally wrap on masked voxels, so that it remains? tbd
			SwapBlocks();
			// bindSets[ "Basic Operation With Lighting" ].apply();
			bindSets[ "Basic Operation" ].apply();
			json j;
			j[ "shader" ] = "Shift";
			// j[ "bindset" ] = "Basic Operation With Lighting";
			j[ "bindset" ] = "Basic Operation";
			j[ "shiftAmount" ][ "type" ] = "ivec3";
			j[ "shiftAmount" ][ "x" ] = trimAmount;
			j[ "shiftAmount" ][ "y" ] = trimAmount;
			j[ "shiftAmount" ][ "z" ] = trimAmount;
			j[ "wraparound" ][ "type" ] = "bool";
			j[ "wraparound" ][ "x" ] = false;
			SendUniforms( j );
			AddToLog( j );
			BlockDispatch();

			SwapBlocks();
			// bindSets[ "Basic Operation With Lighting" ].apply();
			bindSets[ "Basic Operation" ].apply();
			j[ "shiftAmount" ][ "x" ] = -2 * trimAmount;
			j[ "shiftAmount" ][ "y" ] = -2 * trimAmount;
			j[ "shiftAmount" ][ "z" ] = -2 * trimAmount;
			SendUniforms( j );
			AddToLog( j );
			BlockDispatch();

			SwapBlocks();
			// bindSets[ "Basic Operation With Lighting" ].apply();
			bindSets[ "Basic Operation" ].apply();
			j[ "shiftAmount" ][ "x" ] = trimAmount;
			j[ "shiftAmount" ][ "y" ] = trimAmount;
			j[ "shiftAmount" ][ "z" ] = trimAmount;
			SendUniforms( j );
			AddToLog( j );
			BlockDispatch();
		}

		ImGui::Unindent( 32.0f );
		ImGui::EndTabItem();
	}
	if ( ImGui::BeginTabItem( " Description " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );

		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	ImGui::EndTabBar();
}

void engine::MenuLoadSave () {
	OrangeText( "Load/Save" );
	ImGui::BeginTabBar( "load/save" );
	if ( ImGui::BeginTabItem( " Controls " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );

		#define LISTBOX_SIZE_MAX 256
		static char inputString[ 256 ] = "";
		const char *listboxItems[ LISTBOX_SIZE_MAX ];
		uint32_t i;
		for ( i = 0; i < LISTBOX_SIZE_MAX && i < savesList.size(); ++i ) {
			listboxItems[ i ] = savesList[ i ].c_str();
		}

		OrangeText( "Files In Saves Folder" );
		static int listboxSelected = 0;
		ImGui::ListBox( " ", &listboxSelected, listboxItems, i, 24 );

		static bool respectMask = false;
		if ( ImGui::Button( " Load " ) ) {
			Image loadedImage( savesList[ listboxSelected ], LODEPNG );

			// buffer to the loadbuffer
			glBindTexture( GL_TEXTURE_3D, textures[ "LoadBuffer" ] );
			glTexImage3D( GL_TEXTURE_3D, 0, GL_RGBA8, BLOCKDIM, BLOCKDIM, BLOCKDIM, 0, GL_RGBA, GL_UNSIGNED_BYTE, &loadedImage.data.data()[ 0 ] );

			// call the copyLoadbuffer shader
			SwapBlocks();
			bindSets[ "LoadBuffer" ].apply();
			json j;
			j[ "shader" ] = "Load";
			j[ "bindset" ] = "LoadBuffer";
			j[ "respectMask" ][ "type" ] = "bool";
			j[ "respectMask" ][ "x" ] = respectMask;
			SendUniforms( j );
			AddToLog( j );
			cout << j << endl;
			BlockDispatch();
		}
		ImGui::SameLine();
		ImGui::Checkbox( " Respect Mask on Load", &respectMask );

		ImGui::Text( " " );
		OrangeText( "Enter Filename to Save" );
		ImGui::InputTextWithHint( ".png", "", inputString, IM_ARRAYSIZE( inputString ) );
		ImGui::SameLine();
		if ( ImGui::Button( " Save " ) ) {
			std::string saveString;
			if ( hasPNG( std::string( inputString ) ) ) {
				saveString = std::string( inputString );
			} else {
				saveString = std::string( inputString ) + std::string( ".png" );
			}

			// blahblah save it
			std::vector<uint8_t> bytesToSave;
			bytesToSave.resize( BLOCKDIM * BLOCKDIM * BLOCKDIM * 4 );
			glBindTexture( GL_TEXTURE_3D, textures[ "Color Block Front" ] );
			glGetTexImage( GL_TEXTURE_3D, 0, GL_RGBA, GL_UNSIGNED_BYTE, &bytesToSave[ 0 ] );

			Image saveImage( BLOCKDIM, BLOCKDIM * BLOCKDIM, &bytesToSave.data()[ 0 ] );
			saveImage.Save( "saves/" + saveString, LODEPNG );

			// get the list with this included
			updateSavesList();
		}

		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	if ( ImGui::BeginTabItem( " Description " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );

		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	ImGui::EndTabBar();
}

void engine::MenuLimiterCompressor () {
	OrangeText( "Limiter/Compressor" );
	ImGui::BeginTabBar( "limiter/compressor" );
	if ( ImGui::BeginTabItem( " Controls " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );
		OrangeText( "Currently Unimplemented" );
		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	if ( ImGui::BeginTabItem( " Description " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );

		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	ImGui::EndTabBar();
}

void engine::MenuCopyPaste () {
	OrangeText( "Copy/Paste" );
	ImGui::BeginTabBar( "copy/paste" );
	if ( ImGui::BeginTabItem( " Controls " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );
		OrangeText( "Currently Unimplemented" );
		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	if ( ImGui::BeginTabItem( " Description " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );

		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	ImGui::EndTabBar();
}

void engine::MenuLogging () {
	OrangeText( "Logging" );
	ImGui::BeginTabBar( "logging" );
	if ( ImGui::BeginTabItem( " Controls " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );
		OrangeText( "Currently Unimplemented" );
		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	if ( ImGui::BeginTabItem( " Description " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );

		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	ImGui::EndTabBar();
}

void engine::MenuScreenshot () {
	OrangeText( "Screenshot" );
	ImGui::Separator();
	ImGui::Indent( 16.0f );

	ImGui::Text( " " );
	OrangeText( "Accumulator Screenshot" );
	ImGui::Separator();
	ImGui::TextWrapped( "Accumulator texture just has the averaged samples. This is before tonemapping, etc, and does not include any filtering. It will be the size of the image buffer times the SSFACTOR on each x and y." );
	ImGui::Indent( 16.0f );
	if ( ImGui::Button( "Accumulator Screenshot" ) ) {
		std::vector<uint8_t> imageBytesToSaveA;
		imageBytesToSaveA.resize( WIDTH * HEIGHT * SSFACTOR * SSFACTOR * 4 );
		glBindTexture( GL_TEXTURE_2D, textures[ "Accumulator" ] );
		glGetTexImage( GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, &imageBytesToSaveA.data()[ 0 ] );
		Image screenshotA( WIDTH * SSFACTOR, HEIGHT * SSFACTOR, &imageBytesToSaveA.data()[ 0 ] );
		screenshotA.FlipVertical();
		auto nowA = std::chrono::system_clock::now();
		auto inTime_tA = std::chrono::system_clock::to_time_t( nowA );
		std::stringstream ssA;
		ssA << std::put_time( std::localtime( &inTime_tA ), "screenshots/Voraldo13ssA-%Y-%m-%d %X.png" );
		screenshotA.Save( ssA.str(), LODEPNG );
	}
	ImGui::Unindent( 16.0f );
	ImGui::Separator();

	ImGui::Text( " " );
	ImGui::Text( " " );
	OrangeText( "Postprocessed Screenshot" );
	ImGui::Separator();
	ImGui::TextWrapped( "This sets a flag to grab a screenshot out of the display texture after the postprocessing takes place next frame. Because of when this input takes place, the trident and timing text have already been applied, so the point of this is to get it with tonemapping and other postprocessing, but without the widget or timing text. This is sampled at the specified WIDTH and HEIGHT dimensions, without the SSFACTOR applied." );
	ImGui::Indent( 16.0f );
	if ( ImGui::Button( "Get Postprocessed Screenshot" ) ) {
		wantCapturePostprocessScreenshot = true;
	}
	ImGui::Unindent( 16.0f );
	ImGui::Separator();

	ImGui::Text( " " );
	ImGui::Text( " " );
	OrangeText( "Backbuffer Screenshot" );
	ImGui::Separator();
	ImGui::TextWrapped( "Backbuffer Screenshot gets the image after tonemapping, postprocessing, and application of the trident and timing text. This also includes any linear filtering that is applied when the display texture is sampled into the framebuffer, but does not include any of the ImGui menus, because they will not have been drawn yet." );
	ImGui::Indent( 16.0f );
	if ( ImGui::Button( "Backbuffer Screenshot" ) ) {
		std::vector<uint8_t> imageBytesToSaveB;
		const int width = int( ImGui::GetIO().DisplaySize.x );
		const int height = int( ImGui::GetIO().DisplaySize.y );
		imageBytesToSaveB.resize( width * height * 4 );
		glReadPixels( 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, &imageBytesToSaveB[ 0 ] );
		Image screenshotB( width, height, &imageBytesToSaveB.data()[ 0 ] );
		screenshotB.FlipVertical();
		auto nowB = std::chrono::system_clock::now();
		auto inTime_tB = std::chrono::system_clock::to_time_t( nowB );
		std::stringstream ssB;
		ssB << std::put_time( std::localtime( &inTime_tB ), "screenshots/Voraldo13ssB-%Y-%m-%d %X.png" );
		screenshotB.Save( ssB.str(), LODEPNG );
	}
	ImGui::Unindent( 16.0f );
	ImGui::Separator();
	ImGui::Unindent( 16.0f );
}

void engine::MenuClearLightLevels () {
	OrangeText( "Clear Light Levels" );
	ImGui::BeginTabBar( "light clear" );
	if ( ImGui::BeginTabItem( " Controls " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );

		static glm::vec4 color;

		ImGui::ColorEdit3( "Light Color", ( float * ) &color );
		ImGui::SliderFloat( "Intensity Scalar", &color.a, 0.0f, 5.0f );

		if ( ImGui::Button( "Clear Levels" ) ) {
			render.framesSinceLastInput = 0; // no swap, but will require a renderer refresh
			bindSets[ "Lighting Operation" ].apply();
			json j;
			j[ "shader" ] = "Light Clear";
			j[ "bindset" ] = "Lighting Operation";
			j[ "color" ][ "type" ] = "vec4";
			j[ "color" ][ "x" ] = color.r;
			j[ "color" ][ "y" ] = color.g;
			j[ "color" ][ "z" ] = color.b;
			j[ "color" ][ "w" ] = color.a;
			SendUniforms( j );
			AddToLog( j );
			BlockDispatch();
		}
		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	if ( ImGui::BeginTabItem( " Description " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );

		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	ImGui::EndTabBar();
}

void engine::MenuPointLight () {
	OrangeText( "Point Light" );
	ImGui::BeginTabBar( "point light" );
	if ( ImGui::BeginTabItem( " Controls " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );
		OrangeText( "Currently Unimplemented" );
		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	if ( ImGui::BeginTabItem( " Description " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );

		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	ImGui::EndTabBar();
}

void engine::MenuConeLight () {
	OrangeText( "Cone Light" );
	ImGui::BeginTabBar( "cone light" );
	if ( ImGui::BeginTabItem( " Controls " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );
		OrangeText( "Currently Unimplemented" );
		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	if ( ImGui::BeginTabItem( " Description " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );

		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	ImGui::EndTabBar();
}

void engine::MenuDirectionalLight () {
	OrangeText( "Directional Light" );
	ImGui::BeginTabBar( "directional light" );
	if ( ImGui::BeginTabItem( " Controls " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );
		OrangeText( "Currently Unimplemented" );
		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	if ( ImGui::BeginTabItem( " Description " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );

		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	ImGui::EndTabBar();
}

void engine::MenuFakeGI () {
	OrangeText( "Fake Global Illumination" );
	ImGui::BeginTabBar( "fake gi" );
	if ( ImGui::BeginTabItem( " Controls " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );
		OrangeText( "Currently Unimplemented" );
		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	if ( ImGui::BeginTabItem( " Description " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );

		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	ImGui::EndTabBar();
}

void engine::MenuAmbientOcclusion () {
	OrangeText( "Ambient Occlusion" );
	ImGui::BeginTabBar( "ao" );
	if ( ImGui::BeginTabItem( " Controls " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );

		static int radius = 0;

		ImGui::Text( " " );
		ImGui::SliderInt( "Radius", &radius, 0, 5 );
		ImGui::Text( " " );

		ImGui::Indent( 16.0f );
		if ( ImGui::Button( "Ambient Occlusion" ) ) {
			render.framesSinceLastInput = 0; // no swap, but will require a renderer refresh
			bindSets[ "Lighting Operation" ].apply();
			json j;
			j[ "shader" ] = "Ambient Occlusion";
			j[ "bindset" ] = "Lighting Operation";
			j[ "radius" ][ "type" ] = "int";
			j[ "radius" ][ "x" ] = radius;
			SendUniforms( j );
			AddToLog( j );
			BlockDispatch();
		}

		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	if ( ImGui::BeginTabItem( " Description " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );

		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	ImGui::EndTabBar();
}

void engine::MenuLightMash () {
	OrangeText( "Light Mash" );
	ImGui::BeginTabBar( "light mash" );
	if ( ImGui::BeginTabItem( " Controls " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );

		ImGui::Text( " " );
		if ( ImGui::Button( " Mash " ) ) {
			render.framesSinceLastInput = 0; // no swap, but will require a renderer refresh
			bindSets[ "Lighting Operation" ].apply();
			json j;
			j[ "shader" ] = "Light Mash";
			j[ "bindset" ] = "Lighting Operation";
			SendUniforms( j );
			AddToLog( j );
			BlockDispatch();
		}

		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	if ( ImGui::BeginTabItem( " Description " ) ) {
		ImGui::Separator();
		ImGui::Indent( 16.0f );

		ImGui::Unindent( 16.0f );
		ImGui::EndTabItem();
	}
	ImGui::EndTabBar();
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
	ImGui::ColorEdit4( "Clear Color", (float *) &render.clearColor, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreviewHalf );
	if( ImGui::IsItemEdited() ) render.framesSinceLastInput = 0;
	ImGui::SliderFloat( "Alpha Correction Power", &render.alphaCorrectionPower, 0.0f, 4.0f );
	if( ImGui::IsItemEdited() ) render.framesSinceLastInput = 0;
	ImGui::SliderFloat( "Jitter Amount", &render.jitterAmount, 0.0f, 20.0f, "%.2f", ImGuiSliderFlags_Logarithmic );
	if( ImGui::IsItemEdited() ) render.framesSinceLastInput = 0;
	ImGui::SliderFloat( "Perspective", &render.perspective, -2.0f, 4.0f );
	if( ImGui::IsItemEdited() ) render.framesSinceLastInput = 0;
	ImGui::SliderFloat( "Scale", &render.scaleFactor, 0.0f, 40.0f );
	if( ImGui::IsItemEdited() ) render.framesSinceLastInput = 0;
	ImGui::SliderFloat( "Blend Factor", &render.blendFactor, 0.0f, 1.0f );
	if( ImGui::IsItemEdited() ) render.framesSinceLastInput = 0;
	ImGui::SliderInt( "Volume Steps", &render.volumeSteps, 0, 1400 );
	if( ImGui::IsItemEdited() ) render.framesSinceLastInput = 0;

	// render mode - then set what shaders[ "Raymarch" ] points to
		// this will take a little more infrastructure work

	// picker for render mode shader
	ImGui::SliderInt( "History Frames", &render.numFramesHistory, 0, 14 );
	if( ImGui::IsItemEdited() ) render.framesSinceLastInput = 0;

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
	editor.SetPalette( TextEditor::GetDarkPalette() );

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
	ZoneScoped;
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame( windowHandler.window );
	ImGui::NewFrame();
}

void engine::ImguiFrameEnd () {
	ZoneScoped;
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
