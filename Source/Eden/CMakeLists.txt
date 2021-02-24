ADD_LIBRARY(Goblin.Eden STATIC
        Map.cpp
        MapMarker.cpp
        NatureObject.cpp
        )
SET_TARGET_PROPERTIES(Goblin.Eden PROPERTIES CXX_STANDARD 17)
TARGET_INCLUDE_DIRECTORIES(Goblin.Eden PUBLIC $<BUILD_INTERFACE:${GOBLIN_CAMP_ROOT_DIR}/Include/>)