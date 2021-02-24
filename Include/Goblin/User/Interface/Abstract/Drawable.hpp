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

#include <functional>

#include <libtcod.hpp>
#include "Goblin/User/Interface/Tooltip.hpp"
#include <Goblin/User/Interface/Enum/MenuResult.hpp>


/**
 * Abstract class
 */
class Drawable
{

protected:

	int _x;

	int _y;

	int width;

	int height;

	std::function<bool()> visible;

	std::function<void(int, int, Tooltip*)> getTooltip;

public:

	// Constructor

	Drawable(int x, int y, int nwidth, int nheight) :
			_x(x), _y(y), width(nwidth), height(nheight), visible(0), getTooltip(0)
	{
	}

	// Destructor
	virtual ~Drawable() = default;

	// Abstract Methods

	virtual void Draw(int, int, TCODConsole*) = 0;

	// Virtual Methods

	virtual MenuResult Update(int x, int y, bool clicked, TCOD_key_t key)
	{
		return (x >= _x && x < _x + height && y >= _y && y < _y + height) ? MENUHIT : NOMENUHIT;
	}

	// Getters

	int Height()
	{
		return height;
	}

	bool Visible()
	{
		return !visible || visible();
	}

	virtual void GetTooltip(int x, int y, Tooltip* tooltip)
	{
		if (getTooltip) getTooltip(x, y, tooltip);
	}

	// Setters

	void SetVisible(std::function<bool()> nvisible)
	{
		visible = nvisible;
	}

	void SetTooltip(std::function<void(int, int, Tooltip*)> ntooltip)
	{
		getTooltip = ntooltip;
	}
};
