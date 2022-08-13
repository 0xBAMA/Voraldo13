#ifndef DATASTRUCTS
#define DATASTRUCTS

/*==============================================================================
configuration structs for all the operations
==============================================================================*/

	// ...

/*==============================================================================
settings structs, for maintaining program state
==============================================================================*/
struct colorGradeParameters {
	int tonemapMode = 6;
	float gamma = 1.337f;
	float colorTemp = 6500.0f;
};

struct renderSettings {
	// application-wide
	bool showTrident = true;
	bool showTiming = true;
	// ImVec4 clearColor;
	glm::vec4 clearColor;

	// mipmaps need to be regenerated initially
	bool lightMipmapFlag = true;
	bool colorMipmapFlag = true;

	// render inputs
	float scaleFactor = 5.0f;
	float alphaCorrectionPower = 2.0f;
	float jitterAmount = 1.0f;
	float perspective = 0.2f;
	int volumeSteps = 400;

	// TODO: stuff for the spherical camera - use perspective factor to multiply uv, I think
		// maybe roll this into the rendermode? tbd
	bool sphericalCamera = false;
	int renderMode = 3;

	// accumulation stuff
	float blendFactor = 0.1f;
	uint32_t framesSinceLastInput = 0;
	int numFramesHistory = 8; // how long to run after the last input - configurable via menu
};

#endif
