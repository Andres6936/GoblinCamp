// Joan Andrés (@Andres6936) Github.

#include "Goblin/Util/Statistics.hpp"

using namespace Goblin;

void Statistics::CalculateProgressionLevel() noexcept
{
	const std::uint32_t population = GetPopulation();

	// Imports: There was previously an issue with new tier calculation - because
	// the check for each tier had a population range, if you exceeded that population
	// range before you achieved the other requirements you would not be able to reach that tier.
	// The solution is to check eligibility from highest tier downwards and avoid population ranges.

	if (workshops > 10 && itemsBuilt > 10000 && population >= 200)
	{
		progressionLevel = 6;
	}
	else if (workshops > 10 && itemsBuilt > 5000 && population >= 100)
	{
		progressionLevel = 5;
	}
	else if (workshops > 10 && itemsBuilt > 3000 && population >= 70)
	{
		progressionLevel = 4;
	}
	else if (workshops > 10 && itemsBuilt > 1000 && population >= 50)
	{
		progressionLevel = 3;
	}
	else if (workshops > 5 && itemsBuilt > 400 && population >= 30)
	{
		progressionLevel = 2;
	}
	else if (workshops > 1 && itemsBuilt > 20 && population >= 20)
	{
		progressionLevel = 1;
	}
	else
	{
		progressionLevel = 0;
	}
}

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
	return progressionLevel;
}
