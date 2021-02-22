// Joan Andrés (@Andres6936) Github.

#ifndef GOBLINCAMP_SCROLLABLE_HPP
#define GOBLINCAMP_SCROLLABLE_HPP

#include <libtcod.hpp>
#include "Goblin/User/Interface/Tooltip.hpp"
#include <Goblin/User/Interface/Enum/MenuResult.hpp>

/**
 * Abstract class
 */
class Scrollable
{

public:

	// Destructor

	virtual ~Scrollable()
	{
	}

	virtual void Draw(int x, int y, int scroll, int width, int height, TCODConsole*) = 0;

	virtual int TotalHeight() = 0;

	virtual MenuResult Update(int x, int y, bool clicked, TCOD_key_t key)
	{
		return NOMENUHIT;
	}

	virtual void GetTooltip(int x, int y, Tooltip* tooltip)
	{
	};
};

#endif //GOBLINCAMP_SCROLLABLE_HPP
