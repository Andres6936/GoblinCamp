// Joan Andrés (@Andres6936) Github.

#include "Goblin/User/Interface/Button.hpp"

void Button::Draw(int x, int y, TCODConsole* console)
{
	console->setBackgroundFlag(TCOD_BKGND_SET);
	if (selected)
	{
		console->setDefaultForeground(TCODColor::black);
		console->setDefaultBackground(TCODColor::white);
	}
	else
	{
		console->setDefaultForeground(TCODColor::white);
		console->setDefaultBackground(TCODColor::black);
	}
	console->setAlignment(TCOD_CENTER);
	console->printFrame(x + _x, y + _y, width, 3);
	console->print(x + _x + width / 2, y + _y + 1, text.c_str());
	console->setDefaultForeground(TCODColor::white);
	console->setDefaultBackground(TCODColor::black);
}

void LiveButton::Draw(int x, int y, TCODConsole* console)
{
	Button::Draw(x, y, console);
	if (selected)
	{
		console->setDefaultForeground(TCODColor::black);
		console->setDefaultBackground(TCODColor::white);
	}
	else
	{
		console->setDefaultForeground(TCODColor::white);
		console->setDefaultBackground(TCODColor::black);
	}
	console->print(x + _x + width / 2, y + _y + 1, textFunc().c_str());
	console->setDefaultForeground(TCODColor::white);
	console->setDefaultBackground(TCODColor::black);
}


MenuResult Button::Update(int x, int y, bool clicked, TCOD_key_t key)
{
	if (shortcut && (key.c == shortcut || key.vk == shortcut))
	{
		if (callback)
		{
			callback();
		}
		return (MenuResult)((dismiss ? DISMISS : 0) | KEYRESPOND);
	}
	if (x >= _x && x < _x + width && y >= _y && y < _y + 3)
	{
		selected = true;
		if (clicked && callback)
		{
			callback();
		}
		return (MenuResult)(((clicked && dismiss) ? DISMISS : 0) | MENUHIT);
	}
	else
	{
		selected = false;
		return NOMENUHIT;
	}

}

void ToggleButton::Draw(int x, int y, TCODConsole* console)
{
	console->setBackgroundFlag(TCOD_BKGND_SET);
	if (selected)
	{
		console->setDefaultForeground(TCODColor::black);
		console->setDefaultBackground(TCODColor::white);
	}
	else
	{
		console->setDefaultForeground(TCODColor::white);
		console->setDefaultBackground(isOn() ? TCODColor::blue : TCODColor::black);
	}
	console->setAlignment(TCOD_CENTER);
	console->printFrame(x + _x, y + _y, width, 3);
	console->print(x + _x + width / 2, y + _y + 1, text.c_str());
	console->setDefaultForeground(TCODColor::white);
	console->setDefaultBackground(TCODColor::black);
}