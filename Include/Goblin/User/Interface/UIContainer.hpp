// Joan Andrés (@Andres6936) Github.

#ifndef GOBLINCAMP_UICONTAINER_HPP
#define GOBLINCAMP_UICONTAINER_HPP

#include <vector>
#include <libtcod/libtcod.hpp>

#include <Goblin/User/Interface/Abstract/Drawable.hpp>

class UIContainer : public Drawable
{

protected:

	std::vector<Drawable*> components;

public:

	UIContainer(std::vector<Drawable*> ncomponents, int nx, int ny, int nwidth, int nheight) :
			Drawable(nx, ny, nwidth, nheight), components(ncomponents)
	{
	}

	~UIContainer();

	void AddComponent(Drawable* component);

	virtual void Draw(int, int, TCODConsole*);

	virtual MenuResult Update(int, int, bool, TCOD_key_t);

	virtual void GetTooltip(int, int, Tooltip*);
};

#endif //GOBLINCAMP_UICONTAINER_HPP
