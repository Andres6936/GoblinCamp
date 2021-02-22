// Joan Andrés (@Andres6936) Github.

#include "Goblin/User/Interface/Spinner.hpp"


inline int numDigits(int num)
{
	int tmp = num;
	int digits = 1;
	if (tmp < 0)
	{
		digits++;
		tmp = -tmp;
	}
	while (num >= 10)
	{
		digits++;
		num /= 10;
	}
	return digits;
}

void Spinner::Draw(int x, int y, TCODConsole* console)
{
	console->setAlignment(TCOD_CENTER);
	int val = value ? *value : getter();
	console->print(x + _x + width / 2, y + _y, "- %d +", val);
}

MenuResult Spinner::Update(int x, int y, bool clicked, TCOD_key_t key)
{
	if ((x >= _x && x < _x + width && y == _y) || (key.vk == TCODK_KPADD || key.vk == TCODK_KPSUB))
	{
		if (clicked || (key.vk == TCODK_KPADD || key.vk == TCODK_KPSUB))
		{
			int curr = value ? *value : getter();
			int adj = UI::Inst()->ShiftPressed() ? 10 : 1;
			int strWidth = 4 + numDigits(curr);
			if (x == _x + width / 2 - strWidth / 2 || key.vk == TCODK_KPSUB)
			{
				if (value)
				{
					(*value) = std::max(min, curr - adj);
				}
				else
				{
					setter(std::max(min, curr - adj));
				}
			}
			else if (x == _x + width / 2 + (strWidth - 1) / 2 || key.vk == TCODK_KPADD)
			{
				if (value)
				{
					(*value) = std::max(min, curr + adj);
				}
				else
				{
					setter(std::max(min, curr + adj));
				}
			}
		}
		return MENUHIT;
	}
	return NOMENUHIT;
}

