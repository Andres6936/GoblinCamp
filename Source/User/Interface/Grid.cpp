// Joan Andrés (@Andres6936) Github.

#include "Goblin/User/Interface/Grid.hpp"

void Grid::AddComponent(Drawable* component)
{
	contents.push_back(component);
}

void Grid::RemoveAll()
{
	for (std::vector<Drawable*>::iterator it = contents.begin(); it != contents.end(); it++)
	{
		delete *it;
	}
	contents.clear();
}

void Grid::Draw(int x, int y, TCODConsole* console)
{
	Draw(x + _x, y + _y, 0, width, height, console);
}

void Grid::Draw(int x, int y, int scroll, int _width, int _height, TCODConsole* console)
{
	int col = 0;
	int top = y;
	int bottom = y + scroll + _height;
	int colWidth = _width / cols;
	int rowHeight = 0;
	for (std::vector<Drawable*>::iterator it = contents.begin();
		 it != contents.end() && y < bottom; it++)
	{
		Drawable* component = *it;
		if (component->Visible())
		{
			if (y - scroll >= top && y + component->Height() <= bottom)
			{
				component->Draw(x + colWidth * col, y - scroll, console);
			}
			rowHeight = std::max(rowHeight, component->Height());
			col++;
			if (col >= cols)
			{
				col = 0;
				y += rowHeight;
				rowHeight = 0;
			}
		}
	}
}

int Grid::TotalHeight()
{
	int col = 0;
	int rowHeight = 0;
	int y = 0;
	for (std::vector<Drawable*>::iterator it = contents.begin(); it != contents.end(); it++)
	{
		Drawable* component = *it;
		if (component->Visible())
		{
			rowHeight = std::max(rowHeight, component->Height());
			col++;
			if (col >= cols)
			{
				col = 0;
				y += rowHeight;
				rowHeight = 0;
			}
		}
	}
	return y + rowHeight;
}

MenuResult Grid::Update(int x, int y, bool clicked, TCOD_key_t key)
{
	int col = 0;
	int colWidth = width / cols;
	int rowHeight = 0;
	for (std::vector<Drawable*>::iterator it = contents.begin(); it != contents.end(); it++)
	{
		Drawable* component = *it;
		if (component->Visible())
		{
			MenuResult result = component->Update(x - _x - col * (colWidth - 1), y - _y, clicked,
					key);
			if (!(result & NOMENUHIT))
			{
				return result;
			}
			rowHeight = std::max(rowHeight, component->Height());
			col++;
			if (col >= cols)
			{
				col = 0;
				y -= rowHeight;
				rowHeight = 0;
			}
		}
	}
	return NOMENUHIT;
}

void Grid::GetTooltip(int x, int y, Tooltip* tooltip)
{
	Drawable::GetTooltip(x, y, tooltip);
	int col = 0;
	int colWidth = width / cols;
	int rowHeight = 0;
	for (std::vector<Drawable*>::iterator it = contents.begin(); it != contents.end(); it++)
	{
		Drawable* component = *it;
		if (component->Visible())
		{
			component->GetTooltip(x - _x - col * colWidth, y - _y, tooltip);
			rowHeight = std::max(rowHeight, component->Height());
			col++;
			if (col >= cols)
			{
				col = 0;
				y -= rowHeight;
				rowHeight = 0;
			}
		}
	}
}

