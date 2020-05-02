cmake_minimum_required(VERSION 2.8.9) # Do not raise it without a reason. Bump only when you use some feature that is not supported by older verions

# set the project name
project(GoblinCamp C CXX)

SET(CMAKE_CXX_FLAGS "-std=c++11 -DBOOST_NO_CXX11_SCOPED_ENUMS -DBOOST_NO_SCOPED_ENUMS")  # FIXME move to add_compile_options() later

#  -DBOOST_NO_CXX11_SCOPED_ENUMS -DBOOST_NO_SCOPED_ENUMS  https://stackoverflow.com/questions/18783790/boostfilesystemcopy-file-missing-symbol-in-c11 
# and Goblin Camp/src/data/Data.cpp fs::copy_file(source, target);

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



SET (data_SRC
"Goblin Camp/src/data/Config.cpp"
"Goblin Camp/src/data/Data.cpp"
"Goblin Camp/src/data/Mods.cpp"
"Goblin Camp/src/data/Paths.cpp"
"Goblin Camp/src/data/Serialization.cpp"
"Goblin Camp/src/data/Tilesets.cpp"
)
SET (main_SRC
"Goblin Camp/src/Announce.cpp"
"Goblin Camp/src/Attack.cpp"
"Goblin Camp/src/Blood.cpp"
"Goblin Camp/src/Camp.cpp"
"Goblin Camp/src/Construction.cpp"
"Goblin Camp/src/Container.cpp"
"Goblin Camp/src/Coordinate.cpp"
"Goblin Camp/src/Door.cpp"
"Goblin Camp/src/Entity.cpp"
"Goblin Camp/src/Events.cpp"
"Goblin Camp/src/Faction.cpp"
"Goblin Camp/src/Farmplot.cpp"
"Goblin Camp/src/Filth.cpp"
"Goblin Camp/src/Fire.cpp"
"Goblin Camp/src/GCamp.cpp"
"Goblin Camp/src/Game.cpp"
"Goblin Camp/src/Item.cpp"
"Goblin Camp/src/Job.cpp"
"Goblin Camp/src/JobManager.cpp"
"Goblin Camp/src/KuhnMunkres.cpp"
"Goblin Camp/src/Logger.cpp"
"Goblin Camp/src/Map.cpp"
"Goblin Camp/src/MapMarker.cpp"
"Goblin Camp/src/MapRenderer.cpp"
"Goblin Camp/src/NPC.cpp"
"Goblin Camp/src/NatureObject.cpp"
"Goblin Camp/src/Random.cpp"
"Goblin Camp/src/SpawningPool.cpp"
"Goblin Camp/src/Spell.cpp"
"Goblin Camp/src/Squad.cpp"
"Goblin Camp/src/Stats.cpp"
"Goblin Camp/src/StatusEffect.cpp"
"Goblin Camp/src/StockManager.cpp"
"Goblin Camp/src/Stockpile.cpp"
"Goblin Camp/src/TCODMapRenderer.cpp"
"Goblin Camp/src/Tile.cpp"
"Goblin Camp/src/Trap.cpp"
"Goblin Camp/src/UI.cpp"
"Goblin Camp/src/Water.cpp"
"Goblin Camp/src/Weather.cpp"
"Goblin Camp/src/Version.cpp"
)

SET (scripting_SRC
"Goblin Camp/src/scripting/API.cpp"
"Goblin Camp/src/scripting/Engine.cpp"
"Goblin Camp/src/scripting/Event.cpp"
"Goblin Camp/src/scripting/_gcampapi/Functions.cpp"
"Goblin Camp/src/scripting/_gcampapi/LoggerStream.cpp"
"Goblin Camp/src/scripting/_gcampapi/PyConstruction.cpp"
"Goblin Camp/src/scripting/_gcampapi/PyItem.cpp"
)

SET (libtcod_SRC
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

SET (UI_SRC
"Goblin Camp/src/UI/AnnounceDialog.cpp"
"Goblin Camp/src/UI/ConstructionDialog.cpp"
"Goblin Camp/src/UI/DevConsole.cpp"
"Goblin Camp/src/UI/JobDialog.cpp"
"Goblin Camp/src/UI/Menu.cpp"
"Goblin Camp/src/UI/MessageBox.cpp"
"Goblin Camp/src/UI/NPCDialog.cpp"
"Goblin Camp/src/UI/SideBar.cpp"
"Goblin Camp/src/UI/SquadsDialog.cpp"
"Goblin Camp/src/UI/StockManagerDialog.cpp"
"Goblin Camp/src/UI/Tooltip.cpp"
"Goblin Camp/src/UI/UIComponents.cpp"
)

SET (tileRenderer_SRC
"Goblin Camp/src/tileRenderer/ConstructionSprite.cpp"
"Goblin Camp/src/tileRenderer/DrawConstructionVisitor.cpp"
"Goblin Camp/src/tileRenderer/ItemSprite.cpp"
"Goblin Camp/src/tileRenderer/NatureObjectSpriteSet.cpp"
"Goblin Camp/src/tileRenderer/NPCSprite.cpp"
"Goblin Camp/src/tileRenderer/PermutationTable.cpp"
"Goblin Camp/src/tileRenderer/SpellSpriteSet.cpp"
"Goblin Camp/src/tileRenderer/Sprite.cpp"
"Goblin Camp/src/tileRenderer/SpriteSetFactories.cpp"
"Goblin Camp/src/tileRenderer/StatusEffectSprite.cpp"
"Goblin Camp/src/tileRenderer/TerrainSprite.cpp"
"Goblin Camp/src/tileRenderer/TileSet.cpp"
"Goblin Camp/src/tileRenderer/TileSetLoader.cpp"
"Goblin Camp/src/tileRenderer/TilesetParserV1.cpp"
"Goblin Camp/src/tileRenderer/TilesetParserV2.cpp"
"Goblin Camp/src/tileRenderer/TileSetRenderer.cpp"
"Goblin Camp/src/tileRenderer/TileSetTexture.cpp"

"Goblin Camp/src/tileRenderer/sdl/SDLSprite.cpp"
"Goblin Camp/src/tileRenderer/sdl/SDLTilesetRenderer.cpp"

"Goblin Camp/src/tileRenderer/ogl/OGLFunctionExt.cpp"
"Goblin Camp/src/tileRenderer/ogl/OGLResources.cpp"
"Goblin Camp/src/tileRenderer/ogl/OGLSprite.cpp"
"Goblin Camp/src/tileRenderer/ogl/OGLTilesetRenderer.cpp"
"Goblin Camp/src/tileRenderer/ogl/OGLViewportLayer.cpp"

)

SET (platform_SRC
"Goblin Camp/src/platform/linux/DataImpl.cpp"
)

add_executable("goblin-camp" "Goblin Camp/src/platform/unix/main.cpp" "Goblin Camp/src/GCamp.cpp" ${main_SRC} ${data_SRC} ${scripting_SRC} ${libtcod_SRC} ${UI_SRC} ${tileRenderer_SRC} ${platform_SRC})
include_directories("goblin-camp" "Goblin Camp/include")
include_directories("goblin-camp" vendor/libtcod/include)

target_link_libraries("goblin-camp"
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
