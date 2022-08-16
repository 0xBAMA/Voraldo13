#include "engine.h"

bool engine::MainLoop () {
	FrameMarkStart( "Main Loop" );
	render.framesSinceStartup++;
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
	Raymarch();
	Tonemap();

	// shader to apply dithering
	// other postprocessing

	if ( wantCapturePostprocessScreenshot ) {
		CapturePostprocessScreenshot();
	}

	TridentAndTiming();
}

void engine::SendRaymarchParameters () {
	ZoneScoped;
	const GLuint shader = shaders[ "Raymarch" ];
	const glm::mat3 inverseBasisMat = inverse( glm::mat3( -trident.basisX, -trident.basisY, -trident.basisZ ) );
	glUniformMatrix3fv( glGetUniformLocation( shader, "invBasis" ), 1, false, glm::value_ptr( inverseBasisMat ) );
	glUniform1f( glGetUniformLocation( shader, "scale" ), -render.scaleFactor );
	glUniform1f( glGetUniformLocation( shader, "blendFactor" ), render.blendFactor );
	glUniform1f( glGetUniformLocation( shader, "perspectiveFactor" ), render.perspective );
	glUniform4fv( glGetUniformLocation( shader, "clearColor" ), 1, glm::value_ptr( render.clearColor ) );
	glUniform2f( glGetUniformLocation( shader, "renderOffset" ), render.renderOffset.x, render.renderOffset.y );
	glUniform1f( glGetUniformLocation( shader, "alphaPower" ), render.alphaCorrectionPower );
	glUniform1i( glGetUniformLocation( shader, "numSteps" ), render.volumeSteps );
}

void engine::Raymarch () {
	ZoneScoped;
	// set up environment ( 0:blue noise, 1: accumulator ... )
	glBindImageTexture( 0, textures[ "Blue Noise" ], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8UI );
	glBindImageTexture( 1, textures[ "Accumulator" ], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F );
	glBindImageTexture( 2, textures[ "Color Block Front" ], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8UI );
	glBindImageTexture( 3, textures[ "Lighting Block" ], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F );

	glUseProgram( shaders[ "Raymarch" ] );
	SendRaymarchParameters();

	static std::random_device r;
	static std::seed_seq s{ r(), r(), r(), r(), r(), r(), r(), r(), r() };
	static auto gen = std::mt19937_64( s );
	static std::uniform_int_distribution< int > dist( 0, 512 );

	// tiled update of the accumulator texture
	constexpr int w = SSFACTOR * WIDTH;
	constexpr int h = SSFACTOR * HEIGHT;
	constexpr int t = TILESIZE;
	for ( int x = 0; x < w; x += t ) {
		for ( int y = 0; y < h; y += t ) {
			glUniform2i( glGetUniformLocation( shaders[ "Raymarch" ], "noiseOffset"), dist( gen ), dist( gen ) );
			glUniform2i( glGetUniformLocation( shaders[ "Raymarch" ], "tileOffset"), x, y );
			glDispatchCompute( t / 16, t / 16, 1 );
		}
	}
}

void engine::SendTonemappingParameters () {
	ZoneScoped;
	const GLuint shader = shaders[ "Tonemap" ];
	glUniform3fv( glGetUniformLocation( shader, "colorTempAdjust" ), 1, glm::value_ptr( GetColorForTemperature( tonemap.colorTemp ) ) );
	glUniform1i( glGetUniformLocation( shader, "tonemapMode" ), tonemap.tonemapMode );
	glUniform1f( glGetUniformLocation( shader, "gamma" ), tonemap.gamma );
}

void engine::Tonemap () {
	// shader for color grading ( color temp, contrast, gamma ... ) + tonemapping
	ZoneScoped;
	// set up environment ( 0:accumulator, 1:display, 2: blueNoise )
	glBindImageTexture( 0, textures[ "Accumulator" ], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F );
	glBindImageTexture( 1, textures[ "Display Texture" ], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8UI );
	glBindImageTexture( 2, textures[ "Blue Noise" ], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8UI );

	glUseProgram( shaders[ "Tonemap" ] );
	SendTonemappingParameters();
	glDispatchCompute( ( WIDTH + 15 ) / 16, ( HEIGHT + 15 ) / 16, 1 );
	glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT );
}

