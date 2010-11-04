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
#include "stdafx.hpp"

#include <map>

#include <boost/multi_array.hpp>
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

#ifdef DEBUG
#include <iostream>
#endif

#include "Game.hpp"
#include "Tile.hpp"
#include "Coordinate.hpp"
#include "JobManager.hpp"
#include "Logger.hpp"
#include "Map.hpp"
#include "Announce.hpp"
#include "GCamp.hpp"
#include "StockManager.hpp"
#include "UI.hpp"
#include "StatusEffect.hpp"
#include "Stockpile.hpp"
#include "Farmplot.hpp"
#include "Door.hpp"
#include "data/Config.hpp"
#include "UI/YesNoDialog.hpp"
#include "scripting/Event.hpp"
#include "SpawningPool.hpp"

int Game::ItemTypeCount = 0;
int Game::ItemCatCount = 0;

Game* Game::instance = 0;

Game::Game() :
screenWidth(0),
	screenHeight(0),
	season(EarlySpring),
	time(0),
	orcCount(0),
	goblinCount(0),
	peacefulFaunaCount(0),
	paused(false),
	toMainMenu(false),
	running(false),
	safeMonths(9),
	devMode(false),
	events(boost::shared_ptr<Events>()),
	upleft(Coordinate(0,0))
{
	for(int i = 0; i < 12; i++) {
		marks[i] = Coordinate(-1, -1);
	}
}

Game* Game::Inst() {
	if (!instance) instance = new Game();
	return instance;
}

//Checks whether all the tiles under the rectangle (target is the up-left corner) are buildable
bool Game::CheckPlacement(Coordinate target, Coordinate size) {
	for (int x = target.X(); x < target.X() + size.X(); ++x) {
		for (int y = target.Y(); y < target.Y() + size.Y(); ++y) {
			if (x < 0 || y < 0 || x >= Map::Inst()->Width() || y >= Map::Inst()->Height() || !Map::Inst()->Buildable(x,y)) return false;
		}
	}
	return true;
}

int Game::PlaceConstruction(Coordinate target, ConstructionType construct) {
	//Check if the required materials exist before creating the build job
	std::list<boost::weak_ptr<Item> > componentList;
	for (std::list<ItemCategory>::iterator mati = Construction::Presets[construct].materials.begin();
		mati != Construction::Presets[construct].materials.end(); ++mati) {
			boost::weak_ptr<Item> material = Game::Inst()->FindItemByCategoryFromStockpiles(*mati, target);
			if (boost::shared_ptr<Item> item = material.lock()) {
				item->Reserve(true);
				componentList.push_back(item);
			} else {
				for (std::list<boost::weak_ptr<Item> >::iterator compi = componentList.begin();
					compi != componentList.end(); ++compi) {
						compi->lock()->Reserve(false);
				}
				componentList.clear();
				Announce::Inst()->AddMsg((boost::format("Cancelled %s: insufficient [%s] in stockpiles") % Construction::Presets[construct].name % Item::ItemCategoryToString(*mati)).str(), TCODColor::red);
				return -1;
			}
	}

	if (Construction::AllowedAmount[construct] >= 0) {
		if (Construction::AllowedAmount[construct] == 0) {
			Announce::Inst()->AddMsg("Cannot build another "+Construction::Presets[construct].name+"!", TCODColor::red);
			return -1;
		}
		--Construction::AllowedAmount[construct];
	}

	for (std::list<boost::weak_ptr<Item> >::iterator compi = componentList.begin();
		compi != componentList.end(); ++compi) {
			compi->lock()->Reserve(false);
	}
	componentList.clear();

	boost::shared_ptr<Construction> newCons;
	if (Construction::Presets[construct].tags[DOOR]) {
		newCons = boost::shared_ptr<Construction>(new Door(construct, target));
	} else if (Construction::Presets[construct].tags[SPAWNINGPOOL]) {
		newCons = boost::shared_ptr<Construction>(new SpawningPool(construct, target));
	} else {
		newCons = boost::shared_ptr<Construction>(new Construction(construct, target));
	}
	if (Construction::Presets[construct].dynamic) {
		Game::Inst()->dynamicConstructionList.insert(std::pair<int,boost::shared_ptr<Construction> >(newCons->Uid(), newCons));
	} else {
		Game::Inst()->staticConstructionList.insert(std::pair<int,boost::shared_ptr<Construction> >(newCons->Uid(), newCons));
	}
	Coordinate blueprint = Construction::Blueprint(construct);
	for (int x = target.X(); x < target.X() + blueprint.X(); ++x) {
		for (int y = target.Y(); y < target.Y() + blueprint.Y(); ++y) {
			Map::Inst()->Buildable(x,y,false);
			Map::Inst()->SetConstruction(x,y,newCons->Uid());
		}
	}

	boost::shared_ptr<Job> buildJob(new Job("Build " + Construction::Presets[construct].name, MED, 0, false));

	for (std::list<ItemCategory>::iterator materialIter = newCons->MaterialList()->begin(); materialIter != newCons->MaterialList()->end(); ++materialIter) {
		boost::shared_ptr<Job> pickupJob(new Job("Pickup " + Item::ItemCategoryToString(*materialIter) + " for " + Construction::Presets[construct].name, MED, 0, true));
		pickupJob->Parent(buildJob);
		buildJob->PreReqs()->push_back(pickupJob);

		pickupJob->tasks.push_back(Task(FIND, target, boost::weak_ptr<Entity>(), *materialIter));
		pickupJob->tasks.push_back(Task(MOVE));
		pickupJob->tasks.push_back(Task(TAKE));
		pickupJob->tasks.push_back(Task(MOVE, newCons->Position(), newCons));
		pickupJob->tasks.push_back(Task(PUTIN, newCons->Storage().lock()->Position(), newCons->Storage()));
		JobManager::Inst()->AddJob(pickupJob);
	}

	buildJob->tasks.push_back(Task(MOVEADJACENT, newCons->Position(), newCons));
	buildJob->tasks.push_back(Task(BUILD, newCons->Position(), newCons));
	buildJob->ConnectToEntity(newCons);

	JobManager::Inst()->AddJob(buildJob);

	return newCons->Uid();
}

int Game::PlaceStockpile(Coordinate a, Coordinate b, ConstructionType stockpile, int symbol) {
	//Placing a stockpile isn't as straightforward as just building one in each tile
	//We want to create 1 stockpile, at a, and then expand it from a to b.
	//Using the stockpile expansion function ensures that it only expands into valid tiles
	boost::shared_ptr<Stockpile> newSp( (Construction::Presets[stockpile].tags[FARMPLOT]) ? new FarmPlot(stockpile, symbol, a) : new Stockpile(stockpile, symbol, a) );
	Map::Inst()->Buildable(a.X(), a.Y(), false);
	Map::Inst()->SetConstruction(a.X(), a.Y(), newSp->Uid());
	newSp->Expand(a,b);
	if (Construction::Presets[stockpile].dynamic) {
		Game::Inst()->dynamicConstructionList.insert(std::pair<int,boost::shared_ptr<Construction> >(newSp->Uid(),static_cast<boost::shared_ptr<Construction> >(newSp)));
	} else {
		Game::Inst()->staticConstructionList.insert(std::pair<int,boost::shared_ptr<Construction> >(newSp->Uid(),static_cast<boost::shared_ptr<Construction> >(newSp)));
	}

	Game::Inst()->RefreshStockpiles();

	//Spawning a BUILD job is not required because stockpiles are created "built"
	return newSp->Uid();
}

