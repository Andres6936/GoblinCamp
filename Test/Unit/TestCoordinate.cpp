// Joan Andrés (@Andres6936) Github.

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include "Goblin/Geometry/Coordinate.hpp"

TEST_CASE ("Initialization for default to (x: 0, y: 0)")
{
	// Initialization for default
	Coordinate coordinateDefault{};

	// Initialization with parameters
	Coordinate coordinateParams{ 0, 0 };

			CHECK(coordinateDefault == coordinateParams);
}

TEST_CASE ("Direction to Coordinate")
{
			CHECK(Coordinate::DirectionToCoordinate(Direction::NORTH) == Coordinate{ 0, -1 });
			CHECK(Coordinate::DirectionToCoordinate(Direction::NORTHEAST) == Coordinate{ 1, -1 });
			CHECK(Coordinate::DirectionToCoordinate(Direction::NORTHWEST) == Coordinate{ -1, -1 });
			CHECK(Coordinate::DirectionToCoordinate(Direction::SOUTH) == Coordinate{ 0, 1 });
			CHECK(Coordinate::DirectionToCoordinate(Direction::SOUTHEAST) == Coordinate{ 1, 1 });
			CHECK(Coordinate::DirectionToCoordinate(Direction::SOUTHWEST) == Coordinate{ -1, 1 });
			CHECK(Coordinate::DirectionToCoordinate(Direction::EAST) == Coordinate{ 1, 0 });
			CHECK(Coordinate::DirectionToCoordinate(Direction::WEST) == Coordinate{ -1, 0 });
			CHECK(Coordinate::DirectionToCoordinate(Direction::NODIRECTION) == Coordinate{ 0, 0 });
}