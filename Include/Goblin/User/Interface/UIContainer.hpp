// Joan Andrés (@Andres6936) Github.

#ifndef GOBLINCAMP_UICONTAINER_HPP
#define GOBLINCAMP_UICONTAINER_HPP

#include <vector>
#include <libtcod.hpp>

#include <Goblin/User/Interface/Abstract/Drawable.hpp>

/**
 * Used for store pinter to Drawable component, the responsibility for
 * eliminating these component is transferred to this component, which will
 * do so once the life cycle of this component is over.
 */
class UIContainer : public Drawable
{

protected:

	std::vector<Drawable*> components {};

public:

	UIContainer(int nx, int ny, int nwidth, int nheight) :
			Drawable(nx, ny, nwidth, nheight)
	{
	}

	~UIContainer() override;

	void AddComponent(Drawable* component);

	void Draw(int, int, TCODConsole*) override;

	void GetTooltip(int, int, Tooltip*) override;

	MenuResult Update(int, int, bool, TCOD_key_t) override;
};

#endif //GOBLINCAMP_UICONTAINER_HPP