//Returns Coordinate(<0,<0) if not found
Coordinate Game::FindClosestAdjacent(Coordinate pos, boost::weak_ptr<Entity> ent) {
	Coordinate closest(-9999, -9999);
	if (ent.lock()) {
		if (boost::dynamic_pointer_cast<Construction>(ent.lock())) {
			boost::shared_ptr<Construction> construct(boost::static_pointer_cast<Construction>(ent.lock()));
			for (int ix = construct->X()-1; ix <= construct->X() + Construction::Blueprint(construct->Type()).X(); ++ix) {
				for (int iy = construct->Y()-1; iy <= construct->Y() + Construction::Blueprint(construct->Type()).Y(); ++iy) {
					if (ix == construct->X()-1 || ix == construct->X() + Construction::Blueprint(construct->Type()).X() ||
						iy == construct->Y()-1 || iy == construct->Y() + Construction::Blueprint(construct->Type()).Y()) {
							if (Map::Inst()->Walkable(ix,iy)) {
								if (Distance(pos.X(), pos.Y(), ix, iy) < Distance(pos.X(), pos.Y(), closest.X(), closest.Y()))
									closest = Coordinate(ix,iy);
							}
					}
				}
			}
		} else {
			for (int ix = ent.lock()->X()-1; ix <= ent.lock()->X()+1; ++ix) {
				for (int iy = ent.lock()->Y()-1; iy <= ent.lock()->Y()+1; ++iy) {
					if (ix == ent.lock()->X()-1 || ix == ent.lock()->X()+1 ||
						iy == ent.lock()->Y()-1 || iy == ent.lock()->Y()+1) {
							if (Map::Inst()->Walkable(ix,iy)) {
								if (Distance(pos.X(), pos.Y(), ix, iy) < Distance(pos.X(), pos.Y(), closest.X(), closest.Y()))
									closest = Coordinate(ix,iy);
							}
					}
				}
			}
		}
	}
	return closest;
}

//Returns true/false depending on if the given position is adjacent to the entity
//Takes into consideration if the entity is a construction, and thus may be larger than just one tile
bool Game::Adjacent(Coordinate pos, boost::weak_ptr<Entity> ent) {
	if (ent.lock()) {
		if (boost::dynamic_pointer_cast<Construction>(ent.lock())) {
			boost::shared_ptr<Construction> construct(boost::static_pointer_cast<Construction>(ent.lock()));
			for (int ix = construct->X()-1; ix <= construct->X() + Construction::Blueprint(construct->Type()).X(); ++ix) {
				for (int iy = construct->Y()-1; iy <= construct->Y() + Construction::Blueprint(construct->Type()).Y(); ++iy) {
					if (pos.X() == ix && pos.Y() == iy) { return true; }
				}
			}
			return false;
		} else {
			for (int ix = ent.lock()->X()-1; ix <= ent.lock()->X()+1; ++ix) {
				for (int iy = ent.lock()->Y()-1; iy <= ent.lock()->Y()+1; ++iy) {
					if (pos.X() == ix && pos.Y() == iy) { return true; }
				}
			}
			return false;
		}
	}
	return false;
}

int Game::CreateNPC(Coordinate target, NPCType type) {

	int tries = 0;
	int radius = 1;
	Coordinate originalTarget = target;
	while (!Map::Inst()->Walkable(target.X(), target.Y()) && tries < 20) {
		target.X(originalTarget.X() + TCODRandom::getInstance()->get(-radius, radius));
		target.Y(originalTarget.Y() + TCODRandom::getInstance()->get(-radius, radius));
		if (++tries % 3 == 0) ++radius;
	}

	boost::shared_ptr<NPC> npc(new NPC(target));
	npc->type = type;
	npc->InitializeAIFunctions();
	npc->expert = NPC::Presets[type].expert;
	npc->color(NPC::Presets[type].color);
	npc->graphic(NPC::Presets[type].graphic);

	if (NPC::Presets[type].generateName) {
		npc->name = TCODNamegen::generate(const_cast<char*>(NPC::Presets[type].name.c_str()));
	} else npc->name = NPC::Presets[type].name;

	npc->needsNutrition = NPC::Presets[type].needsNutrition;
	npc->needsSleep = NPC::Presets[type].needsSleep;
	npc->health = NPC::Presets[type].health;
	npc->maxHealth = NPC::Presets[type].health;
	for (int i = 0; i < STAT_COUNT; ++i) {
		npc->baseStats[i] = NPC::Presets[type].stats[i] + ((NPC::Presets[type].stats[i] * 0.1) * (rand() % 2) ? 1 : -1);
	}
	for (int i = 0; i < RES_COUNT; ++i) {
		npc->baseResistances[i] = NPC::Presets[type].resistances[i] + ((NPC::Presets[type].resistances[i] * 0.1) * (rand() % 2) ? 1 : -1);
	}

	npc->attacks = NPC::Presets[type].attacks;

	if (boost::iequals(NPC::NPCTypeToString(type), "orc")) ++orcCount;
	else if (boost::iequals(NPC::NPCTypeToString(type), "goblin")) ++goblinCount;
	else if (NPC::Presets[type].tags.find("localwildlife") != NPC::Presets[type].tags.end()) ++peacefulFaunaCount;

	if (NPC::Presets[type].tags.find("flying") != NPC::Presets[type].tags.end()) {
		npc->AddEffect(FLYING);
	}

	if (NPC::Presets[type].tags.find("coward") != NPC::Presets[type].tags.end()) {
		npc->coward = true;
	}

	if (NPC::Presets[type].tags.find("hashands") != NPC::Presets[type].tags.end()) {
		npc->hasHands = true;
	}

	npcList.insert(std::pair<int,boost::shared_ptr<NPC> >(npc->Uid(),npc));

	return npc->Uid();
}

int Game::OrcCount() const { return orcCount; }
void Game::OrcCount(int add) { orcCount += add; }
int Game::GoblinCount() const { return goblinCount; }
void Game::GoblinCount(int add) { goblinCount += add; }


//Moves the entity to a valid walkable tile
//TODO: make it find the closest walkable tile instead of going right, that will lead to weirdness and problems
void Game::BumpEntity(int uid) {
	boost::weak_ptr<Entity> entity;

	std::map<int,boost::shared_ptr<NPC> >::iterator npc = npcList.find(uid);
	if (npc != npcList.end()) {
		entity = npc->second;
	} else {
		std::map<int,boost::shared_ptr<Item> >::iterator item = itemList.find(uid);
		if (item != itemList.end()) {
			entity = item->second;
		}
	}

	if (entity.lock()) {
		int newx = entity.lock()->X();
		int newy = entity.lock()->Y();
		while (!Map::Inst()->Walkable(newx,newy)) {
			++newx;
		}
		entity.lock()->Position(Coordinate(newx,newy));
	} 
#ifdef DEBUG
	else { std::cout<<"\nTried to bump nonexistant entity."; }
#endif
}

void Game::DoNothing() {}

void Game::Exit(bool confirm) {
	boost::function<void()> exitFunc = boost::bind(&Game::Running, Game::Inst(), false);

	if (confirm) {
		YesNoDialog::ShowYesNoDialog("Really exit?", exitFunc, NULL);
	} else {
		exitFunc();
	}
}

int Game::ScreenWidth() const {	return screenWidth; }
int Game::ScreenHeight() const { return screenHeight; }

void Game::Init() {
	int width  = Config::GetCVar<int>("resolutionX");
	int height = Config::GetCVar<int>("resolutionY");
	bool fullscreen = Config::GetCVar<bool>("fullscreen");
	TCOD_renderer_t renderer = static_cast<TCOD_renderer_t>(Config::GetCVar<int>("renderer"));

	if (width <= 0 || height <= 0) {
		if (fullscreen) {
			TCODSystem::getCurrentResolution(&width, &height);
		} else {
			width  = 640;
			height = 480;
		}
	}

	TCODSystem::getCharSize(&charWidth, &charHeight);
	screenWidth  = width / charWidth;
	screenHeight = height / charHeight;

	srand((unsigned int)std::time(0));

	//Enabling TCOD_RENDERER_GLSL can cause GCamp to crash on exit, apparently it's because of an ATI driver issue.
	TCODConsole::initRoot(screenWidth, screenHeight, "Goblin Camp", fullscreen, renderer);
	TCODMouse::showCursor(true);
	TCODConsole::setKeyboardRepeat(500, 10);

	{
		// "Loading..."
		TCODConsole::root->setDefaultForeground(TCODColor::white);
		TCODConsole::root->setDefaultBackground(TCODColor::black);
		TCODConsole::root->setAlignment(TCOD_CENTER);
		TCODConsole::root->clear();

		TCODConsole::root->print(screenWidth / 2, screenHeight / 2, "Loading...");

		TCODConsole::root->flush();
	}

	TCODConsole::root->setAlignment(TCOD_LEFT);

	events = boost::shared_ptr<Events>(new Events(Map::Inst()));

	buffer = new TCODConsole(screenWidth, screenHeight);
	season = LateWinter;
	upleft = Coordinate(180,180);
}

