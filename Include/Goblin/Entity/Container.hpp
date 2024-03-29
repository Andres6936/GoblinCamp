/* Copyright 2010-2011 Ilkka Halila
This file is part of Goblin Camp.

Goblin Camp is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Goblin Camp is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License 
along with Goblin Camp. If not, see <http://www.gnu.org/licenses/>.*/
#pragma once

#include <set>
#include <memory>
#include <vector>

#include "Goblin/Entity/Item.hpp"
#include "Goblin/Config/Serialization.hpp"

class ContainerListener
{
public:
	virtual void ItemAdded(std::shared_ptr<Item>) = 0;

	virtual void ItemRemoved(std::shared_ptr<Item>) = 0;
};

class Container : public Item
{
	GC_SERIALIZABLE_CLASS

	std::set<std::shared_ptr<Item> > items;
	int capacity;
	int reservedSpace;

	std::vector<ContainerListener*> listeners;
	std::vector<int> listenersAsUids;

	int water, filth; //Special cases for real liquids
public:
	Container(Coordinate = Coordinate(0, 0), ItemType type = 0, int cap = 1000, int faction = 0,
			std::vector<std::shared_ptr<Item> > = std::vector<std::shared_ptr<Item> >(),
			std::vector<ContainerListener*> = std::vector<ContainerListener*>());

	~Container() override;

	virtual bool AddItem(std::shared_ptr<Item>);

	virtual void RemoveItem(std::shared_ptr<Item>);

	void ReserveSpace(bool, int bulk = 1);

	std::shared_ptr<Item> GetItem(std::shared_ptr<Item>);

	std::set<std::shared_ptr<Item> >* GetItems();

	std::shared_ptr<Item> GetFirstItem();

	bool empty();

	int size();

	int Capacity() const;

	bool Full() const;

	std::set<std::shared_ptr<Item> >::iterator begin();

	std::set<std::shared_ptr<Item> >::iterator end();

	void AddListener(ContainerListener* listener);

	void RemoveListener(ContainerListener* listener);

	void GetTooltip(int x, int y, Tooltip* tooltip);

	void TranslateContainerListeners();

	void AddWater(int);

	void RemoveWater(int);

	int ContainsWater() const;

	void AddFilth(int);

	void RemoveFilth(int);

	int ContainsFilth() const;
	void Draw(Coordinate, TCODConsole*);
	int GetReservedSpace() const;
	void Position(const Coordinate&) override;
	virtual Coordinate Position();
	virtual void SetFaction(int);
};

BOOST_CLASS_VERSION(::Container, 0)
