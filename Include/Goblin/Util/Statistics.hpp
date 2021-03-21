// Joan Andrés (@Andres6936) Github.

#ifndef GOBLIN_STATISTICS_HPP
#define GOBLIN_STATISTICS_HPP

#include <cstdint>

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
		 * The amount item built in the current game.
		 */
		std::uint32_t itemsBuilt{ 0 };


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
		 * @return Calculate and return the current progression level of game.
		 */
		std::uint32_t GetProgressionLevel() const noexcept;

	};

}

#endif //GOBLIN_STATISTICS_HPP