void Game::RemoveConstruction(boost::weak_ptr<Construction> cons) {
	if (boost::shared_ptr<Construction> construct = cons.lock()) {
		Coordinate blueprint = Construction::Blueprint(construct->Type());
		for (int x = construct->X(); x < construct->X() + blueprint.X(); ++x) {
			for (int y = construct->Y(); y < construct->Y() + blueprint.Y(); ++y) {
				Map::Inst()->Buildable(x,y,true);
				Map::Inst()->SetConstruction(x,y,-1);
			}
		}
		if (Construction::Presets[construct->type].dynamic) {
			Game::Inst()->dynamicConstructionList.erase(construct->Uid());
		} else {
			Game::Inst()->staticConstructionList.erase(construct->Uid());
		}
	}
}

void Game::DismantleConstruction(Coordinate a, Coordinate b) {
	for (int x = a.X(); x <= b.X(); ++x) {
		for (int y = a.Y(); y <= b.Y(); ++y) {
			int construction = Map::Inst()->GetConstruction(x,y);
			if (construction >= 0) {
				if (instance->GetConstruction(construction).lock()) {
					instance->GetConstruction(construction).lock()->Dismantle();
				} else {
					Map::Inst()->SetConstruction(x,y,-1);
				}
			}
		}
	}
}

boost::weak_ptr<Construction> Game::GetConstruction(int uid) {
	if (staticConstructionList.find(uid) != staticConstructionList.end()) 
		return staticConstructionList[uid];
	else if (dynamicConstructionList.find(uid) != dynamicConstructionList.end())
		return dynamicConstructionList[uid];
	return boost::weak_ptr<Construction>();
}

int Game::CreateItem(Coordinate pos, ItemType type, bool store, int ownerFaction, 
	std::vector<boost::weak_ptr<Item> > comps, boost::shared_ptr<Container> container) {

		boost::shared_ptr<Item> newItem;
		if (Item::Presets[type].organic) {
			boost::shared_ptr<OrganicItem> orgItem(new OrganicItem(pos, type));
			newItem = boost::static_pointer_cast<Item>(orgItem);
			orgItem->Nutrition(Item::Presets[type].nutrition);
			orgItem->Growth(Item::Presets[type].growth);
		} else if (Item::Presets[type].container > 0) {
			newItem.reset(static_cast<Item*>(new Container(pos, type, Item::Presets[type].container, 0, comps)));
		} else {
			newItem.reset(new Item(pos, type, 0, comps));
		}

		if (!container) {
			freeItems.insert(newItem);
			Map::Inst()->ItemList(newItem->X(), newItem->Y())->insert(newItem->Uid());
		} else {
			container->AddItem(newItem);
		}
		itemList.insert(std::pair<int,boost::shared_ptr<Item> >(newItem->Uid(), newItem));
		if (store) StockpileItem(newItem);

		Script::Event::ItemCreated(newItem, pos.X(), pos.Y());

#ifdef DEBUG
		std::cout<<newItem->name<<"("<<newItem->Uid()<<") created\n";
#endif

		return newItem->Uid();
}

void Game::RemoveItem(boost::weak_ptr<Item> witem) {
	if (boost::shared_ptr<Item> item = witem.lock()) {
		Map::Inst()->ItemList(item->x, item->y)->erase(item->uid);
		if (freeItems.find(witem) != freeItems.end()) freeItems.erase(witem);
		if (boost::shared_ptr<Container> container = boost::static_pointer_cast<Container>(item->container.lock())) {
			if (container) {
				container->RemoveItem(witem);
			}
		}
		itemList.erase(item->uid);
	}
}

boost::weak_ptr<Item> Game::GetItem(int uid) {
	return itemList[uid];
}

void Game::ItemContained(boost::weak_ptr<Item> item, bool con) {
	if (!con) {
		freeItems.insert(item);
		Map::Inst()->ItemList(item.lock()->X(), item.lock()->Y())->insert(item.lock()->Uid());
	}
	else {
		freeItems.erase(item);
		Map::Inst()->ItemList(item.lock()->X(), item.lock()->Y())->erase(item.lock()->Uid());
	}
}

void Game::CreateWater(Coordinate pos) {
	CreateWater(pos, 10);
}

void Game::CreateWater(Coordinate pos, int amount, int time) {
	boost::weak_ptr<WaterNode> water(Map::Inst()->GetWater(pos.X(), pos.Y()));
	if (!water.lock()) {
		boost::shared_ptr<WaterNode> newWater(new WaterNode(pos.X(), pos.Y(), amount, time));
		waterList.push_back(boost::weak_ptr<WaterNode>(newWater));
		Map::Inst()->SetWater(pos.X(), pos.Y(), newWater);
	} else {water.lock()->Depth(water.lock()->Depth()+amount);}
}

int Game::DistanceNPCToCoordinate(int uid, Coordinate pos) {
	return Distance(npcList[uid]->X(), npcList[uid]->Y(), pos.X(), pos.Y());
}

// TODO this currently checks every stockpile.  We could maintain some data structure that allowed us to check the closest stockpile(s)
// first.
boost::weak_ptr<Item> Game::FindItemByCategoryFromStockpiles(ItemCategory category, Coordinate target, int flags, int value) {
	int nearestDistance = INT_MAX;
	boost::weak_ptr<Item> nearest = boost::weak_ptr<Item>();
	for (std::map<int, boost::shared_ptr<Construction> >::iterator consIter = staticConstructionList.begin(); consIter != staticConstructionList.end(); ++consIter) {
		if (consIter->second->stockpile && !consIter->second->farmplot) {
			boost::weak_ptr<Item> item(boost::static_pointer_cast<Stockpile>(consIter->second)->FindItemByCategory(category, flags, value));
			if (item.lock() && !item.lock()->Reserved()) {
				int distance = Distance(item.lock()->Position(), target);
				if(distance < nearestDistance) {
					nearestDistance = distance;
					nearest = item;
				}
			}
		}
	}
	return nearest;
}

// TODO this currently checks every stockpile.  We could maintain some data structure that allowed us to check the closest stockpile(s)
// first.
boost::weak_ptr<Item> Game::FindItemByTypeFromStockpiles(ItemType type, Coordinate target, int flags, int value) {
	int nearestDistance = INT_MAX;
	boost::weak_ptr<Item> nearest = boost::weak_ptr<Item>();
	for (std::map<int, boost::shared_ptr<Construction> >::iterator consIter = staticConstructionList.begin(); consIter != staticConstructionList.end(); ++consIter) {
		if (consIter->second->stockpile && !consIter->second->farmplot) {
			boost::weak_ptr<Item> item(boost::static_pointer_cast<Stockpile>(consIter->second)->FindItemByType(type, flags, value));
			if (item.lock() && !item.lock()->Reserved()) {
				int distance = Distance(item.lock()->Position(), target);
				if(distance < nearestDistance) {
					nearestDistance = distance;
					nearest = item;
				}
			}
		}
	}
	return nearest;
}

// Spawns items distributed randomly within the rectangle defined by corner1 & corner2
void Game::CreateItems(int quantity, ItemType type, Coordinate corner1, Coordinate corner2) {
	int areaWidth = std::max(abs(corner1.X()-corner2.X()),1);
	int areaLength = std::max(abs(corner1.Y()-corner2.Y()),1);

	for (int items = 0; items < quantity; ++items) {
		Coordinate location(rand() % areaWidth + std::min(corner1.X(),corner2.X()), rand() % areaLength + std::min(corner1.Y(),corner2.Y()));
		Game::Inst()->CreateItem(location, type, true);
	}
}

