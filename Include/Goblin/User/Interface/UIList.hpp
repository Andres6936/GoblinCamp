/* Copyright 2010-2011 Ilkka Halila
 This file is part of Goblin Camp.
 
 Goblin Camp is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 Goblin Camp is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License 
 along with Goblin Camp. If not, see <http://www.gnu.org/licenses/>.*/
#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <libtcod.hpp>

#include "Goblin/User/Interface/Tooltip.hpp"
#include <Goblin/User/Interface/Enum/MenuResult.hpp>
#include <Goblin/User/Interface/Abstract/Drawable.hpp>
#include <Goblin/User/Interface/Abstract/Scrollable.hpp>

template<class Container>
class UIList : public Drawable, public Scrollable
{
private:

	/**
	 * The type of the elements.
	 */
	using ValueType = typename Container::value_type;

	Container* items;

	int selection;

	bool selectable;

	std::function<void(int)> onclick;

	std::function<void(ValueType, Tooltip*)> getTooltip;

	std::function<void(ValueType, int, int, int, int, bool, TCODConsole*)> draw;

public:

	/**
	 * @param nitems Pointer to the items
	 * @param x Coordinate in the X axis for begin to draw.
	 * @param y Coordinate in the Y axis for begin to draw.
	 * @param nwidth The width in characters of zone to draw.
	 * @param nheight The height in characters of zone to draw.
	 * @param ndraw
	 * @param nonclick
	 * @param nselectable
	 * @param ntooltip
	 */
	UIList(
			Container* nitems, int x, int y, int nwidth, int nheight,
			std::function<void(ValueType, int, int, int, int, bool, TCODConsole*)> ndraw,
			std::function<void(int)> nonclick = 0, bool nselectable = false,
			std::function<void(ValueType, Tooltip*)> ntooltip = 0
	) :
			Drawable(x, y, nwidth, nheight),
			items(nitems),
			selectable(nselectable),
			selection(-1),
			draw(ndraw),
			getTooltip(ntooltip),
			onclick(nonclick)
	{
	}

	void Draw(int x, int y, TCODConsole* console) override
	{
		console->setAlignment(TCOD_LEFT);
		int count = 0;
		for (typename Container::iterator it = items->begin();
			 it != items->end() && count < height; it++)
		{
			ValueType item = *it;
			draw(item, count, x + _x, y + _y + count, width, selection == count, console);
			count++;
		}
	}

	void Draw(int x, int y, int scroll, int _width, int _height, TCODConsole* console) override
	{
		console->setAlignment(TCOD_LEFT);
		int count = 0;
		for (typename Container::iterator it = items->begin(); it != items->end(); it++)
		{
			ValueType item = *it;
			if (count >= scroll && count < scroll + _height)
			{
				draw(item, count, x, y + (count - scroll), _width, selection == count, console);
			}
			count++;
		}
	}

	int TotalHeight() override
	{
		return items->size();
	}

	MenuResult Update(int x, int y, bool clicked, TCOD_key_t key) override
	{
		if (x >= _x && x < _x + width && y >= _y && y < _y + height)
		{
			if (clicked)
			{
				if (selectable)
				{
					selection = y - _y;
				}
				if (onclick)
				{
					onclick(y - _y);
				}
			}
			return MENUHIT;
		}
		return NOMENUHIT;
	}

	void GetTooltip(int x, int y, Tooltip* tooltip) override
	{
		if (getTooltip)
		{
			if (x >= _x && x < _x + width && y >= _y && y < _y + width &&
				y - _y < (signed int)items->size())
			{
				typename Container::iterator it = items->begin();
				for (int i = 0; i < (y - _y); i++)
				{
					it++;
				}
				getTooltip(*it, tooltip);
			}
		}
	}

	int Selected()
	{
		if (selection >= 0 && selection < (signed int)items->size())
		{
			return selection;
		}
		return -1;
	}

	void Select(int i)
	{
		selection = i;
	}
};