cmake_minimum_required(VERSION 3.1) # Do not raise it without a reason. Bump only when you use some feature that is not supported by older verions

# set the project name
project(GoblinCamp LANGUAGES C CXX)

SET(CMAKE_CXX_FLAGS "-std=c++11 -DBOOST_NO_CXX11_SCOPED_ENUMS -DBOOST_NO_SCOPED_ENUMS")  # FIXME move to add_compile_options() later

#  -DBOOST_NO_CXX11_SCOPED_ENUMS -DBOOST_NO_SCOPED_ENUMS  https://stackoverflow.com/questions/18783790/boostfilesystemcopy-file-missing-symbol-in-c11 
# and Goblin Camp/Source/data/Data.cpp fs::copy_file(source, target);

# add the executable
# add_executable(Tutorial tutorial.cxx)

SET(GOBLIN_CAMP_BINDIR ${CMAKE_INSTALL_PREFIX}/games CACHE PATH "Path to Goblin Camp binary")
SET(GOBLIN_CAMP_DATA_BASE_DIR "${CMAKE_INSTALL_PREFIX}/share" CACHE PATH "Base Data Dir Path")
SET(GOBLIN_CAMP_SHAREDIR "${GOBLIN_CAMP_DATA_BASE_DIR}/games/goblin-camp" CACHE PATH "Path to Goblin Camp shared files")

ADD_SUBDIRECTORY("Goblin Camp")

find_package(Boost REQUIRED COMPONENTS thread system filesystem serialization iostreams)

if(${Boost_VERSION} GREATER 106699) # boost >= 1.67
    find_package(Boost COMPONENTS python27 REQUIRED)
    set(Boost_PYTHON_LIBRARY ${Boost_PYTHON27_LIBRARY})
else()
    # boost python suffixes are determined by distributives
    # try different alternatives
    find_package(Boost OPTIONAL_COMPONENTS python27)
    if(Boost_PYTHON27_FOUND)
        set(Boost_PYTHON_LIBRARY ${Boost_PYTHON27_LIBRARY})
    else()
        find_package(Boost OPTIONAL_COMPONENTS python-py27)
        if(Boost_PYTHON-PY27_FOUND)
            set(Boost_PYTHON_LIBRARY ${Boost_PYTHON-PY27_LIBRARY})
        else()
            find_package(Boost COMPONENTS python REQUIRED)
        endif()
    endif()
endif()

find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)
find_package(PythonLibs 2.7)
find_package(ZLIB)
find_package(OpenGL REQUIRED)
find_package(PNG REQUIRED)

include_directories(${SDL_INCLUDE_DIR})
include_directories(${PYTHON_INCLUDE_DIRS})
include_directories(${PNG_INCLUDE_DIR})



ADD_LIBRARY (GoblinCamp.Data
    Source/data/Config.cpp
    Source/data/Data.cpp
    Source/data/Mods.cpp
    Source/data/Paths.cpp
    Source/data/Serialization.cpp
    Source/data/Tilesets.cpp
)

ADD_LIBRARY(GoblinCamp::Data ALIAS GoblinCamp.Data)

ADD_LIBRARY(GoblinCamp.Main
        Source/Announce.cpp
        Source/Attack.cpp
        Source/Blood.cpp
        Source/Camp.cpp
        Source/Construction.cpp
        Source/Container.cpp
        Source/Coordinate.cpp
        Source/Door.cpp
        Source/Entity.cpp
        Source/Events.cpp
        Source/Faction.cpp
        Source/Farmplot.cpp
        Source/Filth.cpp
        Source/Fire.cpp
        Source/GCamp.cpp
        Source/Game.cpp
        Source/Item.cpp
        Source/Job.cpp
        Source/JobManager.cpp
        Source/KuhnMunkres.cpp
        Source/Logger.cpp
        Source/Map.cpp
        Source/MapMarker.cpp
        Source/MapRenderer.cpp
        Source/NPC.cpp
        Source/NatureObject.cpp
        Source/Random.cpp
        Source/SpawningPool.cpp
        Source/Spell.cpp
        Source/Squad.cpp
        Source/Stats.cpp
        Source/StatusEffect.cpp
        Source/StockManager.cpp
        Source/Stockpile.cpp
        Source/TCODMapRenderer.cpp
        Source/Tile.cpp
        Source/Trap.cpp
        Source/UI.cpp
        Source/Water.cpp
        Source/Weather.cpp
        Source/Version.cpp
        )