Coordinate Game::FindFilth(Coordinate pos) {
	if (filthList.size() == 0) return Coordinate(-1,-1);
	//Choose random filth
	std::priority_queue<std::pair<int, int> > potentialFilth;
	for (unsigned int i = 0; i < std::min((unsigned int)10, filthList.size()); ++i) {
		int filth = rand() % filthList.size();
		if (boost::next(filthList.begin(), filth)->lock()->Depth() > 0)
			potentialFilth.push(std::pair<int,int>(Distance(pos, boost::next(filthList.begin(), filth)->lock()->Position()), filth));
	}
	if (potentialFilth.size() > 0)
		return boost::next(filthList.begin(), potentialFilth.top().second)->lock()->Position();
	else
		return Coordinate(-1,-1);
}

//Findwater returns the coordinates to the closest Water* that has sufficient depth
Coordinate Game::FindWater(Coordinate pos) {
	Coordinate closest(-9999,-9999);
	for (std::list<boost::weak_ptr<WaterNode> >::iterator wati = waterList.begin(); wati != waterList.end(); ++wati) {
		if (wati->lock()->Depth() > DRINKABLE_WATER_DEPTH) {
			if (Distance(wati->lock()->Position(), pos) < Distance(closest, pos)) closest = wati->lock()->Position();
		}
	}
	if (closest.X() == -9999) return Coordinate(-1,-1);
	return closest;
}

void Game::Update() {
	++time;

	if (time == MONTH_LENGTH) {
		if (safeMonths > 0) --safeMonths;
		if (season < LateWinter) season = (Season)((int)season + 1);
		else season = EarlySpring;

		switch (season) {
		case EarlySpring:
			Announce::Inst()->AddMsg("Spring has begun");
		case Spring:
		case LateSpring:
			SpawnTillageJobs();
		case Summer:
		case LateSummer:
		case Fall:
		case LateFall:
		case Winter:
			DecayItems();
			break;

		case LateWinter:
			break;

		case EarlySummer:
			Announce::Inst()->AddMsg("Summer has begun");
			DecayItems();
			break;

		case EarlyFall:
			Announce::Inst()->AddMsg("Fall has begun");
			DecayItems();
			break;

		case EarlyWinter:
			Announce::Inst()->AddMsg("Winter has begun");
			DeTillFarmPlots();
			break;

		default: break;
		}
		time = 0;
	}

	//This actually only updates every 50th waternode. This is due to 2 things: updating one water tile actually also
	//updates all its neighbours. Also, by updating only every 50th one, the load on the cpu is less, but you need to
	//remember that Update gets called 25 times a second, and given the nature of rand() this means that each waternode
	//will be updated once every 2 seconds. It turns out that from the player's viewpoint this is just fine
	for (std::list<boost::weak_ptr<WaterNode> >::iterator wati = waterList.begin(); wati != waterList.end(); ++wati) {
		if (wati->lock() && rand() % 50 == 0) wati->lock()->Update();
		else if (!wati->lock()) wati = waterList.erase(wati);
	}

	//Updating the last 10 waternodes each time means that recently created water moves faster.
	//This has the effect of making water rush to new places such as a moat very quickly, which is the
	//expected behaviour of water.
	if (waterList.size() > 0) {
		std::list<boost::weak_ptr<WaterNode> >::iterator wati = waterList.end();
		while (std::distance(wati, waterList.end()) < 10) {
			--wati;
			if (wati->lock()) wati->lock()->Update();
		}
	}

	std::list<boost::weak_ptr<NPC> > npcsWaitingForRemoval;
	for (std::map<int,boost::shared_ptr<NPC> >::iterator npci = npcList.begin(); npci != npcList.end(); ++npci) {
		npci->second->Update();
		if (!npci->second->Dead()) npci->second->Think();
		if (npci->second->Dead() || npci->second->Escaped()) npcsWaitingForRemoval.push_back(npci->second);
	}
	JobManager::Inst()->AssignJobs();

	for (std::list<boost::weak_ptr<NPC> >::iterator remNpci = npcsWaitingForRemoval.begin(); remNpci != npcsWaitingForRemoval.end(); ++remNpci) {
		RemoveNPC(*remNpci);
	}

	for (std::map<int,boost::shared_ptr<Construction> >::iterator consi = dynamicConstructionList.begin(); consi != dynamicConstructionList.end(); ++consi) {
		consi->second->Update();
	}

	for (std::list<boost::weak_ptr<Item> >::iterator itemi = stoppedItems.begin(); itemi != stoppedItems.end();) {
		flyingItems.erase(*itemi);
		itemi = stoppedItems.erase(itemi);
	}

	for (std::set<boost::weak_ptr<Item> >::iterator itemi = flyingItems.begin(); itemi != flyingItems.end(); ++itemi) {
		if (boost::shared_ptr<Item> item = itemi->lock()) item->UpdateVelocity();
	}

	//Constantly checking our free item list for items that can be stockpiled is overkill, so it's done once every
	//15 seconds, on average, or immediately if a new stockpile is built or a stockpile's allowed items are changed.
	if (rand() % (UPDATES_PER_SECOND * 15) == 0 || refreshStockpiles) {
		refreshStockpiles = false;
		for (std::set<boost::weak_ptr<Item> >::iterator itemi = freeItems.begin(); itemi != freeItems.end(); ++itemi) {
			if (itemi->lock() && !itemi->lock()->Reserved() && itemi->lock()->GetFaction() == 0 && itemi->lock()->GetVelocity() == 0) 
				StockpileItem(*itemi);
		}
	}

	//Squads needen't update their member rosters ALL THE TIME
	if (time % (UPDATES_PER_SECOND * 1) == 0) {
		for (std::map<std::string, boost::shared_ptr<Squad> >::iterator squadi = squadList.begin(); squadi != squadList.end(); ++squadi) {
			squadi->second->UpdateMembers();
		}
	}

	if (time % (UPDATES_PER_SECOND * 1) == 0) StockManager::Inst()->Update();

	if (time % (UPDATES_PER_SECOND * 1) == UPDATES_PER_SECOND/2) JobManager::Inst()->Update();

	events->Update(safeMonths > 0);

	if (time % (UPDATES_PER_SECOND * 1) == 0) Map::Inst()->Naturify(rand() % Map::Inst()->Width(), rand() % Map::Inst()->Height());
}

boost::shared_ptr<Job> Game::StockpileItem(boost::weak_ptr<Item> item, bool returnJob) {
	boost::shared_ptr<Stockpile> nearest = boost::shared_ptr<Stockpile>();
	int nearestDistance = INT_MAX;
	for (std::map<int,boost::shared_ptr<Construction> >::iterator stocki = staticConstructionList.begin(); stocki != staticConstructionList.end(); ++stocki) {
		if (stocki->second->stockpile) {
			boost::shared_ptr<Stockpile> sp(boost::static_pointer_cast<Stockpile>(stocki->second));
			if (sp->Allowed(Item::Presets[item.lock()->Type()].specificCategories) && !sp->Full(item.lock()->Type())) {

				//Found a stockpile that both allows the item, and has space
				int distance = Distance(sp->Center(), item.lock()->Position());
				if(distance < nearestDistance) {
					nearestDistance = distance;
					nearest = sp;
				}
			}
		}
	}

	if(nearest) {
		//Check if the item can be contained, and if so if any containers are in the stockpile

		boost::shared_ptr<Job> stockJob(new Job("Store " + Item::ItemTypeToString(item.lock()->Type()) + " in stockpile", LOW));
		stockJob->Attempts(1);
		Coordinate target = Coordinate(-1,-1);
		boost::weak_ptr<Item> container;

		if (Item::Presets[item.lock()->Type()].fitsin >= 0) {
			container = nearest->FindItemByCategory(Item::Presets[item.lock()->Type()].fitsin, NOTFULL);
			if (container.lock()) {
				target = container.lock()->Position();
				stockJob->ReserveSpace(boost::static_pointer_cast<Container>(container.lock()));
			}
		}

		if (target.X() == -1) target = nearest->FreePosition();

		if (target.X() != -1) {
			stockJob->ReserveSpot(nearest, target);
			stockJob->ReserveEntity(item);
			stockJob->tasks.push_back(Task(MOVE, item.lock()->Position()));
			stockJob->tasks.push_back(Task(TAKE, item.lock()->Position(), item));
			stockJob->tasks.push_back(Task(MOVE, target));
			if (!container.lock())
				stockJob->tasks.push_back(Task(PUTIN, target, nearest->Storage(target)));
			else
				stockJob->tasks.push_back(Task(PUTIN, target, container));

			if (!returnJob) JobManager::Inst()->AddJob(stockJob);
			else return stockJob;

			return boost::shared_ptr<Job>();
		}

	}

	return boost::shared_ptr<Job>();
}

