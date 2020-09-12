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

    sdl/SDLSprite.cpp
    sdl/SDLTilesetRenderer.cpp

    ogl/OGLFunctionExt.cpp
    ogl/OGLResources.cpp
    ogl/OGLSprite.cpp
    ogl/OGLTilesetRenderer.cpp
    ogl/OGLViewportLayer.cpp
)

ADD_LIBRARY(GoblinCamp::Renderer::Tile ALIAS GoblinCamp.Renderer.Tile)