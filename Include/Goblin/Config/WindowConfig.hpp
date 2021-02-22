// Joan Andrés (@Andres6936) Github.

#ifndef GOBLINCAMP_WINDOWCONFIG_HPP
#define GOBLINCAMP_WINDOWCONFIG_HPP

#include <cstdint>

namespace Goblin
{

	/**
	 * Store the information of main console, this information include the width
	 * and height in cells of main console.
	 */
	class WindowConfig final
	{

	private:

		/**
		 * Width of main console in cells.
		 */
		inline static std::int32_t width{ 0 };

		/**
		 * Height of main console in cells.
		 */
		inline static std::int32_t height{ 0 };

	public:

		// Getters

		/**
		 * @return The width of main console in cells.
		 */
		static std::int32_t getWidth() noexcept;

		/**
		 * @return The height of main console in cells.
		 */
		static std::int32_t getHeight() noexcept;

		// Setters

		/**
		 * @param width Set the width of main console in cells.
		 */
		static void setWidth(std::int32_t width) noexcept;

		/**
		 * @param height Set the height of main console in cells.
		 */
		static void setHeight(std::int32_t height) noexcept;

	};

}

#endif //GOBLINCAMP_WINDOWCONFIG_HPP
