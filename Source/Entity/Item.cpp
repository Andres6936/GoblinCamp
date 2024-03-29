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


#include <libtcod.hpp>
#include <memory>
#include <boost/algorithm/string.hpp>

#ifdef DEBUG
#include <iostream>
#include <boost/format.hpp>
#endif

#include <boost/serialization/weak_ptr.hpp>

#include "Goblin/Math/Random.hpp"
#include "Goblin/Entity/Item.hpp"
#include "Goblin/Util/Game.hpp"
#include "Goblin/Eden/Map.hpp"
#include "Goblin/Log/Logger.hpp"
#include "Goblin/Mechanism/StockManager.hpp"
#include "Goblin/Mechanism/Attack.hpp"
#include "Goblin/Mechanism/Faction.hpp"

std::vector<ItemPreset> Item::Presets = std::vector<ItemPreset>();
std::vector<ItemCat> Item::Categories = std::vector<ItemCat>();
std::vector<ItemCat> Item::ParentCategories = std::vector<ItemCat>();
boost::unordered_map<std::string, ItemType> Item::itemTypeNames = boost::unordered_map<std::string, ItemType>();
boost::unordered_map<std::string, ItemType> Item::itemCategoryNames = boost::unordered_map<std::string, ItemType>();
std::multimap<StatusEffectType, ItemType> Item::EffectRemovers = std::multimap<StatusEffectType, ItemType>();
std::multimap<StatusEffectType, ItemType> Item::GoodEffectAdders = std::multimap<StatusEffectType, ItemType>();

Item::Item(const Coordinate& startPos, ItemType typeval, int owner,
		std::vector<std::shared_ptr<Item> > components) :
		Entity(),

		type(typeval),
		flammable(false),
		decayCounter(-1),

		attemptedStore(false),
		container(std::shared_ptr<Item>()),
		internal(false)
{
	SetFaction(owner);
	pos = startPos;

	//Remember that the components are destroyed after this constructor!
	if (type >= 0 && type < static_cast<int>(Item::Presets.size())) {
		name = Item::Presets[type].name;
		categories = Item::Presets[type].categories;
		graphic = Item::Presets[type].graphic;
		color = Item::Presets[type].color;
		if (Item::Presets[type].decays) decayCounter = Item::Presets[type].decaySpeed;

		attack = Item::Presets[type].attack;

		for (int i = 0; i < RES_COUNT; ++i) {
			resistances[i] = Item::Presets[type].resistances[i];
		}

		bulk = Item::Presets[type].bulk;
		condition = Item::Presets[type].condition;

		//Calculate flammability based on categorical flammability, and then modify it based on components
		int flame = 0;
		for (int categorie : categories) {
			if (Item::Categories[categorie].flammable) flame += 2;
			else --flame;
		}

		if (components.size() > 0) {
			for (auto & component : components)
			{
				if (component)
				{
					color = TCODColor::lerp(color, component->Color(), 0.35f);
					if (component->IsFlammable()) flame += 2;
					else --flame;
				}
			}
		} else if (Item::Presets[type].components.size() > 0) { /*This item was created without real components
																ie. not in a workshop. We still should approximate
																flammability so that it behaves like on built in a 
																workshop would*/
			for (int component : Item::Presets[type].components) {
				if (Item::Categories[component].flammable) flame += 3;
				else --flame;
			}
		}

		if (flame > 0) {
			flammable = true;
#ifdef DEBUG
			std::cout<<"Created flammable object "<<flame<<"\n";
#endif
		} else {
#ifdef DEBUG
			std::cout<<"Created not flammable object "<<flame<<"\n";
#endif
		}
	}

}

Item::~Item() {
#ifdef DEBUG
	std::cout<<name<<"("<<uid<<") destroyed\n";
#endif
	if (faction == PLAYERFACTION) {
		StockManager::Inst()->UpdateQuantity(type, -1);
	}
}

int Item::GetGraphicsHint() const {
	if (type > 0)
		return Presets[type].graphicsHint;
	return 0;
}

