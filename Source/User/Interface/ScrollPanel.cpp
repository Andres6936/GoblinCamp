// Joan Andrés (@Andres6936) Github.

#include "Goblin/User/Interface/ScrollPanel.hpp"


void ScrollPanel::Draw(int x, int y, TCODConsole* console)
{
	if (scroll < 0)
	{
		scroll = 0;
	}
	if (scroll + height - 2 > contents->TotalHeight())
	{
		scroll = std::max(0, contents->TotalHeight() - (height - 2));
	}
	scrollBar = (int)((height - 3) * ((double)scroll / (double)std::max(1,
			contents->TotalHeight() - (height - 2)))) + 2;
	scrollBar = std::min(scrollBar, height - 4);

	if (drawFrame)
	{
		console->printFrame(x + _x, y + _y, width, height);
	}

	contents->Draw(x + _x + 1, y + _y + 1, scroll, width - 2, height - 2, console);
	console->putChar(x + _x + width - 2, y + _y + 1, TCOD_CHAR_ARROW_N, TCOD_BKGND_SET);
	console->putChar(x + _x + width - 2, y + _y + height - 2, TCOD_CHAR_ARROW_S, TCOD_BKGND_SET);
	console->putChar(x + _x + width - 2, y + _y + scrollBar, 219, TCOD_BKGND_SET);
}

MenuResult ScrollPanel::Update(int x, int y, bool clicked, TCOD_key_t key)
{
	if (x >= _x && x < _x + width && y >= _y && y < _y + height)
	{
		if (clicked)
		{
			if (x == _x + width - 2)
			{
				if (y == _y + 1)
				{
					scroll -= step;
				}
				else if (y == _y + height - 2)
				{
					scroll += step;
				}
				else if (y < _y + scrollBar)
				{
					scroll -= height;
				}
				else if (y > _y + scrollBar)
				{
					scroll += height;
				}
			}
			else
			{
				contents->Update(x - _x - 1, y - _y - 1 + scroll, clicked, key);
			}
		}
		else
		{
			contents->Update(x - _x - 1, y - _y - 1 + scroll, clicked, key);
		}
		return MENUHIT;
	}
	if (key.vk != TCODK_NONE) return contents->Update(x, y, clicked, key);
	return NOMENUHIT;
}

void ScrollPanel::GetTooltip(int x, int y, Tooltip* tooltip)
{
	if (x >= _x + 1 && x < _x + width - 1 && y >= _y + 1 && y < _y + height - 1)
	{
		contents->GetTooltip(x - _x - 1, y - _y - 1 + scroll, tooltip);
	}
}