void engine::TridentAndTiming () {
	ZoneScoped;
	if ( render.showTrident ) {
		// draw the orientation trident/gizmo
		trident.Update( textures[ "Display Texture" ] );
		glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT );
	}
	if ( render.showTiming ) {
		// text rendering timestamp, as final step - required texture binds are handled internally
		textRenderer.Update( ImGui::GetIO().DeltaTime );
		textRenderer.Draw( textures[ "Display Texture" ] ); // displayTexture is the writeTarget
		glMemoryBarrier( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT );
	}
}

void engine::ClearColorAndDepth () {
	ZoneScoped;

	// clear the screen
	glClearColor( render.clearColor.x, render.clearColor.y, render.clearColor.z, render.clearColor.w );
	// glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glClear( GL_COLOR_BUFFER_BIT );

	const ImGuiIO &io = ImGui::GetIO();
	const int width = ( int ) io.DisplaySize.x;
	const int height = ( int ) io.DisplaySize.y;
	// prevent -1, -1 being passed on first frame, since ImGui hasn't rendered yet
	glViewport(0, 0, width > 0 ? width : WIDTH, height > 0 ? height : HEIGHT );
}

void engine::BlitToScreen () {
	ZoneScoped;

	glUseProgram( shaders[ "Display" ] );
	// bind the displayTexture and display its current state
	glBindImageTexture( 0, textures[ "Display Texture" ], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8UI );
	glBindVertexArray( displayVAO );
	const ImGuiIO &io = ImGui::GetIO();
	glUniform2f( glGetUniformLocation( shaders[ "Display" ], "resolution" ), io.DisplaySize.x, io.DisplaySize.y );
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

//==============================================================================
// Need to keep this for pQuit handling ( force quit ), and it makes scolling easier, too
//==============================================================================
	SDL_Event event;
	while ( SDL_PollEvent( &event ) ) {
		SDL_PumpEvents();
		// imgui event handling
		ImGui_ImplSDL2_ProcessEvent( &event );
		// swap out the multiple if statements for a big chained boolean setting the value of pQuit
		pQuit = ( event.type == SDL_QUIT ) || ( event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == windowHandler.ID() ) || ( event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_ESCAPE && SDL_GetModState() & KMOD_SHIFT );
		// this has to stay because it doesn't seem like ImGui::IsKeyReleased is stable enough to use
		if ( ( event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_ESCAPE ) || ( event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_X1 )  )
			quitConfirm = !quitConfirm;
		if ( !ImGui::GetIO().WantCaptureMouse ) {
			if ( event.type == SDL_MOUSEWHEEL ) {
				if ( event.wheel.y > 0 ) {
					render.scaleFactor -= 0.1f;
				} else if ( event.wheel.y < 0 ) {
					render.scaleFactor += 0.1f;
				}
			}
		}
		if ( event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_SHIFT && SDL_GetModState() & KMOD_CTRL && SDL_GetModState() & KMOD_ALT ) {
			windowHandler.ToggleVSync(); // control + alt + shift + v to toggle vsync
		}
	}

//==============================================================================
// the rest of the event checking just looks at the current input state
//==============================================================================
	if ( !ImGui::GetIO().WantCaptureKeyboard ) {
		constexpr float bigStep = 0.120;
		constexpr float lilStep = 0.008;
		const uint8_t *state = SDL_GetKeyboardState( NULL );
		// these will operate on the trident object, which retains state for block orientation
		if ( state[ SDL_SCANCODE_LEFT ] )
			trident.RotateY( ( SDL_GetModState() & KMOD_SHIFT ) ?  bigStep :  lilStep );
		if ( state[ SDL_SCANCODE_RIGHT ] )
			trident.RotateY( ( SDL_GetModState() & KMOD_SHIFT ) ? -bigStep : -lilStep );
		if ( state[ SDL_SCANCODE_UP ] )
			trident.RotateX( ( SDL_GetModState() & KMOD_SHIFT ) ?  bigStep :  lilStep );
		if ( state[ SDL_SCANCODE_DOWN ] )
			trident.RotateX( ( SDL_GetModState() & KMOD_SHIFT ) ? -bigStep : -lilStep );
		if ( state[ SDL_SCANCODE_PAGEUP ] )
			trident.RotateZ( ( SDL_GetModState() & KMOD_SHIFT ) ? -bigStep : -lilStep );
		if ( state[ SDL_SCANCODE_PAGEDOWN ] )
			trident.RotateZ( ( SDL_GetModState() & KMOD_SHIFT ) ?  bigStep :  lilStep );

		if ( state[ SDL_SCANCODE_1 ] )
			trident.SetViewFront();
		if ( state[ SDL_SCANCODE_2 ] )
			trident.SetViewRight();
		if ( state[ SDL_SCANCODE_3 ] )
			trident.SetViewBack();
		if ( state[ SDL_SCANCODE_4 ] )
			trident.SetViewLeft();
		if ( state[ SDL_SCANCODE_5 ] )
			trident.SetViewUp();
		if ( state[ SDL_SCANCODE_6 ] )
			trident.SetViewDown();

		// panning around, vim style
		if ( state[ SDL_SCANCODE_H ] )
			render.renderOffset.x += ( SDL_GetModState() & KMOD_SHIFT ) ?  10.0f :  1.0f;
		if ( state[ SDL_SCANCODE_L ] )
			render.renderOffset.x -= ( SDL_GetModState() & KMOD_SHIFT ) ?  10.0f :  1.0f;
		if ( state[ SDL_SCANCODE_J ] )
			render.renderOffset.y += ( SDL_GetModState() & KMOD_SHIFT ) ?  10.0f :  1.0f;
		if ( state[ SDL_SCANCODE_K ] )
			render.renderOffset.y -= ( SDL_GetModState() & KMOD_SHIFT ) ?  10.0f :  1.0f;

	}
}

