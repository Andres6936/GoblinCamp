/* Copyright 2011 Ilkka Halila
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

#include "Goblin/Graphics/Tile/TileSetRenderer.hpp"
#include "Goblin/Graphics/Tile/TileSet.hpp"

#include "Goblin/Mechanism/Farmplot.hpp"
#include "Goblin/Mechanism/Stockpile.hpp"
#include "Goblin/Mechanism/Door.hpp"
#include "Goblin/Mechanism/SpawningPool.hpp"
#include "Goblin/Mechanism/Trap.hpp"

class DrawConstructionVisitor : public ConstructionVisitor
{
private:
	const TilesetRenderer* tileSetRenderer;
	const TileSet* tileSet;
	int screenX, screenY;
	const Coordinate& coordinate;
public:
	explicit DrawConstructionVisitor(const TilesetRenderer* tileSetRenderer, const TileSet* tileSet,
			int screenX, int screenY, const Coordinate& pos);

	~DrawConstructionVisitor();

	void Visit(FarmPlot * farmPlot);
	void Visit(Stockpile * stockpile);
	void Visit(Construction * construction);
	void Visit(SpawningPool * spawningPool);
	void Visit(Door * door);
	void Visit(Trap * trap);
	
};