void Item::Draw(Coordinate upleft, TCODConsole* console) {
	int screenx = (pos - upleft).getX();
	int screeny = (pos - upleft).getY();
	if (screenx >= 0 && screenx < console->getWidth() && screeny >= 0 && screeny < console->getHeight()) {
		console->putCharEx(screenx, screeny, graphic, color, map->GetBackColor(pos));
	}
}

ItemType Item::Type() const {return type;}
bool Item::IsCategory(ItemCategory category) { return (categories.find(category) != categories.end());}
TCODColor Item::Color() {return color;}
void Item::Color(TCODColor col) {color = col;}

void Item::Position(const Coordinate& p) {
	if (map->IsInside(p)) {
		if (!internal && !container) map->ItemList(pos)->erase(uid);
		pos = p;
		if (!internal && !container) map->ItemList(pos)->insert(uid);
	}
}
Coordinate Item::Position()
{
	if (container) return container->Position();
	return this->Entity::Position();
}

void Item::Reserve(bool value)
{
	reserved = value;
	if (!reserved && !container && !attemptedStore)
	{
		attemptedStore = true;
		Game::Inst()->StockpileItem(std::static_pointer_cast<Item>(shared_from_this()));
	}
}

void Item::PutInContainer(std::shared_ptr<Item> con)
{
	container = con;
	attemptedStore = false;

	Game::Inst()->ItemContained(std::static_pointer_cast<Item>(shared_from_this()), !!container);

	if (!container && !reserved)
	{
		Game::Inst()->StockpileItem(std::static_pointer_cast<Item>(shared_from_this()));
		attemptedStore = true;
	}
}

std::shared_ptr<Item> Item::ContainedIn()
{
	return container;
}

int Item::GetGraphic() {return graphic;}

Attack Item::GetAttack() const {return attack;}

std::string Item::ItemTypeToString(ItemType type) {
	if (type >= 0 && type < static_cast<int>(Item::Presets.size()))
		return Item::Presets[type].name;
	return "None";
}

ItemType Item::StringToItemType(std::string str) {
	boost::to_upper(str);
	if (itemTypeNames.find(str) == itemTypeNames.end()) {
		return -1;
	}
	return itemTypeNames[str];
}

std::string Item::ItemCategoryToString(ItemCategory category) {
	if (category >= 0 && category < static_cast<int>(Item::Categories.size()))
		return Item::Categories[category].name;
	return "None";
}

ItemCategory Item::StringToItemCategory(std::string str) {
	boost::to_upper(str);
	if (itemCategoryNames.find(str) == itemCategoryNames.end()) {
		return -1;
	}
	return Item::itemCategoryNames[str];
}

std::vector<ItemCategory> Item::Components(ItemType type) {
	if (type > 0) 
		return Item::Presets[type].components;
	return {};
}

ItemCategory Item::Components(ItemType type, int index) {
	if (type > 0)
		return Item::Presets[type].components[index];
	return 0;
}

class ItemListener : public ITCODParserListener {
	/*preset[x] holds item names as strings untill all items have been
	read, and then they are converted into ItemTypes */
	std::map<int, std::string> presetGrowth;
	std::map<int, std::vector<std::string> > presetFruits;
	std::map<int, std::vector<std::string> > presetDecay;
	std::map<int, std::string> presetProjectile;
	std::map<int, std::string> presetCategoryParent;
	int itemIndex, categoryIndex;

public:
	ItemListener() : ITCODParserListener() {}

