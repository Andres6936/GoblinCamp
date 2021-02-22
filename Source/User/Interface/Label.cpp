// Joan Andrés (@Andres6936) Github.

#include "Goblin/User/Interface/Label.hpp"

void Label::Draw(int x, int y, TCODConsole* console)
{
	console->setAlignment(align);
	console->setDefaultForeground(TCODColor::white);
	console->print(x + _x, y + _y, text.c_str());
}

void LiveLabel::Draw(int x, int y, TCODConsole* console)
{
	console->setAlignment(align);
	console->setDefaultForeground(TCODColor::white);
	console->print(x + _x, y + _y, text().c_str());
}

