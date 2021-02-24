// Joan Andrés (@Andres6936) Github.

#include "Goblin/Task/Task.hpp"
#include "Goblin/Entity/Entity.hpp"
#include <boost/serialization/weak_ptr.hpp>

Task::Task(Action act, Coordinate tar, std::weak_ptr<Entity> ent, ItemCategory itt, int fla) :
		target(tar),
		entity(ent),
		action(act),
		item(itt),
		flags(fla)
{
}

void Task::save(OutputArchive& ar, const unsigned int version) const
{
	ar & target;
	ar & entity;
	ar & action;
	ar & item;
	ar & flags;
}

void Task::load(InputArchive& ar, const unsigned int version)
{
	ar & target;
	ar & entity;
	ar & action;
	ar & item;
	ar & flags;
}

