ADD_LIBRARY(Goblin.Entity STATIC
        Container.cpp
        Entity.cpp
        Item.cpp
        NPC.cpp
        Tile.cpp
        )
TARGET_LINK_LIBRARIES(Goblin.Entity PUBLIC Goblin::Libtcod)
SET_TARGET_PROPERTIES(Goblin.Entity PROPERTIES CXX_STANDARD 17)
TARGET_INCLUDE_DIRECTORIES(Goblin.Entity PUBLIC $<BUILD_INTERFACE:${GOBLIN_CAMP_ROOT_DIR}/Include/>)