namespace {
	template <typename MapT>
	inline void InternalDrawMapItems(const char *name, MapT& map, Coordinate& upleft, TCODConsole *buffer) {
		for (typename MapT::iterator it = map.begin(); it != map.end(); ) {
			typename MapT::mapped_type ptr = it->second;

			if (ptr.get() != NULL) {
				ptr->Draw(upleft, buffer);
				++it;
			} else {
#ifdef DEBUG
				std::cout << "!!! NULL POINTER !!! " << name << " ; id " << it->first << std::endl;
#endif
				typename MapT::iterator tmp = it;
				++it;
				map.erase(tmp);
			}
		}
	}
}

void Game::Draw(Coordinate upleft, TCODConsole* buffer, bool drawUI) {
	Map::Inst()->Draw(upleft, buffer);

	InternalDrawMapItems("static constructions",  staticConstructionList, upleft, buffer);
	InternalDrawMapItems("dynamic constructions", dynamicConstructionList, upleft, buffer);
	InternalDrawMapItems("items",                 itemList, upleft, buffer);
	InternalDrawMapItems("nature objects",        natureList, upleft, buffer);
	InternalDrawMapItems("NPCs",                  npcList, upleft, buffer);

	if (drawUI) {
		UI::Inst()->Draw(upleft, buffer);
	}
}

void Game::FlipBuffer() {
	buffer->flush();
	TCODConsole::blit(buffer, 0, 0, screenWidth, screenHeight, TCODConsole::root, 0, 0);
	TCODConsole::root->flush();
}

Season Game::CurrentSeason() { return season; }

void Game::SpawnTillageJobs() {
	for (std::map<int,boost::shared_ptr<Construction> >::iterator consi = dynamicConstructionList.begin(); consi != dynamicConstructionList.end(); ++consi) {
		if (consi->second->farmplot) {
			boost::shared_ptr<Job> tillJob(new Job("Till farmplot"));
			tillJob->tasks.push_back(Task(MOVE, consi->second->Position()));
			tillJob->tasks.push_back(Task(USE, consi->second->Position(), consi->second));
			JobManager::Inst()->AddJob(tillJob);
		}
	}
}

void Game::DeTillFarmPlots() {
	for (std::map<int,boost::shared_ptr<Construction> >::iterator consi = dynamicConstructionList.begin(); consi != dynamicConstructionList.end(); ++consi) {
		if (consi->second->farmplot) {
			boost::static_pointer_cast<FarmPlot>(consi->second)->tilled = false;
		}
	}
}

//First generates a heightmap, then translates that into the corresponding tiles
//Third places plantlife according to heightmap, and some wildlife as well
void Game::GenerateMap(uint32 seed) {
	TCODRandom* random;
	if (seed == 0) random = new TCODRandom();
	else random = new TCODRandom(seed);

	Map* map = Map::Inst();
	map->heightMap->clear();

	bool riverStartLeft = random->get(0,1) ? true : false;
	bool riverEndRight = random->get(0,1) ? true : false;

	int px[4];
	int py[4];

	do {
		if (riverStartLeft) {
			px[0] = 0; 
			py[0] = random->get(0,map->Height()-1);
		} else {
			px[0] = random->get(0,map->Width()-1);
			py[0] = 0;
		}

		px[1] = 10 + random->get(0,map->Width()-20);
		py[1] = 10 + random->get(0,map->Height()-20);
		px[2] = 10 + random->get(0,map->Width()-20);
		py[2] = 10 + random->get(0,map->Height()-20);

		if (riverEndRight) {
			px[3] = map->Width()-1;
			py[3] = random->get(0,map->Height()-1);
		} else {
			px[3] = random->get(0,map->Width()-1);
			py[3] = map->Height()-1;
		}
		//This conditional ensures that the river's beginning and end are at least 100 units apart
	} while (std::sqrt( std::pow((double)px[0] - px[3], 2) + std::pow((double)py[0] - py[3], 2)) < 100);

	map->heightMap->digBezier(px, py, 50, -5, 50, -5);

	int hills = 0;
	//infinityCheck is just there to make sure our while loop doesn't become an infinite one
	//in case no suitable hill sites are found
	int infinityCheck = 0;
	while (hills < map->Width()/66 && infinityCheck < 1000) {
		int x = random->get(0,map->Width()-1);
		int y = random->get(0,map->Height()-1);
		int riverDistance;
		int distance;
		int lineX, lineY;

		riverDistance = 70;

		for (int i = 0; i < 4; ++i) {

			//We draw four lines from our potential hill site and measure the least distance to a river
			switch (i) {
			case 0:
				lineX = x - 70;
				lineY = y;
				break;

			case 1:
				lineX = x + 70;
				lineY = y;
				break;

			case 2:
				lineX = x;
				lineY = y - 70;
				break;

			case 3:
				lineX = x;
				lineY = y + 70;
				break;
			}

			distance = 70;
			TCODLine::init(lineX, lineY, x, y);
			do {
				if (lineX >= 0 && lineX < map->Width() && lineY >= 0 && lineY < map->Height()) {
					if (map->heightMap->getValue(lineX, lineY) < map->GetWaterlevel()) {
						if (distance < riverDistance) riverDistance = distance;
					}
				}
				--distance;
			} while (!TCODLine::step(&lineX, &lineY));
		}

		if (riverDistance > 35) {
			map->heightMap->addHill((float)x, (float)y, (float)random->get(15,35), (float)random->get(1,3));
			map->heightMap->addHill((float)x+random->get(-7,7), (float)y+random->get(-7,7), (float)random->get(15,25), (float)random->get(1,3));
			map->heightMap->addHill((float)x+random->get(-7,7), (float)y+random->get(-7,7), (float)random->get(15,25), (float)random->get(1,3));
			++hills;
		}

		++infinityCheck;
	}

	map->heightMap->rainErosion(map->Width()*map->Height()*5, 0.005f, 0.30f, random);

	//This is a simple kernel transformation that does some horizontal smoothing (lifted straight from the libtcod docs)
	int dx [] = {-1,1,0};
	int dy[] = {0,0,0};
	float weight[] = {0.33f,0.33f,0.33f};
	map->heightMap->kernelTransform(3, dx, dy, weight, 0.0f, 1.0f);


	//Now take the heightmap values and translate them into tiles
	for (int x = 0; x < map->Width(); ++x) {
		for (int y = 0; y < map->Height(); ++y) {
			float height = map->heightMap->getValue(x,y);
			if (height < map->GetWaterlevel()) {
				if (random->get(0,1)) map->Type(x,y,TILERIVERBED);
				else map->Type(x,y,TILEDITCH);
				CreateWater(Coordinate(x,y));
			} else if (height < 4.5f) {
				map->Type(x,y,TILEGRASS);
				if (random->get(0,9) < 9) {
					if (height < -0.01f) {
						map->ForeColor(x,y, TCODColor(random->get(100,192),127,0));
					} else if (height < 0.0f) {
						map->ForeColor(x,y, TCODColor(random->get(20,170),127,0));
					} else if (height > 4.0f) {
						map->ForeColor(x,y, TCODColor(90, random->get(120,150), 90));
					}
				}
			} else {
				map->Type(x,y,TILEROCK);
			}
		}
	}

	for (int x = 0; x < map->Width(); ++x) {
		for (int y = 0; y < map->Height(); ++y) {
			map->Naturify(x,y);
		}
	}
}

//This is intentional, otherwise designating where to cut down trees would always show red unless you were over a tree
bool Game::CheckTree(Coordinate, Coordinate) {
	return true;
}

