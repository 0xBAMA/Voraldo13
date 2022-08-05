#ifndef MENUENTRY_H
#define MENUENTRY_H

#include <vector>
#include <string>

enum class category_t { none, shapes, utilities, lighting, settings };
enum class elements_t { separator, sameline, floatSlider /*, ... */ };

struct menuEntry {
	menuEntry ( std::string l, category_t c, bool special = false ) :
		label( l ), category( c ), requiresSpecialHandling( special ) {}

	std::string label;
	category_t category;

	// is the vector of interface elements sufficient, or are we doing the layout manually in code
	bool requiresSpecialHandling;

	// vector of layout formatting blocks to be executed in order for the menu entry
	std::vector< elements_t > interfaceElements;
};

struct menuContainer {
	menuContainer () {}
	std::vector< menuEntry > entries;
};

#endif
