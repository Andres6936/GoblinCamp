// Joan Andrés (@Andres6936) Github.

#ifndef GOBLIN_TASK_HPP
#define GOBLIN_TASK_HPP

#include <memory>

#include "Goblin/Task/Enum/Action.hpp"
#include <Goblin/Geometry/Coordinate.hpp>
#include "Goblin/Config/Serialization.hpp"

class Entity;

class Task
{
	GC_SERIALIZABLE_CLASS

private:

	typedef int ItemCategory;

public:
	Task(Action = NOACTION, Coordinate = Coordinate(-1, -1),
			std::weak_ptr<Entity> = std::weak_ptr<Entity>(),
			ItemCategory = 0, int flags = 0);

	Coordinate target;
	std::weak_ptr<Entity> entity;
	Action action;
	ItemCategory item;
	int flags;
};

BOOST_CLASS_VERSION(Task, 0)

#endif //GOBLIN_TASK_HPP
