// Joan Andrés (@Andres6936) Github.

#ifndef GOBLINCAMP_WINDOWCONFIG_HPP
#define GOBLINCAMP_WINDOWCONFIG_HPP

#include <cstdint>

namespace Goblin
{

	class WindowConfig final
	{

	private:

		inline static std::int32_t width{ 0 };

		inline static std::int32_t height{ 0 };

	public:

		// Getters

		static std::int32_t getWidth() noexcept;

		static std::int32_t getHeight() noexcept;

		// Setters

		static void setWidth(std::int32_t width) noexcept;

		static void setHeight(std::int32_t height) noexcept;

	};

}

#endif //GOBLINCAMP_WINDOWCONFIG_HPP
