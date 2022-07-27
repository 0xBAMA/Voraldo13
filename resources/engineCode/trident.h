#include "includes.h"
#ifndef TRIDENT_H
#define TRIDENT_H

// I had always heard this referred to as a gizmo - but looking for reference
//   images for this implementation, I found some package that was on sale for
//   second life calling it an 'orient trident' and now I am absolutely calling
//   it that. This will also be tracking the state of the basis vectors used by
//   the renderer + providing the rotation functions used by the controls.

class orientTrident {
public:
	orientTrident () {
		needsRedraw = true;
		basisX = glm::vec3( 1.0f, 0.0f, 0.0f );
		basisY = glm::vec3( 0.0f, 1.0f, 0.0f );
		basisZ = glm::vec3( 0.0f, 0.0f, 1.0f );

		// create the image

	}

	void PassInShaders ( GLuint generate, GLuint copy ) {
		generateShader = generate;
		copyShader = copy;
	}

	// rotate functions X,Y,Z with an amount to rotate by ( pos/neg )
		// each of these will set needsRedraw to true, since the state changes

	void Update ( GLuint writeTarget ) {

		// check needsRedraw, invoke generateShader if true
			// while relatively cheap, this is still the most expensive part of this
			//   operation, so the idea is to write the result to a texture and copy
			//   that until the state changes and that is no longer good data

		// copy to the writeTarget with copyShader

	}

	glm::vec3 basisX;
	glm::vec3 basisY;
	glm::vec3 basisZ;

	// size in terms of font glyphs - sizes the dispatches
	glm::ivec2 blockDimensions{ 20, 7 };

private:
	GLuint tridentImage;
	GLuint generateShader;
	GLuint copyShader;
	bool needsRedraw; // generate shader needs to run again with new values
};

#endif