	void translateNames() {
		//Translate category parents
		for (std::pair<const int, std::string>& pati : presetCategoryParent) {
			if (!pati.second.empty()) {
				Item::Categories[pati.first].parent = Item::StringToItemCategory(pati.second);
			}
		}

		//Growth items
		for (std::pair<const int, std::string> & growthi : presetGrowth) {
				if (!growthi.second.empty()) Item::Presets[growthi.first].growth = Item::StringToItemType(growthi.second);
				//We'll handle the items categories' parent categories while we're at it
				for (std::set<ItemCategory>::iterator cati = Item::Presets[growthi.first].categories.begin();
					cati != Item::Presets[growthi.first].categories.end(); ++cati) {
						if (Item::Categories[*cati].parent >= 0 && 
							Item::Presets[growthi.first].categories.find(Item::Categories[*cati].parent) ==
							Item::Presets[growthi.first].categories.end()) {
								Item::Presets[growthi.first].categories.insert(Item::StringToItemCategory(Item::Categories[Item::Categories[*cati].parent].name));
								cati = Item::Presets[growthi.first].categories.begin(); //Start from the beginning, inserting into a set invalidates the iterator
						}
				}
		}

		//Fruits
		for (auto & presetFruit : presetFruits)
		{
			for (std::vector<std::string>::iterator fruiti = presetFruit.second.begin();
				 fruiti != presetFruit.second.end(); ++fruiti)
			{
				Item::Presets[presetFruit.first].fruits.push_back(Item::StringToItemType(*fruiti));
			}
		}

		//Decay
		for (auto & itemi : presetDecay)
		{
			for (std::vector<std::string>::iterator decayi = itemi.second.begin();
				 decayi != itemi.second.end(); ++decayi)
			{
				if (boost::iequals(*decayi, "Filth"))
					Item::Presets[itemi.first].decayList.push_back(-1);
				else
					Item::Presets[itemi.first].decayList.push_back(Item::StringToItemType(*decayi));
			}
		}

		//Projectiles
		for (auto & proji : presetProjectile) {
				Item::Presets[proji.first].attack.Projectile(Item::StringToItemCategory(proji.second));
		}
	}

private:
	bool parserNewStruct(TCODParser *parser,const TCODParserStruct *str,const char *name) {
		if (name && boost::iequals(str->getName(), "category_type")) {
			std::string strName(name);
			boost::to_upper(strName);
			if (Item::itemCategoryNames.find(strName) != Item::itemCategoryNames.end()) {
				categoryIndex = Item::itemCategoryNames[strName];
				Item::Categories[categoryIndex] = ItemCat();
				Item::Categories[categoryIndex].name = name;
				presetCategoryParent[categoryIndex] = "";
			} else { //New category
				Item::Categories.push_back(ItemCat());
				categoryIndex = Item::Categories.size() - 1;
				++Game::ItemCatCount;
				Item::Categories.back().name = name;
				Item::itemCategoryNames.insert(std::make_pair(strName, Game::ItemCatCount-1));
				presetCategoryParent.insert(std::make_pair(categoryIndex, ""));
			}
		} else if (name && boost::iequals(str->getName(), "item_type")) {
			std::string strName(name);
			boost::to_upper(strName);
			if (Item::itemTypeNames.find(strName) != Item::itemTypeNames.end()) {
				itemIndex = Item::itemTypeNames[strName];
				Item::Presets[itemIndex] = ItemPreset();
				Item::Presets[itemIndex].name = name;
				presetGrowth[itemIndex] = "";
				presetFruits[itemIndex] = std::vector<std::string>();
				presetDecay[itemIndex] = std::vector<std::string>();
				presetProjectile[itemIndex] = "";
			} else { //New item
				Item::Presets.push_back(ItemPreset());
				itemIndex = Item::Presets.size() - 1;
				presetGrowth.insert(std::make_pair(itemIndex, ""));
				presetFruits.insert(std::make_pair(itemIndex, std::vector<std::string>()));
				presetDecay.insert(std::make_pair(itemIndex, std::vector<std::string>()));
				++Game::ItemTypeCount;
				Item::Presets.back().name = name;
				Item::itemTypeNames.insert(std::make_pair(strName, Game::ItemTypeCount-1));
				presetProjectile.insert(std::make_pair(itemIndex, ""));
			}
		} else if (boost::iequals(str->getName(), "attack")) {
		}

		return true;
	}

	bool parserFlag(TCODParser *parser,const char *name) override {
		if (boost::iequals(name, "flammable")) {
			Item::Categories[categoryIndex].flammable = true;
		}
		return true;
	}

