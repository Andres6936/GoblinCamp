// Joan Andrés (@Andres6936) Github.

#ifndef GOBLIN_STATISTICS_HPP
#define GOBLIN_STATISTICS_HPP

#include <string>
#include <cstdint>
#include <unordered_map>

namespace Goblin
{

	class Statistics
	{

	private:

		/**
		 * The amount of orcs in the current game.
		 */
		std::uint32_t orcs{ 0 };

		/**
		 * The amount of goblin in the current game.
		 */
		std::uint32_t goblins{ 0 };

		/**
		 * The amount of farmplots in the current game.
		 */
		std::uint32_t farmplots{ 0 };

		/**
		 * The amount workshops in the current game.
		 */
		std::uint32_t workshops{ 0 };

		/**
		 * Store in the key: the type of item built and in the value: the
		 * amount of items built of this type in the current game.
		 */
		std::unordered_map<std::string, std::uint32_t> itemsBuilt{};


	public:

		/**
		 * Raise the amount of orcs in the game.
		 *
		 * @param amount The amount of orcs to raise.
		 */
		void RaiseOrcs(const std::uint32_t amount) noexcept;

		/**
		 * Raise the amount of goblins in the game.
		 *
		 * @param amount The amount of goblins to raise.
		 */
		void RaiseGoblins(const std::uint32_t amount) noexcept;

		/**
		 * Raise in 1 the amount of items built of this type.
		 *
		 * @param itemType The type of item built.
		 */
		void RegisterNewItemBuilt(const std::string& itemType) noexcept;

		// Getters

		/**
		 * @return The amount of orcs in the current game.
		 */
		std::uint32_t GetOrcs() const noexcept;

		/**
		 * @return The amoun of goblins in the current game.
		 */
		std::uint32_t GetGoblins() const noexcept;

		/**
		 * @return The amount of population (orcs + goblins) in the current game.
		 */
		std::uint32_t GetPopulation() const noexcept;

		/**
		 * @return The amount of items built in the current game.
		 */
		std::uint32_t GetAmountItemsBuilt() const noexcept;

		/**
		 * @return Calculate and return the current progression level of game.
		 */
		std::uint32_t GetProgressionLevel() const noexcept;

	};

}

#endif //GOBLIN_STATISTICS_HPP
