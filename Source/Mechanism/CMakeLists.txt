ADD_LIBRARY(Goblin.Mechanism STATIC
        Attack.cpp
        Camp.cpp
        Door.cpp
        Faction.cpp
        Filth.cpp
        SpawningPool.cpp
        Squad.cpp
        StatusEffect.cpp
        Stockpile.cpp
        Water.cpp
        Blood.cpp
        Construction.cpp
        Events.cpp
        Farmplot.cpp
        Fire.cpp
        Spell.cpp
        Stats.cpp
        StockManager.cpp
        Trap.cpp
        Weather.cpp
        )
TARGET_INCLUDE_DIRECTORIES(Goblin.Mechanism PUBLIC $<BUILD_INTERFACE:${GOBLIN_CAMP_ROOT_DIR}/Include/>)