	bool parserProperty(TCODParser *parser,const char *name, TCOD_value_type_t type, TCOD_value_t value) override {
		if (boost::iequals(name, "category")) {
			for (int i = 0; i < TCOD_list_size(value.list); ++i) {
				ItemCategory cat = Item::StringToItemCategory((char*)TCOD_list_get(value.list,i));
				Item::Presets[itemIndex].categories.insert(cat);
				Item::Presets[itemIndex].specificCategories.insert(cat);
			}
		} else if (boost::iequals(name, "graphic")) {
			Item::Presets[itemIndex].graphic = value.i;
		} else if (boost::iequals(name, "color")) {
			Item::Presets[itemIndex].color = value.col;
		} else if (boost::iequals(name, "fallbackGraphicsSet")) {
			Item::Presets[itemIndex].fallbackGraphicsSet = value.s;
		}else if (boost::iequals(name, "components")) {
			for (int i = 0; i < TCOD_list_size(value.list); ++i) {
				Item::Presets[itemIndex].components.push_back(Item::StringToItemCategory((char*)TCOD_list_get(value.list, i)));
			}
		} else if (boost::iequals(name, "containin")) {
			Item::Presets[itemIndex].containInRaw = value.s;
		} else if (boost::iequals(name, "nutrition")) {
			Item::Presets[itemIndex].nutrition = static_cast<int>(value.f * MONTH_LENGTH);
			Item::Presets[itemIndex].organic = true;
		} else if (boost::iequals(name, "growth")) {
			presetGrowth[itemIndex] = value.s;
			Item::Presets[itemIndex].organic = true;
		} else if (boost::iequals(name, "fruits")) {
			for (int i = 0; i < TCOD_list_size(value.list); ++i) {
				presetFruits[itemIndex].push_back((char*)TCOD_list_get(value.list,i));
			}
			Item::Presets[itemIndex].organic = true;
		} else if (boost::iequals(name, "multiplier")) {
			Item::Presets[itemIndex].multiplier = value.i;
		} else if (boost::iequals(name, "containerSize")) {
			Item::Presets[itemIndex].container = value.i;
		} else if (boost::iequals(name, "fitsin")) {
			Item::Presets[itemIndex].fitsInRaw = value.s;
		} else if (boost::iequals(name, "constructedin")) {
			Item::Presets[itemIndex].constructedInRaw = value.s;
		} else if (boost::iequals(name, "decay")) {
			Item::Presets[itemIndex].decays = true;
			for (int i = 0; i < TCOD_list_size(value.list); ++i) {
				presetDecay[itemIndex].push_back((char*)TCOD_list_get(value.list,i));
			}
		} else if (boost::iequals(name, "decaySpeed")) {
			Item::Presets[itemIndex].decaySpeed = value.i;
			Item::Presets[itemIndex].decays = true;
		} else if (boost::iequals(name,"type")) {
			Item::Presets[itemIndex].attack.Type(Attack::StringToDamageType(value.s));
		} else if (boost::iequals(name,"damage")) {
			Item::Presets[itemIndex].attack.Amount(value.dice);
		} else if (boost::iequals(name,"cooldown")) {
			Item::Presets[itemIndex].attack.CooldownMax(value.i);
		} else if (boost::iequals(name,"statusEffects")) {
			for (int i = 0; i < TCOD_list_size(value.list); ++i) {
				StatusEffectType type = StatusEffect::StringToStatusEffectType((char*)TCOD_list_get(value.list,i));
				if (StatusEffect::IsApplyableStatusEffect(type))
					Item::Presets[itemIndex].attack.StatusEffects()->push_back(std::pair<StatusEffectType, int>(type, 100));
			}
		} else if (boost::iequals(name,"effectChances")) {
			for (int i = 0; i < TCOD_list_size(value.list); ++i) {
				Item::Presets[itemIndex].attack.StatusEffects()->at(i).second = (intptr_t)TCOD_list_get(value.list,i);
			}
		} else if (boost::iequals(name,"ammo")) {
			presetProjectile[itemIndex] = value.s;
		} else if (boost::iequals(name,"parent")) {
			presetCategoryParent[categoryIndex] = value.s;
		} else if (boost::iequals(name,"physical")) {
			Item::Presets[itemIndex].resistances[PHYSICAL_RES] = value.i;
		} else if (boost::iequals(name,"magic")) {
			Item::Presets[itemIndex].resistances[MAGIC_RES] = value.i;
		} else if (boost::iequals(name,"cold")) {
			Item::Presets[itemIndex].resistances[COLD_RES] = value.i;
		} else if (boost::iequals(name,"fire")) {
			Item::Presets[itemIndex].resistances[FIRE_RES] = value.i;
		} else if (boost::iequals(name,"poison")) {
			Item::Presets[itemIndex].resistances[POISON_RES] = value.i;
		} else if (boost::iequals(name,"bleeding")) {
			Item::Presets[itemIndex].resistances[BLEEDING_RES] = value.i;
		} else if (boost::iequals(name,"bulk")) {
			Item::Presets[itemIndex].bulk = value.i;
		} else if (boost::iequals(name,"durability")) {
			Item::Presets[itemIndex].condition = value.i;
		} else if (boost::iequals(name,"addStatusEffects")) {
			for (int i = 0; i < TCOD_list_size(value.list); ++i) {
				StatusEffectType type = StatusEffect::StringToStatusEffectType((char*)TCOD_list_get(value.list,i));
				if (StatusEffect::IsApplyableStatusEffect(type))
					Item::Presets[itemIndex].addsEffects.push_back(std::pair<StatusEffectType, int>(type, 100));
			}
		} else if (boost::iequals(name,"addEffectChances")) {
			for (int i = 0; i < TCOD_list_size(value.list); ++i) {
				Item::Presets[itemIndex].addsEffects.at(i).second = (intptr_t)TCOD_list_get(value.list,i);
			}
		} else if (boost::iequals(name,"removeStatusEffects")) {
			for (int i = 0; i < TCOD_list_size(value.list); ++i) {
				StatusEffectType type = StatusEffect::StringToStatusEffectType((char*)TCOD_list_get(value.list,i));
				if (StatusEffect::IsApplyableStatusEffect(type))
					Item::Presets[itemIndex].removesEffects.push_back(std::pair<StatusEffectType, int>(type, 100));
			}
		} else if (boost::iequals(name,"removeEffectChances")) {
			for (int i = 0; i < TCOD_list_size(value.list); ++i) {
				Item::Presets[itemIndex].removesEffects.at(i).second = (intptr_t)TCOD_list_get(value.list,i);
			}
		}
		return true;
	}