void Game::FellTree(Coordinate a, Coordinate b) {
	for (int x = a.X(); x <= b.X(); ++x) {
		for (int y = a.Y(); y <= b.Y(); ++y) {
			int natUid = Map::Inst()->NatureObject(x,y);
			if (natUid >= 0) {
				boost::weak_ptr<NatureObject> natObj = Game::Inst()->natureList[natUid];
				if (natObj.lock() && natObj.lock()->Tree() && !natObj.lock()->Marked()) {
					natObj.lock()->Mark();
					boost::shared_ptr<Job> fellJob(new Job("Fell tree", MED, 0, true));
					fellJob->ConnectToEntity(natObj);
					fellJob->SetRequiredTool(Item::StringToItemCategory("Slashing weapon"));
					fellJob->tasks.push_back(Task(MOVEADJACENT, natObj.lock()->Position(), natObj));
					fellJob->tasks.push_back(Task(FELL, natObj.lock()->Position(), natObj));
					JobManager::Inst()->AddJob(fellJob);
				}
			}
		}
	}
}

void Game::DesignateTree(Coordinate a, Coordinate b) {
	for (int x = a.X(); x <= b.X(); ++x) {
		for (int y = a.Y(); y <= b.Y(); ++y) {
			int natUid = Map::Inst()->NatureObject(x,y);
			if (natUid >= 0) {
				boost::weak_ptr<NatureObject> natObj = Game::Inst()->natureList[natUid];
				if (natObj.lock() && natObj.lock()->Tree() && !natObj.lock()->Marked()) {
					//TODO: Implement proper map marker system and change this to use that
					natObj.lock()->Mark();
					StockManager::Inst()->UpdateTreeDesignations(natObj, true);
				}
			}
		}
	}
}

void Game::HarvestWildPlant(Coordinate a, Coordinate b) {
	for (int x = a.X(); x <= b.X(); ++x) {
		for (int y = a.Y(); y <= b.Y(); ++y) {
			int natUid = Map::Inst()->NatureObject(x,y);
			if (natUid >= 0) {
				boost::weak_ptr<NatureObject> natObj = Game::Inst()->natureList[natUid];
				if (natObj.lock() && natObj.lock()->Harvestable() && !natObj.lock()->Marked()) {
					natObj.lock()->Mark();
					boost::shared_ptr<Job> harvestJob(new Job("Harvest wild plant"));
					harvestJob->ConnectToEntity(natObj);
					harvestJob->tasks.push_back(Task(MOVEADJACENT, natObj.lock()->Position(), natObj));
					harvestJob->tasks.push_back(Task(HARVESTWILDPLANT, natObj.lock()->Position(), natObj));
					harvestJob->tasks.push_back(Task(STOCKPILEITEM));
					JobManager::Inst()->AddJob(harvestJob);
				}
			}
		}
	}
}


void Game::RemoveNatureObject(boost::weak_ptr<NatureObject> natObj) {
	Map::Inst()->NatureObject(natObj.lock()->X(), natObj.lock()->Y(), -1);
	natureList.erase(natObj.lock()->Uid());
}

bool Game::CheckTileType(TileType type, Coordinate target, Coordinate size) {
	for (int x = target.X(); x < target.X()+size.X(); ++x) {
		for (int y = target.Y(); y < target.Y()+size.Y(); ++y) {
			if (Map::Inst()->Type(x,y) == type) return true;
		}
	}
	return false;
}

void Game::DesignateBog(Coordinate a, Coordinate b) {
	for (int x = a.X(); x <= b.X(); ++x) {
		for (int y = a.Y(); y <= b.Y(); ++y) {
			if (Map::Inst()->Type(x,y) == TILEBOG) {
				StockManager::Inst()->UpdateBogDesignations(Coordinate(x,y), true);
				Map::Inst()->Mark(x,y);
			}
		}
	}
}

void Game::Undesignate(Coordinate a, Coordinate b) {
	for (int x = a.X(); x <= b.X(); ++x) {
		for (int y = a.Y(); y <= b.Y(); ++y) {	
			int natUid = Map::Inst()->NatureObject(x,y);
			if (natUid >= 0) {
				boost::weak_ptr<NatureObject> natObj = Game::Inst()->natureList[natUid];
				if (natObj.lock() && natObj.lock()->Tree() && natObj.lock()->Marked()) {
					//TODO: Implement proper map marker system and change this to use that
					natObj.lock()->Unmark();
					StockManager::Inst()->UpdateTreeDesignations(natObj, false);
				}
			}
			if (Map::Inst()->Type(x,y) == TILEBOG) {
				StockManager::Inst()->UpdateBogDesignations(Coordinate(x,y), false);
				Map::Inst()->Unmark(x,y);
			}
		}
	}
}

std::string Game::SeasonToString(Season season) {
	switch (season) {
	case EarlySpring: return "Early Spring";
	case Spring: return "Spring";
	case LateSpring: return "Late Spring";
	case EarlySummer: return "Early Summer";
	case Summer: return "Summer";
	case LateSummer: return "Late Summer";
	case EarlyFall: return "Early Fall";
	case Fall: return "Fall";
	case LateFall: return "Late Fall";
	case EarlyWinter: return "Early Winter";
	case Winter: return "Winter";
	case LateWinter: return "Late Winter";
	default: return "???";
	}
}

void Game::DecayItems() {
	std::list<int> eraseList;
	std::list<std::pair<ItemType, Coordinate> > creationList;
	for (std::map<int,boost::shared_ptr<Item> >::iterator itemit = itemList.begin(); itemit != itemList.end(); ++itemit) {
		if (itemit->second->decayCounter > 0) {
			if (--itemit->second->decayCounter == 0) {
				for (std::vector<ItemType>::iterator decaylisti = Item::Presets[itemit->second->type].decayList.begin(); decaylisti != Item::Presets[itemit->second->type].decayList.end(); ++decaylisti) {
					creationList.push_back(std::pair<ItemType, Coordinate>(*decaylisti, itemit->second->Position()));
				}
				eraseList.push_back(itemit->first);
			}
		}
	}

	for (std::list<int>::iterator delit = eraseList.begin(); delit != eraseList.end(); ++delit) {
		RemoveItem(GetItem(*delit));
	}

	for (std::list<std::pair<ItemType, Coordinate> >::iterator crit = creationList.begin(); crit != creationList.end(); ++crit) {
		if (crit->first >= 0) {
			CreateItem(crit->second, crit->first, false);
		} else {
			CreateFilth(crit->second);
		}
	}

	for (std::list<boost::weak_ptr<BloodNode> >::iterator bli = bloodList.begin(); bli != bloodList.end(); ++bli) {
		if (boost::shared_ptr<BloodNode> blood = bli->lock()) {
			blood->Depth(blood->Depth()-50);
			if (blood->Depth() <= 0) {
				Map::Inst()->SetBlood(blood->Position().X(), blood->Position().Y(), boost::shared_ptr<BloodNode>());
				bli = bloodList.erase(bli);
			}
		} else {
			bli = bloodList.erase(bli);
		}
	}
}

void Game::CreateFilth(Coordinate pos) {
	CreateFilth(pos, 1);
}

void Game::CreateFilth(Coordinate pos, int amount) {
	boost::weak_ptr<FilthNode> filth(Map::Inst()->GetFilth(pos.X(), pos.Y()));
	if (!filth.lock()) {
		boost::shared_ptr<FilthNode> newFilth(new FilthNode(pos.X(), pos.Y(), amount));
		filthList.push_back(boost::weak_ptr<FilthNode>(newFilth));
		Map::Inst()->SetFilth(pos.X(), pos.Y(), newFilth);
	} else {filth.lock()->Depth(filth.lock()->Depth()+amount);}
}

void Game::CreateBlood(Coordinate pos) {
	CreateBlood(pos, 100);
}

void Game::CreateBlood(Coordinate pos, int amount) {
	boost::weak_ptr<BloodNode> blood(Map::Inst()->GetBlood(pos.X(), pos.Y()));
	if (!blood.lock()) {
		boost::shared_ptr<BloodNode> newBlood(new BloodNode(pos.X(), pos.Y(), amount));
		bloodList.push_back(boost::weak_ptr<BloodNode>(newBlood));
		Map::Inst()->SetBlood(pos.X(), pos.Y(), newBlood);
	} else {blood.lock()->Depth(blood.lock()->Depth()+amount);}
}


