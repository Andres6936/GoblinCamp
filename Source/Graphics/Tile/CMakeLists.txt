ADD_LIBRARY (GoblinCamp.Renderer.Tile
    ConstructionSprite.cpp
    DrawConstructionVisitor.cpp
    ItemSprite.cpp
    NatureObjectSpriteSet.cpp
    NPCSprite.cpp
    PermutationTable.cpp
    SpellSpriteSet.cpp
    Sprite.cpp
        SpriteSetFactories.cpp
        StatusEffectSprite.cpp
        TerrainSprite.cpp
        TileSet.cpp
        TileSetLoader.cpp
        TilesetParserV1.cpp
        TilesetParserV2.cpp
        TileSetRenderer.cpp
        TileSetTexture.cpp

        SDL/SDLSprite.cpp
        SDL/SDLTilesetRenderer.cpp

        OGL/OGLFunctionExt.cpp
        OGL/OGLResources.cpp
        OGL/OGLSprite.cpp
        OGL/OGLTilesetRenderer.cpp
        OGL/OGLViewportLayer.cpp
        )

ADD_LIBRARY(GoblinCamp::Renderer::Tile ALIAS GoblinCamp.Renderer.Tile)
TARGET_INCLUDE_DIRECTORIES(GoblinCamp.Renderer.Tile
        PUBLIC
        $<BUILD_INTERFACE:${GOBLIN_CAMP_ROOT_DIR}/Include/>
        $<INSTALL_INTERFACE:${GOBLIN_CAMP_ROOT_DIR}/Include/>
        )

TARGET_LINK_LIBRARIES(GoblinCamp.Renderer.Tile PUBLIC GoblinCamp::Libtcod)