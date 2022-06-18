// Joan Andrés (@Andres6936) Github.

#include "Goblin/User/Interface/UIContainer.hpp"


void UIContainer::AddComponent(Drawable* component)
{
	components.push_back(component);
}

void UIContainer::Draw(int x, int y, TCODConsole* console)
{
	for (Drawable* component : components)
	{
			if (component->Visible())
		{
			component->Draw(x + _x, y + _y, console);
		}
	}
}

MenuResult UIContainer::Update(int x, int y, bool clicked, TCOD_key_t key)
{
	for (Drawable* component : components)
	{
			if (component->Visible())
		{
			MenuResult result = component->Update(x - _x, y - _y, clicked, key);
			if (!(result & NOMENUHIT))
			{
				return result;
			}
		}
	}

	if (x >= _x && x < _x + width && y >= _y && y < _y + height)
	{
		return MENUHIT;
	}

	return NOMENUHIT;
}

void UIContainer::GetTooltip(int x, int y, Tooltip* tooltip)
{
	Drawable::GetTooltip(x, y, tooltip);
	for (Drawable* component : components)
	{
			if (component->Visible())
		{
			component->GetTooltip(x - _x, y - _y, tooltip);
		}
	}
}

UIContainer::~UIContainer()
{
	for (Drawable* & component : components)
	{
		delete component;
	}
}


