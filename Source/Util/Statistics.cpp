// Joan Andrés (@Andres6936) Github.

#include <type_traits>

#include "Goblin/Util/Statistics.hpp"

using namespace Goblin;

// Private Zone for method not registered in the class

/**
 * Assert in compilation time that the value type of container is Integer.
 *
 * @tparam Container The parameter must be of meet the requirements
 * of Container, it is: A Container is an object used to store other
 * objects and taking care of the management of the memory used by
 * the objects it contains.
 *
 * @param container The container that store keys and values, generally a
 * unordered associative containers, its is: a containers that implement
 * unsorted (hashed) data structures that can be quickly searched (O(1)
 * amortized, O(n) worst-case complexity)..
 */
template<typename Container>
void AssertThatValueTypeOfContainerIsInteger(const Container& container)
{
	// Assert that the type of container value is of type Integral.
	static_assert(std::is_integral<typename Container::mapped_type>::value,
			"Integral required for the value type of container.");
}

/**
 * Wrapper around of a method that added or updated the value of a key an
 * container (Aka. unordered associative containers) in 1 (one) unit.
 *
 * Requirements:
 *
 * - The value type of container must be a Integer, it
 * is: (char, int, long).
 *
 * @tparam Container The parameter must be of meet the requirements
 * of Container, it is: A Container is an object used to store other
 * objects and taking care of the management of the memory used by
 * the objects it contains.
 *
 * @param elementType The key in the container, if the key not exist in the
 * container, it is added to container and set the value to 1 (one).
 *
 * @param container The container that store the keys, generally a
 * unordered associative containers, its is: a containers that implement
 * unsorted (hashed) data structures that can be quickly searched (O(1)
 * amortized, O(n) worst-case complexity)..
 */
template<typename Container>
void AddOrUpdateValueOfKeyInOne(const std::string& elementType, Container& container)
{
	// Assert in compilation time that the requirement is meet.
	AssertThatValueTypeOfContainerIsInteger(container);

	// If the type of element not exist in the map, insert it and set to 0 (zero).
	if (container.find(elementType) == container.end())
	{
		// Create the key and the set to 1 (because a item has been built).
		container[elementType] = 1;
	}
		// The type of element already has been defined and have minimum a item built.
	else
	{
		// Raise the amount of item built a 1 (one).
		container[elementType] = container[elementType] + 1;
	}
}

/**
 * Wrapper around of method that sum the value of all the keys of a
 * container and return the sum.
 *
 * Requirements:
 *
 * - The value type of container must be a Integer, it
 * is: (char, int, long).
 *
 * @tparam Container The parameter must be of meet the requirements
 * of Container, it is: A Container is an object used to store other
 * objects and taking care of the management of the memory used by
 * the objects it contains.
 *
 * @param container The container that store the values, generally a
 * unordered associative containers, its is: a containers that implement
 * unsorted (hashed) data structures that can be quickly searched (O(1)
 * amortized, O(n) worst-case complexity)..
 *
 * @return The sum total of all the values in the container.
 */
template<typename Container>
std::uint32_t SumAndGetValueOfAllKeys(const Container& container)
{
	// Assert in compilation time that the requirement is meet.
	AssertThatValueTypeOfContainerIsInteger(container);

	// Store the result of operation here
	std::uint32_t amountElements{ 0 };

	for (const auto&[elementType, amount] : container)
	{
		// Sum the amount of all the elements
		amountElements += amount;
	}

	return amountElements;
}

// End Private Zone, Begin Public Zone of methods registered in the class

void Statistics::RegisterNewItemBurned() noexcept
{
	itemsBurned += 1;
}

void Statistics::RaiseOrcs(const std::uint32_t amount) noexcept
{
	orcs += amount;
}

void Statistics::RaiseGoblins(const std::uint32_t amount) noexcept
{
	goblins += amount;
}

void Statistics::RegisterNewItemBuilt(const std::string& itemType) noexcept
{
	AddOrUpdateValueOfKeyInOne(itemType, itemsBuilt);
}

void Statistics::RegisterNewConstructionBuilt(const std::string& constructionType) noexcept
{
	AddOrUpdateValueOfKeyInOne(constructionType, constructionsBuilt);
}

// Getters

std::uint32_t Statistics::GetOrcs() const noexcept
{
	return orcs;
}

std::uint32_t Statistics::GetScore() const noexcept
{
	return GetAmountItemsBuilt() + (GetAmountConstructionsBuilt() * 10);
}

std::uint32_t Statistics::GetGoblins() const noexcept
{
	return goblins;
}

std::uint32_t Statistics::GetPopulation() const noexcept
{
	return orcs + goblins;
}

std::uint32_t Statistics::GetAmountItemsBuilt() const noexcept
{
	return SumAndGetValueOfAllKeys(itemsBuilt);
}

std::uint32_t Statistics::GetAmountItemsBurned() const noexcept
{
	return itemsBurned;
}

std::uint32_t Statistics::GetProgressionLevel() const noexcept
{
	const std::uint32_t population = GetPopulation();
	const std::uint32_t amountItemsBuilt = GetAmountItemsBuilt();

	// Store the result in this variable.
	std::uint32_t progressionLevel{ 0 };

	// Imports: There was previously an issue with new tier calculation - because
	// the check for each tier had a population range, if you exceeded that population
	// range before you achieved the other requirements you would not be able to reach that tier.
	// The solution is to check eligibility from highest tier downwards and avoid population ranges.

	if (workshops > 10 && amountItemsBuilt > 10000 && population >= 200)
	{
		progressionLevel = 6;
	}
	else if (workshops > 10 && amountItemsBuilt > 5000 && population >= 100)
	{
		progressionLevel = 5;
	}
	else if (workshops > 10 && amountItemsBuilt > 3000 && population >= 70)
	{
		progressionLevel = 4;
	}
	else if (workshops > 10 && amountItemsBuilt > 1000 && population >= 50)
	{
		progressionLevel = 3;
	}
	else if (workshops > 5 && amountItemsBuilt > 400 && population >= 30)
	{
		progressionLevel = 2;
	}
	else if (workshops > 1 && amountItemsBuilt > 20 && population >= 20)
	{
		progressionLevel = 1;
	}
	else
	{
		progressionLevel = 0;
	}

	return progressionLevel;
}

std::uint32_t Statistics::GetAmountConstructionsBuilt() const noexcept
{
	return SumAndGetValueOfAllKeys(constructionsBuilt);
}
