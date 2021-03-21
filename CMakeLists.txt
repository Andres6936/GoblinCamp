CMAKE_MINIMUM_REQUIRED(VERSION 3.14) # Do not raise it without a reason. Bump only when you use some feature that is not supported by older verions

# set the project name
PROJECT(Goblin LANGUAGES C CXX)

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
FIND_PACKAGE(SDL REQUIRED)
FIND_PACKAGE(SDL_image REQUIRED)
FIND_PACKAGE(PythonLibs 2.7)
FIND_PACKAGE(ZLIB)
FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(PNG REQUIRED)

INCLUDE_DIRECTORIES(${SDL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${PNG_INCLUDE_DIR})
INCLUDE_DIRECTORIES(vendor/libtcod/include/)

# Include the Content Package Manager for C++
INCLUDE(CMakeModules/CPM.cmake)

# Added the Doryen Library (Framework)
CPMADDPACKAGE("gh:Andres6936/Doryen@2021.0131")

# Define the target: GoblinCamp::Libtcod
ADD_SUBDIRECTORY(vendor/)
ADD_SUBDIRECTORY(Source/)

# Execute the Test in the directory
ADD_SUBDIRECTORY(Test/)