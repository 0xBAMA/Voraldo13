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

struct rendererState {
	// mipmaps need to be regenerated
	bool lightMipmapFlag = true;
	bool colorMipmapFlag = true;
	uint32_t framesSinceLastInput = 0;
	//...
};

struct renderSettings {
	bool showTrident = true;
	bool showTiming = true;
	ImVec4 clearColor;
	float blendFactor = 0.1f;
	float scaleFactor = 5.0f;
	float alphaCorrectionPower = 2.0f;
	float jitterAmount = 1.0f;
	float perspective = 0.2f;
	int volumeSteps = 400;
	int renderMode = 3;
	int numFramesHistory = 8;
};

#endif
