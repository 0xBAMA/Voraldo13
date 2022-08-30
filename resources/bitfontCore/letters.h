#ifndef LETTERS
#define LETTERS

#include "../JSON/json.h"
using json = nlohmann::json;

class letter {
public:
	bool flaggedForRemoval = false;
	bool nfg = false;

	std::vector< std::vector< unsigned char > > data;
	void buildrow ( int r, std::string s ) {
		std::vector< unsigned char > ints;
		for ( unsigned int i = 0; i < s.size(); i++ ) {
			if ( s[ i ] == '.' ) {
				ints.push_back( 0 );
			} else if ( s[ i ] == '@' ) {
				ints.push_back( 1 );
			} else {
				nfg = true;
				break;
			}
		}
		data[ r ] = ints;
	}

	void print () {
		for ( auto row : data ) {
			for( auto elem : row ) {
				std::cout << ( int ) elem;
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}

	bool check () {
		unsigned int firstrow = data.empty() ? 0 : data[ 0 ].size();
		if ( !firstrow ) return false;
		for ( unsigned int i = 0; i < data.size(); i++ )
			if ( data[ i ].size() != firstrow )
				return false;
		return true;
	}

	friend bool operator== ( const letter& l1, const letter& l2 ) {
		if ( l1.data.size() != l2.data.size() ) return false; // y dim doesn'nt match
		if ( l1.data[ 0 ].size() != l2.data[ 0 ].size() ) return false; // x dim doesn't match
		for ( unsigned int i = 0; i < l1.data.size(); i++ ) {
			for ( unsigned int j = 0; j < l1.data[ 0 ].size(); j++ ) {
				if ( l1.data[ i ][ j ] != l2.data[ i ][ j ] ) {
					return false;
				}
			}
		}
		return true;
	}
};

class letterSelector {
public:

	letterSelector () {
		if ( glyphs.size() == 0 ) {
			ReadUintModel();
		}

		// std::cout << "There are " << glyphs.size() << " glyphs" << endl;
		// for ( auto& g : glyphs ) {
			// g.print();
		// }
	}


	// make static?
	std::vector< letter > glyphs;

	std::string UintToString ( uint64_t data, int width ) {
		std::string temp;
		for ( int i = 0; i <= width; i++ ) {
			temp += ( data % 2 ) ? '@' : '.';
			data = data >> 1;
		}
		reverse( temp.begin(), temp.end() );
		return temp;
	}

	void ReadUintModel () {
		std::ifstream i( "resources/bitfontCore/uintEncoded.json" );
		json j; i >> j;
		for ( auto& element : j ) { // per character
			int loadX = element[ "x" ];
			int loadY = element[ "y" ];
			int currentY = 0;
			letter temp;
			temp.data.resize( element[ "d" ].size() );
			for ( int i = 0; i < loadY; i++ ) {
				temp.buildrow( currentY, UintToString( element[ "d" ][ i ], loadX ) );
				currentY++;
			}
			if( !temp.nfg ) {
				glyphs.push_back( temp );
			}
		}
	}


	void write ( int x, int y, int z, uint8_t color [ 4 ], std::vector< uint8_t > &data, int dim ) {
		// this function has to do bounds checking
		if( x < 0 || x >= dim || y < 0 || y >= dim || z < 0 || z >= dim ) return;
		int index = 4 * ( x + y * dim + z * dim * dim );
		data[ index+0 ] = color[ 0 ];
		data[ index+1 ] = color[ 1 ];
		data[ index+2 ] = color[ 2 ];
		data[ index+3 ] = color[ 3 ];
	}

	void writeLetterToBlock ( letter l, int x, int y, int z, int dir, int scale, uint8_t color[ 4 ], std::vector< uint8_t > &data, int dim ) {
		int xdim = l.data.size();
		int ydim = l.data[ 1 ].size();
		for( int xx = 0; xx < xdim; xx++ )
		for( int yy = 0; yy < ydim; yy++ )
		for( int xs = 0; xs < scale; xs++ )
		for( int ys = 0; ys < scale; ys++ )
		for( int zs = 0; zs < scale; zs++ )
		switch( dir ) {
			case 1: if ( l.data[ xx ][ yy ] == 1 ) write(x+xx*scale+xs,y+yy*scale+ys,z+zs,color,data,dim); break;
			case 2: if ( l.data[ xx ][ yy ] == 1 ) write(x-xx+xs,y+yy*scale+ys,z+zs,color,data,dim); break;
			case 3: if ( l.data[ xx ][ yy ] == 1 ) write(x+xx*scale+xs,y+yy*scale+ys,z+zs,color,data,dim); break;
			case 4: if ( l.data[ xx ][ yy ] == 1 ) write(x+xx*scale+xs,y-yy+ys,z+zs,color,data,dim); break;
			case 5: if ( l.data[ xx ][ yy ] == 1 ) write(x+xs,y+yy*scale+ys,z+xx*scale+zs,color,data,dim); break;
			case 6: if ( l.data[ xx ][ yy ] == 1 ) write(x+xs,y+yy*scale+ys,z-xx+zs,color,data,dim); break;
			default: break;
		}
	}

	// function to populate the block with
		// n glyphs, with m variants, of some color
		// takes the data block by reference
	void blockulate ( int n, int m, int dim, glm::vec4 color, std::vector< uint8_t > &data ) {
		std::mt19937_64 gen;
		std::random_device r;
		std::seed_seq s{ r(), r(), r(), r(), r(), r(), r(), r(), r() };
		gen = std::mt19937_64( s );
		std::uniform_int_distribution< int > colorDistribution( -15, 15 );
		std::uniform_int_distribution< int > glyphPick( 0, glyphs.size() );
		std::uniform_int_distribution< int > locationDistribution( 0, dim );
		std::uniform_int_distribution< int > directionDistribution( 1, ( m % 6 ) + 1 );
		std::uniform_int_distribution< int > scaleDistribution( 1, ( m / 6 ) + 1 );
		unsigned char col[ 4 ];
		for(int i = 0; i < n; i++) { // n letters
			// randomize position and color (a small amount with the <random> distributions)
			for(int j = 0; j < 4; j++) {
				col[ j ] = ( color[ j ] * 255 ) + colorDistribution( gen );
			}
			writeLetterToBlock( glyphs[ glyphPick( gen ) ], locationDistribution( gen ), locationDistribution( gen ), locationDistribution( gen ), directionDistribution( gen ), directionDistribution( gen ), col, data, dim );
		}
	}

};

#endif
