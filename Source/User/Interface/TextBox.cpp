// Joan Andrés (@Andres6936) Github.

#include "Goblin/User/Interface/TextBox.hpp"

void TextBox::Draw(int x, int y, TCODConsole* console)
{
	console->setAlignment(TCOD_CENTER);
	console->setDefaultBackground(TCODColor::darkGrey);
	console->rect(x + _x, y + _y, width, 1, true, TCOD_BKGND_SET);
	console->setDefaultBackground(TCODColor::black);
	if (value)
	{
		console->print(x + _x + width / 2, y + _y, value->c_str());
	}
	else
	{
		console->print(x + _x + width / 2, y + _y, getter().c_str());
	}
}

MenuResult TextBox::Update(int x, int y, bool clicked, TCOD_key_t key)
{
	std::string currValue;
	if (value)
	{
		currValue = *value;
	}
	else
	{
		currValue = getter();
	}
	if (key.vk == TCODK_BACKSPACE && currValue.size() > 0)
	{
		if (value)
		{
			value->erase(value->end() - 1);
		}
		else
		{
			currValue.erase(currValue.end() - 1);
			setter(currValue);
		}
		return KEYRESPOND;
	}
	else if (key.c >= ' ' && key.c <= '}' && key.c != '+' && key.c != '-')
	{
		if ((signed int)currValue.size() < width)
		{
			if (value)
			{
				(*value) += key.c;
			}
			else
			{
				currValue += key.c;
				setter(currValue);
			}
		}
		return KEYRESPOND;
	}
	return NOMENUHIT;
}

