#ifndef MENUENTRY_H
#define MENUENTRY_H

#include <vector>
#include <string>
using std::string;

enum class category_t { none, shapes, utilities, lighting, settings };
enum class elements_t { separator, sameline, floatSlider /*, ... */ };

struct element {
	// what type of element is it?
	elements_t elementType;

	// imgui label, label used with glGetUniformLocation
	string label;
	string shaderUniformLabel;

	// keep a reduntant list of all possible values that the element could take on
	// this avoids having to do some complex inheritance hierarchy / handling
	bool valueB;
	int valueI;
	float valueF;
	string valueS;
	// some type for a color picker, tbd

	// keep list of mins, maxs for ints, floats
	float minF;
	float maxF;
	int minI;
	int maxI;

	// ...

}

struct menuEntry {
	menuEntry ( std::string l, category_t c ) : label( l ), category( c ) {}
	std::string label;
	category_t category;
};

struct menuContainer {
	menuContainer () {}
	std::vector< menuEntry > entries;
};

#endif
