// Joan Andrés (@Andres6936) Github.

#include "Goblin/User/Interface/Dialog.hpp"

Dialog::Dialog(Drawable* ncontents, std::string ntitle, int nwidth, int nheight) :
		Panel(nwidth, nheight), title(ntitle), contents(ncontents)
{
	_x = (WindowConfig::getWidth() - nwidth) / 2;
	_y = (WindowConfig::getHeight() - nheight) / 2;
}


void Dialog::SetTitle(std::string ntitle)
{
	title = ntitle;
}

void Dialog::SetHeight(int nheight)
{
	height = nheight;
	_x = (WindowConfig::getWidth() - width) / 2;
	_y = (WindowConfig::getHeight() - height) / 2;
}

void Dialog::Draw(int x, int y, TCODConsole* console)
{
	console->printFrame(_x, _y, width, height, true, TCOD_BKGND_SET,
			title.empty() ? 0 : title.c_str());
	contents->Draw(_x, _y, console);
}

MenuResult Dialog::Update(int x, int y, bool clicked, TCOD_key_t key)
{
	return contents->Update(x - _x, y - _y, clicked, key);
}

void Dialog::GetTooltip(int x, int y, Tooltip* tooltip)
{
	Drawable::GetTooltip(x, y, tooltip);
	contents->GetTooltip(x - _x, y - _y, tooltip);
}


