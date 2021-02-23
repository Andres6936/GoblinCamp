ADD_LIBRARY(Goblin.Configuration STATIC WindowConfig.cpp)
ADD_LIBRARY(Goblin::Configuration ALIAS Goblin.Configuration)
TARGET_INCLUDE_DIRECTORIES(Goblin.Configuration PUBLIC $<BUILD_INTERFACE:${GOBLIN_CAMP_ROOT_DIR}/Include/>)

ADD_LIBRARY(GoblinCamp.Data
        Config.cpp
        Data.cpp
        Mods.cpp
        Paths.cpp
        Serialization.cpp
        Tilesets.cpp
        )

ADD_LIBRARY(GoblinCamp::Data ALIAS GoblinCamp.Data)
TARGET_INCLUDE_DIRECTORIES(GoblinCamp.Data
        PUBLIC
        $<BUILD_INTERFACE:${GOBLIN_CAMP_ROOT_DIR}/Include/>
        $<INSTALL_INTERFACE:${GOBLIN_CAMP_ROOT_DIR}/Include/>
        )