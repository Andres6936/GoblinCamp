// Joan Andrés (@Andres6936) Github.

#ifndef GOBLIN_STATISTICSDISPLAY_HPP
#define GOBLIN_STATISTICSDISPLAY_HPP

namespace Goblin
{

	class Statistics;

	/**
	 * Responsibility of show the statistics to user of current game.
	 */
	class StatisticsDisplay
	{

	public:

		/**
		 * Show the current statistics of current game.
		 *
		 * @param statistics The data structure that store the statistics.
		 */
		static void Show(const Statistics& statistics);

	};

}

#endif //GOBLIN_STATISTICSDISPLAY_HPP