	bool parserEndStruct(TCODParser *parser,const TCODParserStruct *str,const char *name) override {
		if (boost::iequals(str->getName(), "category_type")) {
			if (presetCategoryParent[categoryIndex] == "")
				Item::ParentCategories.push_back(Item::Categories[categoryIndex]);
		}
		return true;
	}
	void error(const char *msg) {
		throw std::runtime_error(msg);
	}
};

void Item::LoadPresets(std::string filename) {
	TCODParser parser = TCODParser();
	TCODParserStruct* categoryTypeStruct = parser.newStructure("category_type");
	categoryTypeStruct->addProperty("parent", TCOD_TYPE_STRING, false);
	categoryTypeStruct->addFlag("flammable");

	TCODParserStruct* itemTypeStruct = parser.newStructure("item_type");
	itemTypeStruct->addListProperty("category", TCOD_TYPE_STRING, true);
	itemTypeStruct->addProperty("graphic", TCOD_TYPE_INT, true);
	itemTypeStruct->addProperty("color", TCOD_TYPE_COLOR, true);
	itemTypeStruct->addListProperty("components", TCOD_TYPE_STRING, false);
	itemTypeStruct->addProperty("containIn", TCOD_TYPE_STRING, false);
	itemTypeStruct->addProperty("nutrition", TCOD_TYPE_FLOAT, false);
	itemTypeStruct->addProperty("growth", TCOD_TYPE_STRING, false);
	itemTypeStruct->addListProperty("fruits", TCOD_TYPE_STRING, false);
	itemTypeStruct->addProperty("multiplier", TCOD_TYPE_INT, false);
	itemTypeStruct->addProperty("containerSize", TCOD_TYPE_INT, false);
	itemTypeStruct->addProperty("fitsin", TCOD_TYPE_STRING, false);
	itemTypeStruct->addListProperty("decay", TCOD_TYPE_STRING, false);
	itemTypeStruct->addProperty("decaySpeed", TCOD_TYPE_INT, false);
	itemTypeStruct->addProperty("constructedin", TCOD_TYPE_STRING, false);
	itemTypeStruct->addProperty("bulk", TCOD_TYPE_INT, false);
	itemTypeStruct->addProperty("durability", TCOD_TYPE_INT, false);
	itemTypeStruct->addProperty("fallbackGraphicsSet", TCOD_TYPE_STRING, false);
	itemTypeStruct->addListProperty("addStatusEffects", TCOD_TYPE_STRING, false);
	itemTypeStruct->addListProperty("addEffectChances", TCOD_TYPE_INT, false);
	itemTypeStruct->addListProperty("removeStatusEffects", TCOD_TYPE_STRING, false);
	itemTypeStruct->addListProperty("removeEffectChances", TCOD_TYPE_INT, false);

	TCODParserStruct *attackTypeStruct = parser.newStructure("attack");
	const char* damageTypes[] = { "slashing", "piercing", "blunt", "magic", "fire", "cold", "poison", "wielded", "ranged", NULL };
	attackTypeStruct->addValueList("type", damageTypes, true);
	attackTypeStruct->addProperty("damage", TCOD_TYPE_DICE, false);
	attackTypeStruct->addProperty("cooldown", TCOD_TYPE_INT, false);
	attackTypeStruct->addListProperty("statusEffects", TCOD_TYPE_STRING, false);
	attackTypeStruct->addListProperty("effectChances", TCOD_TYPE_INT, false);
	attackTypeStruct->addProperty("ammo", TCOD_TYPE_STRING, false);

	itemTypeStruct->addStructure(attackTypeStruct);

	TCODParserStruct *resistancesStruct = parser.newStructure("resistances");
	resistancesStruct->addProperty("physical", TCOD_TYPE_INT, false);
	resistancesStruct->addProperty("magic", TCOD_TYPE_INT, false);
	resistancesStruct->addProperty("cold", TCOD_TYPE_INT, false);
	resistancesStruct->addProperty("fire", TCOD_TYPE_INT, false);
	resistancesStruct->addProperty("poison", TCOD_TYPE_INT, false);
	resistancesStruct->addProperty("bleeding", TCOD_TYPE_INT, false);
	itemTypeStruct->addStructure(resistancesStruct);

	ItemListener itemListener = ItemListener();
	parser.run(filename.c_str(), &itemListener);
	itemListener.translateNames();
	UpdateEffectItems();
}

