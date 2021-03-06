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

#include <boost/functional/hash.hpp>
#include "Goblin/Geometry/Coordinate.hpp"

std::size_t hash_value(const Coordinate& coord) {
	std::size_t seed = 0;
	boost::hash_combine(seed, coord.x);
	boost::hash_combine(seed, coord.y);
	return seed;
}

void Coordinate::save(OutputArchive& ar, const unsigned int version) const {
	ar & x;
	ar & y;
}

void Coordinate::load(InputArchive& ar, const unsigned int version)
{
	if (version == 0)
	{
		ar & x;
		ar & y;
	}
}

// Public Static Methods

Coordinate Coordinate::DirectionToCoordinate(Direction dir) noexcept
{
	switch (dir)
	{

	case NORTH:
		return { 0, -1 };
	case NORTHEAST:
		return { 1, -1 };
	case EAST:
		return { 1, 0 };
	case SOUTHEAST:
		return { 1, 1 };
	case SOUTH:
		return { 0, 1 };
	case SOUTHWEST:
		return { -1, 1 };
	case WEST:
		return { -1, 0 };
	case NORTHWEST:
		return { -1, -1 };
	case NODIRECTION:
		return { 0, 0 };
	default:
		// Without direction, unreachable code, avoid warnings.
		return { 0, 0 };
	}
}

// Getters

int Coordinate::getX() const noexcept
{
	return x;
}

int Coordinate::getY() const noexcept
{
	return y;
}

// Setters

void Coordinate::setX(const int _x) noexcept
{
	x = _x;
}

void Coordinate::setY(const int _y) noexcept
{
	y = _y;
}
