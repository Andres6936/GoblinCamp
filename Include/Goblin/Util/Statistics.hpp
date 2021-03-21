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
		 * The amount of items burned (deleted or removed) of current game.
		 */
		std::uint32_t itemsBurned{ 0 };

		/**
		 * Store in the key: the type of item built and in the value: the
		 * amount of items built of this type in the current game.
		 */
		std::unordered_map<std::string, std::uint32_t> itemsBuilt{};

		/**
		 * Store in the key: the type of construction and in the value: the
		 * amount of constructions of this type built in the current game.
		 */
		std::unordered_map<std::string, std::uint32_t> constructionsBuilt{};

	public:

		/**
		 * Raise in 1 the amount of items burned in the current game.
		 */
		void RegisterNewItemBurned() noexcept;

		/**
		 * Raise the amount of orcs in the current game.
		 *
		 * @param amount The amount of orcs to raise.
		 */
		void RaiseOrcs(const std::uint32_t amount) noexcept;

		/**
		 * Raise the amount of goblins in the current game.
		 *
		 * @param amount The amount of goblins to raise.
		 */
		void RaiseGoblins(const std::uint32_t amount) noexcept;

		/**
		 * Raise in 1 the amount of items built of this type in the current game.
		 *
		 * @param itemType The type of item built.
		 */
		void RegisterNewItemBuilt(const std::string& itemType) noexcept;

		/**
		 * Raise in 1 the amount of constructions of this type in the current game.
		 *
		 * @param itemType The type of construction built.
		 */
		void RegisterNewConstructionBuilt(const std::string& constructionType) noexcept;

		// Getters

		/**
		 * @return The amount of orcs in the current game.
		 */
		std::uint32_t GetOrcs() const noexcept;

		/**
		 * The score is calculated in base a all the item and constructions
		 * built in the current game.
		 *
		 * - Each item built in the current game sum 1 (one) point.
		 * - Each construction built in the current game sum 10 (ten) points.
		 *
		 * @return The score of user in the current game.
		 */
		std::uint32_t GetScore() const noexcept;

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
		 * @return The amount of items burned in the current game.
		 */
		std::uint32_t GetAmountItemBurned() const noexcept;

		/**
		 * @return Calculate and return the current progression level of game.
		 */
		std::uint32_t GetProgressionLevel() const noexcept;

		/**
		 * @return The amount of construction built in the current game.
		 */
		std::uint32_t GetAmountConstructionsBuilt() const noexcept;

	};

}

#endif //GOBLIN_STATISTICS_HPP