void Item::ResolveContainers() {
	for (ItemPreset & preset : Item::Presets) {
			if (!preset.fitsInRaw.empty()) {
			preset.fitsin = Item::StringToItemCategory(preset.fitsInRaw);
		}

		if (!preset.containInRaw.empty()) {
			preset.containIn = Item::StringToItemCategory(preset.containInRaw);
			preset.components.push_back(preset.containIn);
		}

		preset.fitsInRaw.clear();
		preset.containInRaw.clear();
	}
}

void Item::SetFaction(int val) {
	if (val == PLAYERFACTION && faction != PLAYERFACTION) { //Transferred to player
		StockManager::Inst()->UpdateQuantity(type, 1);
	} else if (val != PLAYERFACTION && faction == PLAYERFACTION) { //Transferred from player
		StockManager::Inst()->UpdateQuantity(type, -1);
	}
	faction = val;
}

int Item::GetFaction() const { return faction; }

int Item::RelativeValue() {
	TCOD_dice_t amount = attack.Amount();
	int minDamage = (int)(amount.nb_dices + amount.addsub);
	int maxDamage = (int)((amount.nb_dices * amount.nb_faces) + amount.addsub);
	return (minDamage + maxDamage) / 2;
}

int Item::Resistance(int i) const { return resistances[i]; }

