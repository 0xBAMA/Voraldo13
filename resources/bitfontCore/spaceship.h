#ifndef SPACESHIP
#define SPACESHIP

#include <algorithm>
#include <random>
#include <unordered_map>
#include "letters.h"

struct bbox {
	glm::ivec3 mins;
	glm::ivec3 maxs;
};

// custom specialization of std::hash can be injected in namespace std
namespace std { // this is required for the use of std::unordered_map
	template<> struct hash< glm::ivec3 > {
		std::size_t operator()( glm::ivec3 const& s ) const noexcept {
			std::size_t h1 = std::hash< int >{}( s.x );
			std::size_t h2 = std::hash< int >{}( s.y );
			std::size_t h3 = std::hash< int >{}( s.z );
			return h1 ^ ( h2 << 4 ) ^ ( h3 << 8 );
		}
	};
}

class spaceshipGenerator {
public:
	spaceshipGenerator () {
		genRandomEngine();
	}

	letterSelector l;
	std::unordered_map< glm::ivec3, glm::vec4 > data;
	std::mt19937_64 rng;
	glm::vec4 palette[ 4 ];

	void genRandomEngine();
	void genRandomPalette();
	void setPalette( glm::vec4 values[ 4 ] );
	glm::vec4 getColorFromPalette( float t );
	bbox getModelBBox();
	void stampRandomGlyph();
	void shaveBlock();
	void flipBlock();
	void mirrorBlock();
	void squareBlock();
	void genSpaceship();
	void getData( std::vector< uint8_t > &data, int dim );
};

inline void spaceshipGenerator::genRandomEngine () {
	std::random_device r;
	rng = std::mt19937_64( r() );
}

inline void spaceshipGenerator::genRandomPalette () {
	// four glm::vec4's for the iq style palette
}

inline void spaceshipGenerator::setPalette ( glm::vec4 values[ 4 ] ) {
	// set the palette coefficients off of the input
}

inline glm::vec4 spaceshipGenerator::getColorFromPalette ( float t ) {
	// return a color for the palette
	return glm::vec4( 0.0 );
}

inline bbox spaceshipGenerator::getModelBBox () {
	// iterate through the hashmap and get min and max values on each index
	bbox temp;
	temp.mins = glm::ivec3( 0 );
	temp.maxs = glm::ivec3( 0 );
	return temp;
}

inline void spaceshipGenerator::stampRandomGlyph () {
	// put a random glyph into the hashmap
}

inline void spaceshipGenerator::shaveBlock () {
	// pick an axis - take some data off which is greater than the max minus some random distance
}

inline void spaceshipGenerator::flipBlock () {
	// pick an axis, subtract the bbox axis max from all points
}

inline void spaceshipGenerator::mirrorBlock () {
	// pick an axis, then create a copy of all data, mirrored about 0
}

inline void spaceshipGenerator::squareBlock () {
	// make sure all the data is in positive indices - all positive offsets from the negative faces
}

inline void spaceshipGenerator::genSpaceship ( /* this will have some arguments */ ) {
	// zero out the model, then run some number of operations
}

inline void spaceshipGenerator::getData ( std::vector< uint8_t > &data, int dim ) {
	// take the data from the model and put it in the block
}

#endif
