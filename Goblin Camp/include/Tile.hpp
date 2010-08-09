/* Copyright 2010 Ilkka Halila
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
#include <vector>

#include <boost/serialization/split_member.hpp>
#include <boost/shared_ptr.hpp>

#include "Water.hpp"
#include "Filth.hpp"
#include "Blood.hpp"

enum TileType {
	TILENONE,
	TILEGRASS,
	TILEDITCH,
	TILERIVERBED,
	TILEBOG
};

class Tile {
	friend class boost::serialization::access;
	friend class Map;
private:
	template<class Archive>
	void save(Archive & ar, const unsigned int version) const;
	template<class Archive>
	void load(Archive & ar, const unsigned int version);
	BOOST_SERIALIZATION_SPLIT_MEMBER()

	TileType _type;
	bool vis; //Does light pass through this tile? Tile type, but also constructions/objects affect this
	bool walkable;
	bool buildable;
	int _moveCost;
	int construction;
	bool low, blocksWater;
	boost::shared_ptr<WaterNode> water;
	int graphic;
	TCODColor foreColor;
	TCODColor backColor;
	int natureObject;
	std::set<int> npcList; //Set of NPC uid's
	std::set<int> itemList; //Set of Item uid's
	boost::shared_ptr<FilthNode> filth;
	boost::shared_ptr<BloodNode> blood;
	bool marked;

public:
	Tile(TileType = TILEGRASS, int = 1);
	TileType type();
	void type(TileType);
	bool BlocksLight() const;
	void BlocksLight(bool);
	bool Walkable() const;
	void Walkable(bool);
	bool Buildable() const;
	void Buildable(bool);
	int MoveCost() const;
	int MoveCost(void*) const;
	void SetMoveCost(int);
	void MoveFrom(int);
	void MoveTo(int);
	void SetConstruction(int);
	int GetConstruction() const;
	boost::weak_ptr<WaterNode> GetWater() const;
	void SetWater(boost::shared_ptr<WaterNode>);
	bool Low() const;
	void Low(bool);
	bool BlocksWater() const;
	void BlocksWater(bool);
	int Graphic() const;
	TCODColor ForeColor() const;
	TCODColor BackColor() const;
	void NatureObject(int);
	int NatureObject() const;
	boost::weak_ptr<FilthNode> GetFilth() const;
	void SetFilth(boost::shared_ptr<FilthNode>);
	boost::weak_ptr<BloodNode> GetBlood() const;
	void SetBlood(boost::shared_ptr<BloodNode>);
	void Mark();
	void Unmark();
};