void Item::SetVelocity(int speed) {
	velocity = speed;
	if (speed > 0)
	{
		Game::Inst()->flyingItems.insert(std::static_pointer_cast<Item>(shared_from_this()));
	} else
	{
		//The item has moved before but has now stopped
		Game::Inst()->stoppedItems.push_back(std::static_pointer_cast<Item>(shared_from_this()));
		if (!map->IsWalkable(pos))
		{
			for (int radius = 1; radius < 10; ++radius)
			{
				//TODO consider using something more believable here; the item would jump over 9 walls?
				for (int ix = pos.getX() - radius; ix <= pos.getX() + radius; ++ix)
				{
					for (int iy = pos.getY() - radius; iy <= pos.getY() + radius; ++iy)
					{
						Coordinate p(ix, iy);
						if (map->IsWalkable(p))
						{
							Position(p);
							return;
						}
					}
				}
			}
		}
	}
}

void Item::UpdateVelocity() {
	if (velocity > 0) {
		nextVelocityMove += velocity;
		while (nextVelocityMove > 100) {
			nextVelocityMove -= 100;
			if (flightPath.size() > 0) {

				if (flightPath.back().height < ENTITYHEIGHT) { //We're flying low enough to hit things
					Coordinate t = flightPath.back().coord;

					if (map->BlocksWater(t) || !map->IsWalkable(t)) { //We've hit an obstacle
						Attack attack = GetAttack();
						if (map->GetConstruction(t) > -1)
						{
							if (std::shared_ptr<Construction> construct = Game::Inst()->GetConstruction(
									map->GetConstruction(t)).lock())
							{
								construct->Damage(&attack);
							}
						}
						Impact(velocity);
						return;
					}
					if (map->NPCList(t)->size() > 0) { //Hit a creature
						if (Random::Generate(std::max(1, flightPath.back().height) - 1) < (signed int)(2 + map->NPCList(t)->size()))
						{
							Attack attack = GetAttack();
							std::shared_ptr<NPC> npc = Game::Inst()->GetNPC(*map->NPCList(t)->begin());
							npc->Damage(&attack);

							Position(flightPath.back().coord);
							Impact(velocity);
							return;
						}
					}
				}

				Position(flightPath.back().coord);

				if (flightPath.back().height <= 0) { //Hit the ground early
					Impact(velocity);
					return;
				}

				flightPath.pop_back();
			} else { Impact(velocity); return; } //No more flightpath
		}
	} 
}

void Item::SetInternal() { internal = true; }

int Item::GetDecay() const { return decayCounter; }

void Item::Impact(int speedChange) {
	SetVelocity(0);
	flightPath.clear();

	if (speedChange >= 10 && Random::Generate(9) < 7) DecreaseCondition(); //A sudden impact will damage the item
	if (condition == 0)
	{ //Note that condition < 0 means that it is not damaged by impacts
		//The item has impacted and broken. Create debris owned by no one
		std::vector<std::shared_ptr<Item> > component(1, std::static_pointer_cast<Item>(shared_from_this()));
		Game::Inst()->CreateItem(Position(), Item::StringToItemType("debris"), false, -1, component);
		//Game::Update removes all condition==0 items in the stopped items list, which is where this item will be
	}
}

bool Item::IsFlammable() { return flammable; }

void Item::UpdateEffectItems() {
	int index = -1;
	for (ItemPreset & Preset : Presets)
	{
		++index;
		for (auto & removesEffect : Preset.removesEffects)
		{
			EffectRemovers.insert(std::make_pair(removesEffect.first, (ItemType)index));
		}
		for (auto & addsEffect : Preset.addsEffects)
		{
			StatusEffect effect(addsEffect.first);
			if (!effect.negative)
			{
				GoodEffectAdders.insert(std::make_pair(addsEffect.first, (ItemType)index));
			}
		}
	}
}

int Item::DecreaseCondition() {
	if (condition > 0) --condition;
	return condition;
}

