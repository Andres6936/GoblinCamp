ADD_LIBRARY(Goblin.Configuration STATIC
        Config.cpp
        Data.cpp
        Mods.cpp
        Paths.cpp
        Serialization.cpp
        Tilesets.cpp
        WindowConfig.cpp)
ADD_LIBRARY(Goblin::Configuration ALIAS Goblin.Configuration)
SET_TARGET_PROPERTIES(Goblin.Configuration PROPERTIES CXX_STANDARD 17)
TARGET_INCLUDE_DIRECTORIES(Goblin.Configuration PUBLIC $<BUILD_INTERFACE:${GOBLIN_CAMP_ROOT_DIR}/Include/>)