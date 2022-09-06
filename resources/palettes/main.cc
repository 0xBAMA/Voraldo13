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
		j[ temp ] = 1; // placeholder
	}

	std::ofstream out( "palette.json" );
	out << j.dump();
	return 0;
}
