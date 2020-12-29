CMAKE_MINIMUM_REQUIRED(VERSION 3.1) # Do not raise it without a reason. Bump only when you use some feature that is not supported by older verions

# set the project name
PROJECT(GoblinCamp LANGUAGES C CXX)

SET(CMAKE_CXX_FLAGS "-std=c++11 -DBOOST_NO_CXX11_SCOPED_ENUMS -DBOOST_NO_SCOPED_ENUMS")  # FIXME move to add_compile_options() later

#  -DBOOST_NO_CXX11_SCOPED_ENUMS -DBOOST_NO_SCOPED_ENUMS  https://stackoverflow.com/questions/18783790/boostfilesystemcopy-file-missing-symbol-in-c11 
# and Goblin Camp/Source/data/Data.cpp fs::copy_file(source, target);

# add the executable
# add_executable(Tutorial tutorial.cxx)

# Used for export the headers and set the directory share/games/
SET(GOBLIN_CAMP_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

SET(GOBLIN_CAMP_BINDIR ${CMAKE_INSTALL_PREFIX}/games CACHE PATH "Path to Goblin Camp binary")
SET(GOBLIN_CAMP_DATA_BASE_DIR "${CMAKE_INSTALL_PREFIX}/share" CACHE PATH "Base Data Dir Path")
SET(GOBLIN_CAMP_SHAREDIR "${GOBLIN_CAMP_DATA_BASE_DIR}/games/goblin-camp" CACHE PATH "Path to Goblin Camp shared files")

ADD_SUBDIRECTORY("Goblin Camp")

FIND_PACKAGE(Boost REQUIRED COMPONENTS thread system filesystem serialization iostreams)

IF (${Boost_VERSION} GREATER 106699) # boost >= 1.67
    FIND_PACKAGE(Boost COMPONENTS python27 REQUIRED)
    SET(Boost_PYTHON_LIBRARY ${Boost_PYTHON27_LIBRARY})
ELSE ()
    # boost python suffixes are determined by distributives
    # try different alternatives
    FIND_PACKAGE(Boost OPTIONAL_COMPONENTS python27)
    IF (Boost_PYTHON27_FOUND)
        SET(Boost_PYTHON_LIBRARY ${Boost_PYTHON27_LIBRARY})
    ELSE ()
        FIND_PACKAGE(Boost OPTIONAL_COMPONENTS python-py27)
        IF (Boost_PYTHON-PY27_FOUND)
            SET(Boost_PYTHON_LIBRARY ${Boost_PYTHON-PY27_LIBRARY})
        ELSE ()
            FIND_PACKAGE(Boost COMPONENTS python REQUIRED)
        ENDIF ()
    ENDIF ()
ENDIF ()

FIND_PACKAGE(SDL REQUIRED)
FIND_PACKAGE(SDL_image REQUIRED)
FIND_PACKAGE(PythonLibs 2.7)
FIND_PACKAGE(ZLIB)
FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(PNG REQUIRED)

INCLUDE_DIRECTORIES(${SDL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${PNG_INCLUDE_DIR})

# Support to Standard 17
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

ADD_SUBDIRECTORY(vendor/)
ADD_SUBDIRECTORY(Source/)

ADD_LIBRARY(GoblinCamp.Data
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


SET(platform_SRC
        Source/platform/linux/DataImpl.cpp
        )

ADD_EXECUTABLE(goblin-camp Source/platform/unix/main.cpp Source/GCamp.cpp ${platform_SRC})

INCLUDE_DIRECTORIES("goblin-camp" Include/)
INCLUDE_DIRECTORIES("goblin-camp" vendor/libtcod/include/)

TARGET_LINK_LIBRARIES(GoblinCamp.Data PUBLIC GoblinCamp::Renderer::Tile)

TARGET_LINK_LIBRARIES(GoblinCamp.Main PUBLIC GoblinCamp::Libtcod)
TARGET_LINK_LIBRARIES(GoblinCamp.Main PUBLIC Goblin::Geometry)

TARGET_LINK_LIBRARIES(goblin-camp PUBLIC GoblinCamp::Renderer::Tile)
TARGET_LINK_LIBRARIES(goblin-camp PUBLIC GoblinCamp::Scripting)
TARGET_LINK_LIBRARIES(goblin-camp PUBLIC GoblinCamp::Libtcod)
TARGET_LINK_LIBRARIES(goblin-camp PUBLIC GoblinCamp::Data)
TARGET_LINK_LIBRARIES(goblin-camp PUBLIC GoblinCamp::Main)
TARGET_LINK_LIBRARIES(goblin-camp PUBLIC GoblinCamp::UI)

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

FILE(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Library/ DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/share/games/goblin-camp/lib/)

# Include the Content Package Manager for C++
INCLUDE(CMakeModules/CPM.cmake)

# Execute the Test in the directory
ADD_SUBDIRECTORY(Test/)