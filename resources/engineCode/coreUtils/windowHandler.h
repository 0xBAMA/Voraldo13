#ifndef WINDOWHANDLER_H
#define WINDOWHANDLER_H
#include "../includes.h"

class windowHandlerWrapper {
public:
	windowHandlerWrapper(){}

	void PreInit () {
		if ( SDL_Init( SDL_INIT_EVERYTHING ) != 0 )
			cout << "Error: " << SDL_GetError() << endl;
		SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER,       1 );
		SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL, 1 );
		SDL_GL_SetAttribute( SDL_GL_RED_SIZE,           8 );
		SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE,         8 );
		SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE,          8 );
		SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE,         8 );
		SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE,        24 );
		SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE,       8 );
		SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, MSAACount );
	}

	void Init ( int mode ) {
		SDL_DisplayMode dm;
		SDL_GetDesktopDisplayMode( 0, &dm );
		// putting this on different monitors
		int baseX = ( STARTONWINDOW - 1 ) * dm.w;
		// different window configurations
		switch ( mode ) {
			case 0: // little window, using WIDTH/HEIGHT defines in includes.h
				flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
				window = SDL_CreateWindow( "NQADE", baseX, 0, WIDTH, HEIGHT, flags );
				break;

			case 1: // fullscreen borderless
				flags = SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS;
				window = SDL_CreateWindow( "NQADE", baseX, 0, dm.w, dm.h, flags );
				break;

			case 2: // borderless floating
				flags = SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS;
				window = SDL_CreateWindow( "NQADE", 50 + baseX, 50, dm.w - 100, dm.h - 100, flags );
				break;

				// other modes?
		}
	}

	void OpenGLSetup () {
		// initialize OpenGL 4.3 + GLSL version 430
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_FLAGS, 0 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
		GLcontext = SDL_GL_CreateContext( window );
		SDL_GL_MakeCurrent( window, GLcontext );
		if ( gl3wInit() != 0 ) { // load OpenGL functions
			cout << "Failed to initialize OpenGL loader!" << endl;
			abort();
		}
		// basic OpenGL Config
		// glEnable( GL_DEPTH_TEST );
		// glEnable( GL_LINE_SMOOTH );
		// glPointSize( 3.0 );
		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	}

	void ToggleVSync () {
		vsyncState = !vsyncState;
		SDL_GL_SetSwapInterval( vsyncState ? 1 : 0 );
	}

	void Swap () {
		SDL_GL_SwapWindow( window );
	}

	uint32_t ID () {
		return SDL_GetWindowID( window );
	}

	void Kill () {
		SDL_GL_DeleteContext( GLcontext );
		SDL_DestroyWindow( window );
		SDL_Quit();
	}

	bool vsyncState = true;
	uint32_t flags;
	SDL_Window * window;
	SDL_GLContext GLcontext;
};

#endif
