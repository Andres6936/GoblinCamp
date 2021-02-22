// Joan Andrés (@Andres6936) Github.

#include "Goblin/User/Interface/Frame.hpp"

void Frame::Draw(int x, int y, TCODConsole* console)
{
	console->printFrame(x + _x, y + _y, width, height, true, TCOD_BKGND_SET, title.c_str());
	UIContainer::Draw(x, y, console);
}
