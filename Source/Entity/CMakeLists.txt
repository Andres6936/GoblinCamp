ADD_LIBRARY(Goblin.Entity STATIC
        Container.cpp
        Entity.cpp
        Item.cpp
        NPC.cpp
        Tile.cpp
        )
TARGET_INCLUDE_DIRECTORIES(Goblin.Entity PUBLIC $<BUILD_INTERFACE:${GOBLIN_CAMP_ROOT_DIR}/Include/>)