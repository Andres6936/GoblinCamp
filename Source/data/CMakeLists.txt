ADD_LIBRARY (GoblinCamp.Data
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