ADD_LIBRARY(Goblin.Util STATIC
        Game.cpp
        GCamp.cpp
        Statistics.cpp
        ../Screen/StatisticsDisplay.cpp
        )
SET_TARGET_PROPERTIES(Goblin.Util PROPERTIES CXX_STANDARD 17)
TARGET_INCLUDE_DIRECTORIES(Goblin.Util PUBLIC $<BUILD_INTERFACE:${GOBLIN_CAMP_ROOT_DIR}/Include/>)

TARGET_LINK_LIBRARIES(Goblin.Util PRIVATE Goblin::User::Interface)
TARGET_LINK_LIBRARIES(Goblin.Util PRIVATE Goblin::Renderer::Tile)
TARGET_LINK_LIBRARIES(Goblin.Util PRIVATE Goblin::Scripting)
TARGET_LINK_LIBRARIES(Goblin.Util PRIVATE Goblin::Libtcod)
TARGET_LINK_LIBRARIES(Goblin.Util PRIVATE Goblin::Configuration)
TARGET_LINK_LIBRARIES(Goblin.Util PRIVATE Goblin.Eden)
TARGET_LINK_LIBRARIES(Goblin.Util PRIVATE Goblin.Entity)
TARGET_LINK_LIBRARIES(Goblin.Util PRIVATE Goblin::Geometry)
TARGET_LINK_LIBRARIES(Goblin.Util PRIVATE Goblin.Graphics)
TARGET_LINK_LIBRARIES(Goblin.Util PRIVATE Goblin.Log)
TARGET_LINK_LIBRARIES(Goblin.Util PRIVATE Goblin.Math)
TARGET_LINK_LIBRARIES(Goblin.Util PRIVATE Goblin.Mechanism)
TARGET_LINK_LIBRARIES(Goblin.Util PRIVATE Goblin.Platform)
TARGET_LINK_LIBRARIES(Goblin.Util PRIVATE Goblin.Scripting)
TARGET_LINK_LIBRARIES(Goblin.Util PRIVATE Goblin.Task)