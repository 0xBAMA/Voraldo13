

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
		ReadUintModel();
		// std::cout << "There are " << glyphs.size() << " glyphs" << endl;
		// for ( auto& g : glyphs ) {
			// g.print();
		// }
	}


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

};
