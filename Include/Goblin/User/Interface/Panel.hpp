// Joan Andrés (@Andres6936) Github.

#ifndef GOBLINCAMP_PANEL_HPP
#define GOBLINCAMP_PANEL_HPP

#include <libtcod.hpp>
#include <Goblin/User/Interface/Abstract/Drawable.hpp>

class Panel : public Drawable
{

public:

	Panel(int nwidth, int nheight) :
			Drawable(0, 0, nwidth, nheight)
	{
	}

	void ShowModal();

	virtual void Open();

	virtual void Close();

	virtual void selected(int);
};


#endif //GOBLINCAMP_PANEL_HPP
