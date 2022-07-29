#ifndef MENUENTRY_H
#define MENUENTRY_H

#include <vector>
#include <string>

enum class category_t { shapes, utilities, lighting, settings };

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
