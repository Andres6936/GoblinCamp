/* Copyright 2011 Ilkka Halila
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

#include <memory>
#include <filesystem>

#include <boost/noncopyable.hpp>
#include <libtcod.hpp>

#include "Goblin/Graphics/Tile/TileSetLoader.hpp"
#include "Goblin/Graphics/Tile/TileSet.hpp"
#include "Goblin/Graphics/Tile/TileSetTexture.hpp"
#include "Goblin/Graphics/Tile/SpriteSetFactories.hpp"
#include "Goblin/Graphics/Tile/NPCSprite.hpp"
#include "Goblin/Graphics/Tile/ItemSprite.hpp"
#include "Goblin/Graphics/Tile/ConstructionSprite.hpp"
#include "Goblin/Graphics/Tile/SpellSpriteSet.hpp"
#include "Goblin/Graphics/Tile/StatusEffectSprite.hpp"

class TileSetParserV2 : public ITCODParserListener, private boost::noncopyable
{
public:
	explicit TileSetParserV2(std::shared_ptr<TilesetRenderer> spriteFactory);

	~TileSetParserV2();

	std::shared_ptr<TileSet> Run(std::filesystem::path tileSetPath);

	void Modify(std::shared_ptr<TileSet> tileset, std::filesystem::path modPath);

	bool parserNewStruct(TCODParser* parser, const TCODParserStruct* str, const char* name);

	bool parserFlag(TCODParser* parser, const char* name);

	bool parserProperty(TCODParser* pakrser, const char* name, TCOD_value_type_t type, TCOD_value_t value);

	bool parserEndStruct(TCODParser* parser, const TCODParserStruct* str, const char* name);

	void error(const char* msg);

private:
	TCODParser parser;
	std::shared_ptr<TilesetRenderer> spriteFactory;

	std::shared_ptr<TileSet> tileSet;
	bool success;
	bool readTexture;
	bool extendingExisting;

	enum ParserState
	{
		PS_NORMAL,
		PS_TERRAIN,
		PS_NPC,
		PS_ITEM,
		PS_NATURE,
		PS_CONSTRUCTION,
		PS_SPELL,
		PS_STATUS_EFFECT
	};
	TileSetParserV2::ParserState currentParsingState;

	// Path where textures are found
	std::filesystem::path tileSetPath;
	std::shared_ptr<TileSetTexture> currentTexture;

	std::string tileSetName;
	int tileWidth;
	int tileHeight;

	std::vector<int> markerFrames;
	int markerFPS;

	std::vector<int> fireFrames;
	int fireFPS;

	struct AnimatedSpriteFactory
	{
		std::vector<int> sprites;
		int fps;

		AnimatedSpriteFactory() : sprites(), fps(15)
		{
		}

		Sprite_ptr
		Build(std::shared_ptr<TilesetRenderer> spriteFactory, std::shared_ptr<TileSetTexture> currentTexture);
	};
	AnimatedSpriteFactory animSpriteFactory;
	ConstructionSpriteFactory constructionFactory;
	StatusEffectSpriteFactory statusEffectFactory;
	NPCSpriteFactory npcSpriteFactory;
	TerrainSpriteFactory terrainSpriteFactory;
		
	ItemSprite itemSprite;
	Sprite_ptr corruptionOverride;
	Sprite_ptr corruptionOverlayOverride;
	
	static const char * uninitialisedTilesetError;
	
	void SetCursorSprites(CursorType type, TCOD_list_t cursors);
	TileType StringToTileType(std::string typeString) const;
	void ApplyCorruptionOverrides();
};

class TileSetMetadataParserV2 : public ITCODParserListener
{
public:
	TileSetMetadataParserV2();

	TileSetMetadata Run(std::filesystem::path path);

	bool parserNewStruct(TCODParser* parser, const TCODParserStruct* str, const char* name);

	bool parserFlag(TCODParser* parser, const char* name);

	bool parserProperty(TCODParser* parser, const char* name, TCOD_value_type_t type, TCOD_value_t value);

	bool parserEndStruct(TCODParser* parser, const TCODParserStruct* str, const char* name);

	void error(const char* msg);

private:
	TCODParser parser;
	TileSetMetadata metadata;

};

class TileSetModMetadataParserV2 : public ITCODParserListener
{
public:
	TileSetModMetadataParserV2();

	std::list<TilesetModMetadata> Run(std::filesystem::path path);

	bool parserNewStruct(TCODParser* parser, const TCODParserStruct* str, const char* name);

	bool parserFlag(TCODParser* parser, const char* name);

	bool parserProperty(TCODParser* parser, const char* name, TCOD_value_type_t type, TCOD_value_t value);

	bool parserEndStruct(TCODParser* parser, const TCODParserStruct* str, const char* name);

	void error(const char* msg);

private:
	TCODParser parser;
	std::list<TilesetModMetadata> metadata;
	std::filesystem::path location;

};