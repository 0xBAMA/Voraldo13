#ifndef DATASTRUCTS
#define DATASTRUCTS

/*==============================================================================
configuration structs for all the operations
==============================================================================*/
	// does it make sense to do these as separate values? I think it can work on
	// a json record, with labels as the uniform labels - iterate through and send
	// but will also need to know the type on them... maybe part of the json

	// I just want to avoid having the data load that there was last time, it got
	// extremely disorganized - construct the record inside of the if statement
	// on the button to invoke... I still want to be able to support the scripting
	// interface that 12 had, it was fairly complete

/*==============================================================================
settings structs, for maintaining program state
==============================================================================*/
struct colorGradeParameters {
	int tonemapMode = 6;
	float gamma = 1.337f;
	float colorTemp = 6500.0f;
};

struct renderState {
	// application-wide
	bool showTrident = true;
	bool showTiming = true;
	glm::vec4 clearColor = glm::vec4( 0.0f, 0.0f, 0.0f, 1.0f );

	// mipmaps need to be regenerated initially
	bool lightMipmapFlag = true;
	bool colorMipmapFlag = true;

	// render inputs
	glm::vec2 renderOffset = glm::vec2( 0.0, 0.0 );
	float scaleFactor = 5.0f;
	float alphaCorrectionPower = 2.0f;
	float jitterAmount = 1.0f;
	float perspective = 0.2f;
	int volumeSteps = 500;

	// TODO: stuff for the spherical camera - use perspective factor to multiply uv, I think
		// maybe roll this into the rendermode? tbd
	bool sphericalCamera = false;
	int renderMode = 3;

	// accumulation stuff
	float blendFactor = 0.618f;
	uint32_t framesSinceStartup = 0;
	uint32_t framesSinceLastInput = 0;
	int numFramesHistory = 8; // how long to run after the last input - configurable via menu
};

#endif
