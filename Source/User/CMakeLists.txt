ADD_LIBRARY (GoblinCamp.UI
        Interface/AnnounceDialog.cpp
        Interface/ConstructionDialog.cpp
        Interface/DevConsole.cpp
        Interface/JobDialog.cpp
        Interface/Menu.cpp
        Interface/MessageBox.cpp
        Interface/NPCDialog.cpp
        Interface/SideBar.cpp
        Interface/SquadsDialog.cpp
        Interface/StockManagerDialog.cpp
        Interface/Tooltip.cpp
        Interface/UIComponents.cpp
        )

ADD_LIBRARY(GoblinCamp::UI ALIAS GoblinCamp.UI)
TARGET_INCLUDE_DIRECTORIES(GoblinCamp.UI
        PUBLIC
        $<BUILD_INTERFACE:${GOBLIN_CAMP_ROOT_DIR}/Include/>
        $<INSTALL_INTERFACE:${GOBLIN_CAMP_ROOT_DIR}/Include/>
        )

TARGET_LINK_LIBRARIES(GoblinCamp.UI PUBLIC GoblinCamp::Libtcod)