//This function uses straightforward raycasting, and it is somewhat imprecise right now as it only casts a ray to every second
//tile at the edge of the line of sight distance. This is to conserve cpu cycles, as there may be several hundred creatures
//active at a time, and given the fact that they'll usually be constantly moving, this function needen't be 100% accurate.
void Game::FindNearbyNPCs(boost::shared_ptr<NPC> npc, bool onlyHostiles) {
	npc->nearNpcs.clear();
	for (int endx = std::max((signed int)npc->x - LOS_DISTANCE, 0); endx <= std::min((signed int)npc->x + LOS_DISTANCE, Map::Inst()->Width()-1); endx += 2) {
		for (int endy = std::max((signed int)npc->y - LOS_DISTANCE, 0); endy <= std::min((signed int)npc->y + LOS_DISTANCE, Map::Inst()->Height()-1); endy += 2) {
			if (endx == std::max((signed int)npc->x - LOS_DISTANCE, 0) || endx == std::min((signed int)npc->x + LOS_DISTANCE, Map::Inst()->Width()-1)
				|| endy == std::max((signed int)npc->y - LOS_DISTANCE, 0) || endy == std::min((signed int)npc->y + LOS_DISTANCE, Map::Inst()->Height()-1)) {
					int x = npc->x;
					int y = npc->y;
					TCODLine::init(x, y, endx, endy);
					do {
						if (Map::Inst()->BlocksLight(x,y)) break;
						for (std::set<int>::iterator npci = Map::Inst()->NPCList(x,y)->begin(); npci != Map::Inst()->NPCList(x,y)->end(); ++npci) {
							if (*npci != npc->uid) {
								if (!onlyHostiles || (onlyHostiles && npcList[*npci]->faction != npc->faction)) npc->nearNpcs.push_back(npcList[*npci]);
							}
						}

						if (npc->nearNpcs.size() > 10) break;

					} while(!TCODLine::step(&x, &y));
			}
		}
	}
}

void Game::Pause() {
	paused = !paused;
}

bool Game::Paused() { return paused; }

int Game::CharHeight() const { return charHeight; }
int Game::CharWidth() const { return charWidth; }

void Game::RemoveNPC(boost::weak_ptr<NPC> wnpc) {
	if (boost::shared_ptr<NPC> npc = wnpc.lock()) {
		if (boost::iequals(npc->name, "orc")) --orcCount;
		else if (boost::iequals(npc->name, "goblin")) --goblinCount;
		npcList.erase(npc->uid);
	}
}

int Game::FindMilitaryRecruit() {
	for (std::map<int, boost::shared_ptr<NPC> >::iterator npci = npcList.begin(); npci != npcList.end(); ++npci) {
		if (npci->second->type == NPC::StringToNPCType("orc") && npci->second->faction == 0 && !npci->second->squad.lock()) {
			return npci->second->uid;
		}
	}
	return -1;
}

void Game::CreateSquad(std::string name) {
	squadList.insert(std::pair<std::string, boost::shared_ptr<Squad> >(name, boost::shared_ptr<Squad>(new Squad(name))));
}

void Game::SetSquadTargetCoordinate(Coordinate target, boost::shared_ptr<Squad> squad) {
	squad->TargetCoordinate(target);
	UI::Inst()->CloseMenu();
	Announce::Inst()->AddMsg((boost::format("[%1%] guarding position (%2%,%3%)") % squad->Name() % target.X() % target.Y()).str(), TCODColor::white, target);
}
void Game::SetSquadTargetEntity(Coordinate target, boost::shared_ptr<Squad> squad) {
	if (target.X() >= 0 && target.X() < Map::Inst()->Width() && target.Y() >= 0 && target.Y() < Map::Inst()->Height()) {
		std::set<int> *npcList = Map::Inst()->NPCList(target.X(), target.Y());
		if (!npcList->empty()) {
			squad->TargetEntity(Game::Inst()->npcList[*npcList->begin()]);
			UI::Inst()->CloseMenu();
			Announce::Inst()->AddMsg((boost::format("[%1%] escorting %2%") % squad->Name() % squad->TargetEntity().lock()->Name()).str(), TCODColor::white, target);
		}
	}
}

// Spawns NPCs distributed randomly within the rectangle defined by corner1 & corner2
void Game::CreateNPCs(int quantity, NPCType type, Coordinate corner1, Coordinate corner2) {
	int areaWidth = std::max(abs(corner1.X()-corner2.X()), 1);
	int areaLength = std::max(abs(corner1.Y()-corner2.Y()), 1);

	for (int npcs = 0; npcs < quantity; ++npcs) {
		Coordinate location((rand() % areaWidth) + std::min(corner1.X(),corner2.X()), (rand() % areaLength) + std::min(corner1.Y(),corner2.Y()));

		Game::Inst()->CreateNPC(location, type);
	}
}

int Game::DiceToInt(TCOD_dice_t dice) {
	if (dice.nb_faces == 0) 
		dice.nb_faces = 1;
	return (int)(((dice.nb_dices * ((rand() % dice.nb_faces) + 1)) *
		dice.multiplier) + dice.addsub);
}

void Game::ToMainMenu(bool value) { Game::Inst()->toMainMenu = value; }
bool Game::ToMainMenu() { return Game::Inst()->toMainMenu; }

void Game::Running(bool value) { running = value; }
bool Game::Running() { return running; }

boost::weak_ptr<Construction> Game::FindConstructionByTag(ConstructionTag tag) {
	for (std::map<int, boost::shared_ptr<Construction> >::iterator stati = staticConstructionList.begin();
		stati != staticConstructionList.end(); ++stati) {
			if (!stati->second->Reserved() && stati->second->HasTag(tag)) return stati->second;
	}

	for (std::map<int, boost::shared_ptr<Construction> >::iterator dynai = dynamicConstructionList.begin();
		dynai != dynamicConstructionList.end(); ++dynai) {
			if (!dynai->second->Reserved() && dynai->second->HasTag(tag)) return dynai->second;
	}

	return boost::weak_ptr<Construction>();
}

void Game::Reset() {
	npcList.clear();
	squadList.clear();
	hostileSquadList.clear();
	itemList.clear();
	staticConstructionList.clear();
	dynamicConstructionList.clear();
	natureList.clear();
	waterList.clear();
	filthList.clear();
	bloodList.clear();
	JobManager::Inst()->Reset();
	StockManager::Inst()->Reset();
	time = 0;
	paused = false;
	toMainMenu = false;
	running = false;
	events = boost::shared_ptr<Events>(new Events(Map::Inst()));
	season = LateWinter;
	upleft = Coordinate(180,180);
	safeMonths = 9;
	Announce::Inst()->Reset();
	for (int x = 0; x < Map::Inst()->Width(); ++x) {
		for (int y = 0; y < Map::Inst()->Height(); ++y) {
			Map::Inst()->Reset(x,y);
		}
	}
}

NPCType Game::GetRandomNPCTypeByTag(std::string tag) {
	std::vector<NPCType> npcList;
	for (unsigned int i = 0; i < NPC::Presets.size(); ++i) {
		if (NPC::Presets[i].tags.find(boost::to_lower_copy(tag)) != NPC::Presets[i].tags.end()) {
			npcList.push_back(i);
		}
	}
	if (npcList.size() > 0)
		return npcList[rand() % npcList.size()];
	return -1;
}

void Game::CenterOn(Coordinate target) {
	int x = std::max(0, target.X() - ScreenWidth() / 2);
	int y = std::max(0, target.Y() - ScreenHeight() / 2);
	upleft = Coordinate(x, y);
}

void Game::SetMark(int i) {
	marks[i] = Coordinate(upleft.X(), upleft.Y());
}

void Game::ReturnToMark(int i) {
	upleft = Coordinate(marks[i].X(), marks[i].Y());
}

