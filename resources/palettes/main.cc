#include <fstream>
#include <iostream>

#include "../JSON/json.h"
using json = nlohmann::json;

int main ( int argc, char const *argv[] ) {
	std::ifstream in( "paletteList.txt" );
	std::string temp;
	int count = 0;
	json j;

	while( in >> temp ) {
		count++;
		j[ temp ][ "index" ] = count;
		j[ temp ][ "label" ] = temp;
	}

	std::ofstream out( "palette.json" );
	out << j.dump();
	return 0;
}