void Item::save(OutputArchive& ar, const unsigned int version) const {
	ar & boost::serialization::base_object<Entity>(*this);
	ar & graphic;
	std::string itemType(Item::ItemTypeToString(type));
	ar & itemType;
	ar & color.r;
	ar & color.g;
	ar & color.b;
	int categoryCount = (int)categories.size();
	ar & categoryCount;
	for (int categorie : categories) {
		std::string itemCat(Item::ItemCategoryToString(categorie));
		ar & itemCat;
	}
	ar & flammable;
	ar & attemptedStore;
	ar & decayCounter;
	ar & attack;
	ar & resistances;
	ar & condition;
	ar & container;
	ar & internal;
}

void Item::load(InputArchive& ar, const unsigned int version) {
	ar & boost::serialization::base_object<Entity>(*this);
	ar & graphic;
	bool failedToFindType = false;
	std::string typeName;
	ar & typeName;
	type = Item::StringToItemType(typeName);
	if (type == -1) {
		type = Item::StringToItemType("debris");
		failedToFindType = true;
	}
	ar & color.r;
	ar & color.g;
	ar & color.b;
	int categoryCount = 0;
	ar & categoryCount;
	categories.clear();
	for (int i = 0; i < categoryCount; ++i) {
		std::string categoryName;
		ar & categoryName;
		int categoryType = Item::StringToItemCategory(categoryName);
		if (categoryType >= 0 && categoryType < static_cast<int>(Item::Categories.size()))
			categories.insert(categoryType);
	}
	if (categories.empty())
		categories.insert(Item::StringToItemCategory("garbage"));
	ar & flammable;
	if (failedToFindType)
		flammable = true; //Just so you can get rid of it
	ar & attemptedStore;
	ar & decayCounter;
	ar & attack;
	ar & resistances;
	ar & condition;
	ar & container;
	ar & internal;
}

ItemCat::ItemCat() : flammable(false),
	name("Category schmategory"),
	parent(-1)
{}

std::string ItemCat::GetName() {
	return name;
}

ItemPreset::ItemPreset() : graphic('?'),
	color(TCODColor::pink),
	name("Preset default"),
	categories(std::set<ItemCategory>()),
	nutrition(-1),
	growth(-1),
	fruits(std::list<ItemType>()),
	organic(false),
	container(0),
	multiplier(1),
	fitsin(-1),
	containIn(-1),
	decays(false),
	decaySpeed(0),
	decayList(std::vector<ItemType>()),
	attack(Attack()),
	bulk(1),
	condition(1),
	fallbackGraphicsSet(),
	graphicsHint(-1)
{
	for (int & resistance : resistances) {
		resistance = 0;
	}
}

OrganicItem::OrganicItem(Coordinate pos, ItemType typeVal) : Item(pos, typeVal),
	nutrition(-1),
	growth(-1)
{}

int OrganicItem::Nutrition() { return nutrition; }
void OrganicItem::Nutrition(int val) { nutrition = val; }

ItemType OrganicItem::Growth() { return growth; }
void OrganicItem::Growth(ItemType val) { growth = val; }

void OrganicItem::save(OutputArchive& ar, const unsigned int version) const {
	ar & boost::serialization::base_object<Item>(*this);
	ar & nutrition;
	ar & growth;
}

void OrganicItem::load(InputArchive& ar, const unsigned int version) {
	ar & boost::serialization::base_object<Item>(*this);
	ar & nutrition;
	ar & growth;
}

WaterItem::WaterItem(Coordinate pos, ItemType typeVal) : OrganicItem(pos, typeVal) {}

void WaterItem::PutInContainer(std::shared_ptr<Item> con)
{
	container = con;
	attemptedStore = false;

	Game::Inst()->ItemContained(std::static_pointer_cast<Item>(shared_from_this()), !!container);

	if (!container && !reserved)
	{
		//WaterItems transform into an actual waternode if not contained
		Game::Inst()->CreateWater(Position(), 1);
		Game::Inst()->RemoveItem(std::static_pointer_cast<Item>(shared_from_this()));
	}
}

void WaterItem::save(OutputArchive& ar, const unsigned int version) const {
	ar & boost::serialization::base_object<OrganicItem>(*this);
}

void WaterItem::load(InputArchive& ar, const unsigned int version) {
	ar & boost::serialization::base_object<OrganicItem>(*this);
}
