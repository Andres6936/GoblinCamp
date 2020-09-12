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