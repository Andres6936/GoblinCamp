// Joan Andrés (@Andres6936) Github.

#include "Goblin/Config/WindowConfig.hpp"

using namespace Goblin;

// Getters

std::int32_t WindowConfig::getWidth() noexcept
{
	return width;
}

std::int32_t WindowConfig::getHeight() noexcept
{
	return height;
}

// Setters

void WindowConfig::setWidth(std::int32_t _width) noexcept
{
	width = _width;
}

void WindowConfig::setHeight(std::int32_t _height) noexcept
{
	height = _height;
}
