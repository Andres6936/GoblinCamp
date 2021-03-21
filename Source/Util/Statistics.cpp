// Joan Andrés (@Andres6936) Github.

#include "Goblin/Util/Statistics.hpp"

using namespace Goblin;

void Statistics::RaiseOrcs(const std::uint32_t amount) noexcept
{
	orcs += amount;
}

void Statistics::RaiseGoblins(const std::uint32_t amount) noexcept
{
	goblins += amount;
}

std::uint32_t Statistics::GetOrcs() const noexcept
{
	return orcs;
}

std::uint32_t Statistics::GetGoblins() const noexcept
{
	return goblins;
}

std::uint32_t Statistics::GetPopulation() const noexcept
{
	return orcs + goblins;
}

std::uint32_t Statistics::GetProgressionLevel() const noexcept
{
	return 0;
}
