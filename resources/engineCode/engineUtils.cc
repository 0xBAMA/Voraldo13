#include "engine.h"

bool engine::MainLoop () {
	FrameMarkStart( "Main Loop" );
	HandleEvents();			// handle keyboard / mouse events
	ClearColorAndDepth();	// if I just disable depth testing, this can disappear
	ComputePasses();		// multistage update of displayTexture
	BlitToScreen();			// fullscreen triangle copying displayTexture to screen
	ImguiPass();			// do all the gui stuff
	windowHandler.Swap();	// show contents of back buffer ( displayTexture + ImGui )
	FrameMark;				// tells tracy that this is the end of a frame
	return pQuit;			// break main loop when pQuit turns true
}

void engine::ComputePasses () {
	ZoneScoped;

// dummy draw
	// set up environment ( 0:blue noise, 1: accumulator )
	glBindImageTexture( 0, textures[ "Blue Noise" ], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8UI );
	glBindImageTexture( 1, textures[ "Accumulator" ], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8UI );

	// blablah draw something into accumulatorTexture
	glUseProgram( dummyDrawShader );
	glDispatchCompute( ( WIDTH + 15 ) / 16, ( HEIGHT + 15 ) / 16, 1 );
	glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT );

// postprocessing
	// set up environment ( 0:accumulator, 1:display )
	glBindImageTexture( 0, textures[ "Accumulator" ], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8UI );
	glBindImageTexture( 1, textures[ "Display Texture" ], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8UI );

	// shader for color grading ( color temp, contrast, gamma ... ) + tonemapping
	glUseProgram( tonemapShader );
	SendTonemappingParameters();
	glDispatchCompute( ( WIDTH + 15 ) / 16, ( HEIGHT + 15 ) / 16, 1 );
	glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT );

	// shader to apply dithering
		// ...

	// other postprocessing
		// ...

	// draw the orientation trident/gizmo
	trident.Update( textures[ "Display Texture" ] );
	glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT );

	// text rendering timestamp, as final step - required texture binds are handled internally
	textRenderer.Update( ImGui::GetIO().DeltaTime );
	textRenderer.Draw( textures[ "Display Texture" ] ); // displayTexture is the writeTarget
	glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT );
}

void engine::ClearColorAndDepth () {
	ZoneScoped;

	// clear the screen
	glClearColor( clearColor.x, clearColor.y, clearColor.z, clearColor.w );
	// glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glClear( GL_COLOR_BUFFER_BIT );

	const ImGuiIO &io = ImGui::GetIO();
	const int width = ( int ) io.DisplaySize.x;
	const int height = ( int ) io.DisplaySize.y;
	// prevent -1, -1 being passed on first frame, since ImGui hasn't rendered yet
	glViewport(0, 0, width > 0 ? width : WIDTH, height > 0 ? height : HEIGHT );
}

void engine::SendTonemappingParameters () {
	ZoneScoped;

	glUniform3fv( glGetUniformLocation( tonemapShader, "colorTempAdjust" ), 1, glm::value_ptr( GetColorForTemperature( tonemap.colorTemp ) ) );
	glUniform1i( glGetUniformLocation( tonemapShader, "tonemapMode" ), tonemap.tonemapMode );
	glUniform1f( glGetUniformLocation( tonemapShader, "gamma" ), tonemap.gamma );
}

void engine::BlitToScreen () {
	ZoneScoped;

	// bind the displayTexture and display its current state
	glBindImageTexture( 0, textures[ "Display Texture" ], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8UI );
	glUseProgram( displayShader );
	glBindVertexArray( displayVAO );
	const ImGuiIO &io = ImGui::GetIO();
	glUniform2f( glGetUniformLocation( displayShader, "resolution" ), io.DisplaySize.x, io.DisplaySize.y );
	glDrawArrays( GL_TRIANGLES, 0, 3 );
}

void engine::ImguiPass () {
	ZoneScoped;
	ImguiFrameStart();			// start the imgui frame

	// if ( true ) ImGui::ShowDemoWindow();	// show the demo window

	// menu container
	static bool showMenu = true;
	MenuLayout( &showMenu );

	// this has to be the final thing, because apparently order matters with modals
	QuitConf( &quitConfirm );	// show quit confirm window, if triggered
	ImguiFrameEnd();			// finish up the imgui stuff and put it in the framebuffer
}

void engine::HandleEvents () {
	ZoneScoped;

	const uint8_t *state = SDL_GetKeyboardState( NULL );

	const float bigStep = 0.120;
	const float lilStep = 0.008;

	// these will operate on the trident, now - will also set the basis for the main display
	if ( state[ SDL_SCANCODE_LEFT ] )
		trident.RotateY( ( SDL_GetModState() & KMOD_SHIFT ) ? bigStep : lilStep );
	if ( state[ SDL_SCANCODE_RIGHT ] )
		trident.RotateY( -( ( SDL_GetModState() & KMOD_SHIFT ) ? bigStep : lilStep ) );
	if ( state[ SDL_SCANCODE_UP ] )
		trident.RotateX( ( SDL_GetModState() & KMOD_SHIFT ) ? bigStep : lilStep );
	if ( state[ SDL_SCANCODE_DOWN ] )
		trident.RotateX( -( ( SDL_GetModState() & KMOD_SHIFT ) ? bigStep : lilStep ) );
	if ( state[ SDL_SCANCODE_PAGEUP ] )
		trident.RotateZ( -( ( SDL_GetModState() & KMOD_SHIFT ) ? bigStep : lilStep ) );
	if ( state[ SDL_SCANCODE_PAGEDOWN ] )
		trident.RotateZ( ( ( SDL_GetModState() & KMOD_SHIFT ) ? bigStep : lilStep ) );


//==============================================================================
// Need to keep this for pQuit handling ( force quit )
// In particular - checking for window close and the SDL_QUIT event can't really be determined
//  via the keyboard state, and then imgui needs it too, so can't completely kill the event
//  polling loop - maybe eventually I'll find a solution for this
	SDL_Event event;
	SDL_PumpEvents();
	while ( SDL_PollEvent( &event ) ) {
		// imgui event handling
		ImGui_ImplSDL2_ProcessEvent( &event );
		// swap out the multiple if statements for a big chained boolean setting the value of pQuit
		pQuit = ( event.type == SDL_QUIT ) || ( event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == windowHandler.ID() ) || ( event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_ESCAPE && SDL_GetModState() & KMOD_SHIFT );
		// this has to stay because it doesn't seem like ImGui::IsKeyReleased is stable enough to use
		if ( ( event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_ESCAPE ) || ( event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_X1 )  )
			quitConfirm = !quitConfirm;
	}
}
