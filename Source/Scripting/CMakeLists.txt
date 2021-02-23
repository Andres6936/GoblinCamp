ADD_LIBRARY(GoblinCamp.Scripting
        API.cpp
        Engine.cpp
        Event.cpp
        API/Functions.cpp
        API/LoggerStream.cpp
        API/PyConstruction.cpp
        API/PyItem.cpp
        )

ADD_LIBRARY(GoblinCamp::Scripting ALIAS GoblinCamp.Scripting)
TARGET_INCLUDE_DIRECTORIES(GoblinCamp.Scripting
        PUBLIC
        $<BUILD_INTERFACE:${GOBLIN_CAMP_ROOT_DIR}/Include/>
        $<INSTALL_INTERFACE:${GOBLIN_CAMP_ROOT_DIR}/Include/>
        )

TARGET_LINK_LIBRARIES(GoblinCamp.Scripting PUBLIC GoblinCamp::Libtcod)