ADD_LIBRARY(GoblinCamp::Main ALIAS GoblinCamp.Main)

ADD_LIBRARY(GoblinCamp.Scripting
        Source/scripting/API.cpp
        Source/scripting/Engine.cpp
        Source/scripting/Event.cpp
        Source/scripting/_gcampapi/Functions.cpp
        Source/scripting/_gcampapi/LoggerStream.cpp
        Source/scripting/_gcampapi/PyConstruction.cpp
        Source/scripting/_gcampapi/PyItem.cpp
)

ADD_LIBRARY(GoblinCamp::Scripting ALIAS GoblinCamp.Scripting)

ADD_LIBRARY(GoblinCamp.Libtcod
        vendor/libtcod/src/bresenham_c.c
        vendor/libtcod/src/bresenham.cpp
        vendor/libtcod/src/bsp_c.c
        vendor/libtcod/src/bsp.cpp
        vendor/libtcod/src/color_c.c
        vendor/libtcod/src/color.cpp
        vendor/libtcod/src/console_c.c
        vendor/libtcod/src/console.cpp
        vendor/libtcod/src/fov_c.c
        vendor/libtcod/src/fov_circular_raycasting.c
        vendor/libtcod/src/fov.cpp
        vendor/libtcod/src/fov_diamond_raycasting.c
        vendor/libtcod/src/fov_digital.c
        vendor/libtcod/src/fov_permissive2.c
        vendor/libtcod/src/fov_permissive.c
        vendor/libtcod/src/fov_recursive_shadowcasting.c
        vendor/libtcod/src/fov_restrictive.c
        vendor/libtcod/src/heightmap_c.c
        vendor/libtcod/src/heightmap.cpp
        vendor/libtcod/src/image_c.c
        vendor/libtcod/src/image.cpp
        vendor/libtcod/src/lex_c.c
        vendor/libtcod/src/lex.cpp
        vendor/libtcod/src/list_c.c
        vendor/libtcod/src/mersenne_c.c
        vendor/libtcod/src/mersenne.cpp
        vendor/libtcod/src/mouse.cpp
        vendor/libtcod/src/namegen_c.c
        vendor/libtcod/src/namegen.cpp
        vendor/libtcod/src/noise_c.c
        vendor/libtcod/src/noise.cpp
        vendor/libtcod/src/parser_c.c
        vendor/libtcod/src/parser.cpp
        vendor/libtcod/src/path_c.c
        vendor/libtcod/src/path.cpp
        vendor/libtcod/src/sys_c.c
        vendor/libtcod/src/sys.cpp
        vendor/libtcod/src/sys_opengl_c.c
        vendor/libtcod/src/sys_sdl_c.c
        vendor/libtcod/src/sys_sdl_img_bmp.c
        vendor/libtcod/src/sys_sdl_img_png.c
        vendor/libtcod/src/tree_c.c
        vendor/libtcod/src/txtfield_c.c
        vendor/libtcod/src/txtfield.cpp
        vendor/libtcod/src/wrappers.c
        vendor/libtcod/src/zip_c.c
        vendor/libtcod/src/zip.cpp
        )

ADD_LIBRARY (GoblinCamp::Libtcod ALIAS GoblinCamp.Libtcod)

ADD_LIBRARY (GoblinCamp.UI
    Source/UI/AnnounceDialog.cpp
    Source/UI/ConstructionDialog.cpp
    Source/UI/DevConsole.cpp
    Source/UI/JobDialog.cpp
    Source/UI/Menu.cpp
    Source/UI/MessageBox.cpp
    Source/UI/NPCDialog.cpp
    Source/UI/SideBar.cpp
    Source/UI/SquadsDialog.cpp
    Source/UI/StockManagerDialog.cpp
    Source/UI/Tooltip.cpp
    Source/UI/UIComponents.cpp
)

