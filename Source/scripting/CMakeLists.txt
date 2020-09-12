ADD_LIBRARY(GoblinCamp.Scripting
    API.cpp
    Engine.cpp
    Event.cpp
    _gcampapi/Functions.cpp
    _gcampapi/LoggerStream.cpp
    _gcampapi/PyConstruction.cpp
    _gcampapi/PyItem.cpp
)

ADD_LIBRARY(GoblinCamp::Scripting ALIAS GoblinCamp.Scripting)
TARGET_INCLUDE_DIRECTORIES(GoblinCamp.Scripting
        PUBLIC
        $<BUILD_INTERFACE:${GOBLIN_CAMP_ROOT_DIR}/Include/>
        $<INSTALL_INTERFACE:${GOBLIN_CAMP_ROOT_DIR}/Include/>
        )

TARGET_LINK_LIBRARIES(GoblinCamp.Scripting PUBLIC GoblinCamp::Libtcod)