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

ADD_SUBDIRECTORY(vendor/)
ADD_SUBDIRECTORY(Source/)

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



SET (platform_SRC
Source/platform/linux/DataImpl.cpp
)

add_executable(goblin-camp Source/platform/unix/main.cpp Source/GCamp.cpp ${platform_SRC})

include_directories("goblin-camp" Include/)
include_directories("goblin-camp" vendor/libtcod/include/)

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
