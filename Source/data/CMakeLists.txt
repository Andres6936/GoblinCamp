ADD_LIBRARY (GoblinCamp.Data
    Config.cpp
    Data.cpp
    Mods.cpp
    Paths.cpp
    Serialization.cpp
    Tilesets.cpp
)

ADD_LIBRARY(GoblinCamp::Data ALIAS GoblinCamp.Data)