ADD_LIBRARY(GoblinCamp::UI ALIAS GoblinCamp.UI)

ADD_LIBRARY (GoblinCamp.Renderer.Tile
    Source/tileRenderer/ConstructionSprite.cpp
    Source/tileRenderer/DrawConstructionVisitor.cpp
    Source/tileRenderer/ItemSprite.cpp
    Source/tileRenderer/NatureObjectSpriteSet.cpp
    Source/tileRenderer/NPCSprite.cpp
    Source/tileRenderer/PermutationTable.cpp
    Source/tileRenderer/SpellSpriteSet.cpp
    Source/tileRenderer/Sprite.cpp
    Source/tileRenderer/SpriteSetFactories.cpp
    Source/tileRenderer/StatusEffectSprite.cpp
    Source/tileRenderer/TerrainSprite.cpp
    Source/tileRenderer/TileSet.cpp
    Source/tileRenderer/TileSetLoader.cpp
    Source/tileRenderer/TilesetParserV1.cpp
    Source/tileRenderer/TilesetParserV2.cpp
    Source/tileRenderer/TileSetRenderer.cpp
    Source/tileRenderer/TileSetTexture.cpp

    Source/tileRenderer/sdl/SDLSprite.cpp
    Source/tileRenderer/sdl/SDLTilesetRenderer.cpp

    Source/tileRenderer/ogl/OGLFunctionExt.cpp
    Source/tileRenderer/ogl/OGLResources.cpp
    Source/tileRenderer/ogl/OGLSprite.cpp
    Source/tileRenderer/ogl/OGLTilesetRenderer.cpp
    Source/tileRenderer/ogl/OGLViewportLayer.cpp
)

ADD_LIBRARY(GoblinCamp::Renderer::Tile ALIAS GoblinCamp.Renderer.Tile)

SET (platform_SRC
Source/platform/linux/DataImpl.cpp
)

add_executable(goblin-camp Source/platform/unix/main.cpp Source/GCamp.cpp ${platform_SRC})

include_directories("goblin-camp" "Goblin Camp/include")
include_directories("goblin-camp" vendor/libtcod/include)

TARGET_LINK_LIBRARIES(GoblinCamp.Renderer.Tile PRIVATE ${Boost_FILESYSTEM_LIBRARY})

TARGET_LINK_LIBRARIES(GoblinCamp.Data PRIVATE GoblinCamp::Renderer::Tile)

TARGET_LINK_LIBRARIES(GoblinCamp.Main PRIVATE GoblinCamp::Libtcod)

TARGET_LINK_LIBRARIES(goblin-camp PRIVATE GoblinCamp::Renderer::Tile)
TARGET_LINK_LIBRARIES(goblin-camp PRIVATE GoblinCamp::Scripting)
TARGET_LINK_LIBRARIES(goblin-camp PRIVATE GoblinCamp::Libtcod)
TARGET_LINK_LIBRARIES(goblin-camp PRIVATE GoblinCamp::Data)
TARGET_LINK_LIBRARIES(goblin-camp PRIVATE GoblinCamp::Main)
TARGET_LINK_LIBRARIES(goblin-camp PRIVATE GoblinCamp::UI)

TARGET_LINK_LIBRARIES(goblin-camp PRIVATE
  ${SDL_LIBRARY}
  ${SDLIMAGE_LIBRARY}
  ${PYTHON_LIBRARIES}
  ${Boost_THREAD_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_SERIALIZATION_LIBRARY}
  ${Boost_PYTHON_LIBRARY}
  ${Boost_IOSTREAMS_LIBRARY}
  ${ZLIB_LIBRARIES}
  ${OPENGL_LIBRARIES}
  ${PNG_LIBRARY}
)

INSTALL(TARGETS "goblin-camp" RUNTIME DESTINATION ${GOBLIN_CAMP_BINDIR})