// Function to get color temperature from shadertoy user BeRo
// from the author:
//   Color temperature (sRGB) stuff
//   Copyright (C) 2014 by Benjamin 'BeRo' Rosseaux
//   Because the german law knows no public domain in the usual sense,
//   this code is licensed under the CC0 license
//   http://creativecommons.org/publicdomain/zero/1.0/
// Valid from 1000 to 40000 K (and additionally 0 for pure full white)
glm::vec3 engine::GetColorForTemperature ( float temperature ) {
	// Values from:
	// http://blenderartists.org/forum/showthread.php?270332-OSL-Goodness&p=2268693&viewfull=1#post2268693
	glm::mat3 m = ( temperature <= 6500.0f )
	? glm::mat3( glm::vec3( 0.0f, -2902.1955373783176f, -8257.7997278925690f ),
				glm::vec3( 0.0f, 1669.5803561666639f, 2575.2827530017594f ),
				glm::vec3( 1.0f, 1.3302673723350029f, 1.8993753891711275f ) )
	: glm::mat3( glm::vec3( 1745.0425298314172f, 1216.6168361476490f, -8257.7997278925690f ),
				glm::vec3( -2666.3474220535695f, -2173.1012343082230f, 2575.2827530017594f ),
				glm::vec3( 0.55995389139931482f, 0.70381203140554553f, 1.8993753891711275f ) );

	return glm::mix( glm::clamp( glm::vec3( m[ 0 ] / ( glm::vec3( glm::clamp( temperature, 1000.0f, 40000.0f ) ) +
		m[ 1 ] ) + m[ 2 ] ), glm::vec3( 0.0f ), glm::vec3( 1.0f ) ), glm::vec3( 1.0f ), glm::smoothstep( 1000.0f, 0.0f, temperature ) );
}

void engine::CapturePostprocessScreenshot () {
	wantCapturePostprocessScreenshot = false;
	std::vector<uint8_t> imageBytesToSaveP;
	imageBytesToSaveP.resize( WIDTH * HEIGHT * 4 );
	glBindTexture( GL_TEXTURE_2D, textures[ "Display Texture" ] );
	glGetTexImage( GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, &imageBytesToSaveP.data()[ 0 ] );
	Image screenshotP( WIDTH, HEIGHT, &imageBytesToSaveP.data()[ 0 ] );
	screenshotP.FlipVertical();
	auto nowP = std::chrono::system_clock::now();
	auto inTime_tP = std::chrono::system_clock::to_time_t( nowP );
	std::stringstream ssP;
	ssP << std::put_time( std::localtime( &inTime_tP ), "screenshots/Voraldo13ssP-%Y-%m-%d %X.png" );
	screenshotP.Save( ssP.str(), LODEPNG );
}