void Game::TranslateContainerListeners() {
	for (std::map<int,boost::shared_ptr<Item> >::iterator it = itemList.begin(); it != itemList.end(); ++it) {
		if (boost::dynamic_pointer_cast<Container>(it->second)) {
			boost::static_pointer_cast<Container>(it->second)->TranslateContainerListeners();
		}
	}
	for (std::map<int, boost::shared_ptr<Construction> >::iterator it = staticConstructionList.begin(); 
		it != staticConstructionList.end(); ++it) {
			if (boost::dynamic_pointer_cast<Stockpile>(it->second)) {
				boost::static_pointer_cast<Stockpile>(it->second)->TranslateInternalContainerListeners();
			}
	}
	for (std::map<int, boost::shared_ptr<Construction> >::iterator it = dynamicConstructionList.begin(); 
		it != dynamicConstructionList.end(); ++it) {
			if (boost::dynamic_pointer_cast<Stockpile>(it->second)) {
				boost::static_pointer_cast<Stockpile>(it->second)->TranslateInternalContainerListeners();
			}
	}
}

unsigned int Game::PeacefulFaunaCount() const { return peacefulFaunaCount; }
void Game::PeacefulFaunaCount(int add) { peacefulFaunaCount += add; }

bool Game::DevMode() { return devMode; }
void Game::EnableDevMode() { devMode = true; }

void Game::Dig(Coordinate a, Coordinate b) {
	for (int x = a.X(); x <= b.X(); ++x) {
		for (int y = a.Y(); y <= b.Y(); ++y) {
			/*TODO: Relying on GroundMarked() is iffy, it doesn't necessarily mean that that
			spot is reserved for digging. */
			if (CheckPlacement(Coordinate(x,y), Coordinate(1,1)) && !Map::Inst()->GroundMarked(x,y) && !Map::Inst()->Low(x,y)) {
				boost::shared_ptr<Job> digJob(new Job("Dig"));
				digJob->SetRequiredTool(Item::StringToItemCategory("Digging tool"));
				digJob->MarkGround(Coordinate(x,y));
				digJob->Attempts(1000);
				digJob->tasks.push_back(Task(MOVEADJACENT, Coordinate(x,y)));
				digJob->tasks.push_back(Task(DIG, Coordinate(x,y)));
				JobManager::Inst()->AddJob(digJob);
			}
		}
	}
}

Coordinate Game::FindClosestAdjacent(Coordinate from, Coordinate target) {
	Coordinate closest = Coordinate(-9999, -9999);
	for (int ix = target.X()-1; ix <= target.X()+1; ++ix) {
		for (int iy = target.Y()-1; iy <= target.Y()+1; ++iy) {
			if (ix == target.X()-1 || ix == target.X()+1 ||
				iy == target.Y()-1 || iy == target.Y()+1) {
					if (Map::Inst()->Walkable(ix,iy)) {
						if (Distance(from.X(), from.Y(), ix, iy) < Distance(from.X(), from.Y(), closest.X(), closest.Y()))
							closest = Coordinate(ix,iy);
					}
			}
		}
	}
	return closest;
}

bool Game::Adjacent(Coordinate a, Coordinate b) {
	if (std::abs(a.X() - b.X()) < 2 && std::abs(a.Y() - b.Y()) < 2) return true;
	return false;
}

void Game::CreateNatureObject(Coordinate location) {
	if (Map::Inst()->Walkable(location.X(),location.Y()) && Map::Inst()->Type(location.X(),location.Y()) == TILEGRASS && rand() % 5 < 2) {
		std::priority_queue<std::pair<int, int> > natureObjectQueue;
		float height = Map::Inst()->heightMap->getValue(location.X(),location.Y());

		//Populate the priority queue with all possible plants and give each one a random
		//value based on their rarity
		bool evil = Map::Inst()->GetCorruption(location.X(), location.Y()) >= 100;
		for (unsigned int i = 0; i < NatureObject::Presets.size(); ++i) {
			if (NatureObject::Presets[i].minHeight <= height &&
				NatureObject::Presets[i].maxHeight >= height &&
				NatureObject::Presets[i].evil == evil)
				natureObjectQueue.push(std::pair<int,int>(rand() % NatureObject::Presets[i].rarity + rand() % 3, i));
		}

		if (natureObjectQueue.empty()) return;
		int chosen = natureObjectQueue.top().second;
		int rarity = NatureObject::Presets[chosen].rarity;
		if (std::abs(height - NatureObject::Presets[chosen].minHeight) <= 0.01f ||
			std::abs(height - NatureObject::Presets[chosen].maxHeight) <= 0.5f) rarity = rarity - rarity / 5;
		if (std::abs(height - NatureObject::Presets[chosen].minHeight) <= 0.005f ||
			std::abs(height - NatureObject::Presets[chosen].maxHeight) <= 0.05f) rarity /= 2;

		if (rand() % 100 < rarity) {

			for (int clus = 0; clus < NatureObject::Presets[chosen].cluster; ++clus) {
				int ax = location.X() + ((rand() % NatureObject::Presets[chosen].cluster) - NatureObject::Presets[chosen].cluster/2);
				int ay = location.Y() + ((rand() % NatureObject::Presets[chosen].cluster) - NatureObject::Presets[chosen].cluster/2);
				if (ax < 0) ax = 0; if (ax >= Map::Inst()->Width()) ax = Map::Inst()->Width()-1;
				if (ay < 0) ay = 0; if (ay >= Map::Inst()->Height()) ay = Map::Inst()->Height()-1;
				if (Map::Inst()->Walkable(ax,ay) && Map::Inst()->Type(ax,ay) == TILEGRASS &&
					Map::Inst()->NatureObject(ax,ay) < 0 &&
					Map::Inst()->GetConstruction(ax, ay) < 0) {
						boost::shared_ptr<NatureObject> natObj(new NatureObject(Coordinate(ax,ay), chosen));
						natureList.insert(std::pair<int, boost::shared_ptr<NatureObject> >(natObj->Uid(), natObj));
						Map::Inst()->NatureObject(ax,ay,natObj->Uid());
						Map::Inst()->SetWalkable(ax,ay,NatureObject::Presets[natObj->Type()].walkable);
						Map::Inst()->Buildable(ax,ay,NatureObject::Presets[natObj->Type()].walkable);
						Map::Inst()->BlocksLight(ax,ay,!NatureObject::Presets[natObj->Type()].walkable);
				}
			}
		}
	}
}

void Game::CreateNatureObject(Coordinate location, std::string name) {
	unsigned int natureObjectIndex = 0;
	for (std::vector<NatureObjectPreset>::iterator preseti = NatureObject::Presets.begin(); preseti != NatureObject::Presets.end();
		++preseti) {
			if (boost::iequals(preseti->name, name)) break;
			++natureObjectIndex;
	}

	if (natureObjectIndex < NatureObject::Presets.size() && 
		boost::iequals(NatureObject::Presets[natureObjectIndex].name, name)) {
		if (location.X() >= 0 && location.X() < Map::Inst()->Width() && 
			location.Y() >= 0 && location.Y() < Map::Inst()->Height() &&
			Map::Inst()->NatureObject(location.X(),location.Y()) < 0 &&
			Map::Inst()->GetConstruction(location.X(), location.Y()) < 0) {
				boost::shared_ptr<NatureObject> natObj(new NatureObject(Coordinate(location.X(),location.Y()), natureObjectIndex));
				natureList.insert(std::pair<int, boost::shared_ptr<NatureObject> >(natObj->Uid(), natObj));
				Map::Inst()->NatureObject(location.X(),location.Y(),natObj->Uid());
				Map::Inst()->SetWalkable(location.X(),location.Y(),NatureObject::Presets[natObj->Type()].walkable);
				Map::Inst()->Buildable(location.X(),location.Y(),NatureObject::Presets[natObj->Type()].walkable);
				Map::Inst()->BlocksLight(location.X(),location.Y(),!NatureObject::Presets[natObj->Type()].walkable);
		}

	}
}

void Game::RemoveNatureObject(Coordinate a, Coordinate b) {
	for (int x = a.X(); x <= b.X(); ++x) {
		for (int y = a.Y(); y <= b.Y(); ++y) {
			int uid = Map::Inst()->NatureObject(x,y);
			if (uid >= 0) {
				Map::Inst()->NatureObject(x,y,-1);
				natureList.erase(uid);
			}
		}
	}
}