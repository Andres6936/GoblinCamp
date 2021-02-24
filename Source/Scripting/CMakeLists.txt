ADD_LIBRARY(Goblin.Scripting STATIC
        API.cpp
        Engine.cpp
        Event.cpp
        API/Functions.cpp
        API/LoggerStream.cpp
        API/PyConstruction.cpp
        API/PyItem.cpp
        )

ADD_LIBRARY(Goblin::Scripting ALIAS Goblin.Scripting)
TARGET_LINK_LIBRARIES(Goblin.Scripting PUBLIC Goblin::Libtcod)
SET_TARGET_PROPERTIES(Goblin.Scripting PROPERTIES CXX_STANDARD 17)
TARGET_INCLUDE_DIRECTORIES(Goblin.Scripting PUBLIC $<BUILD_INTERFACE:${GOBLIN_CAMP_ROOT_DIR}/Include/>)
