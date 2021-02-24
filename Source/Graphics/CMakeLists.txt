ADD_LIBRARY(Goblin.Graphics STATIC
        TCODMapRenderer.cpp
        UI.cpp
        )
TARGET_INCLUDE_DIRECTORIES(Goblin.Graphics PUBLIC $<BUILD_INTERFACE:${GOBLIN_CAMP_ROOT_DIR}/Include/>)