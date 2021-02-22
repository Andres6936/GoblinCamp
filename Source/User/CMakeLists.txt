ADD_LIBRARY (GoblinCamp.UI
        Interface/Announce.cpp
        Interface/AnnounceDialog.cpp
        Interface/Button.cpp
        Interface/ConstructionDialog.cpp
        Interface/DevConsole.cpp
        Interface/JobDialog.cpp
        Interface/Label.cpp
        Interface/Menu.cpp
        Interface/MessageBox.cpp
        Interface/NPCDialog.cpp
        Interface/ScrollPanel.cpp
        Interface/SideBar.cpp
        Interface/Spinner.cpp
        Interface/SquadsDialog.cpp
        Interface/StockManagerDialog.cpp
        Interface/TextBox.cpp
        Interface/Tooltip.cpp
        Interface/UIComponents.cpp
        )

ADD_LIBRARY(GoblinCamp::UI ALIAS GoblinCamp.UI)
TARGET_INCLUDE_DIRECTORIES(GoblinCamp.UI PUBLIC $<BUILD_INTERFACE:${GOBLIN_CAMP_ROOT_DIR}/Include/>)
TARGET_LINK_LIBRARIES(GoblinCamp.UI PUBLIC GoblinCamp::Libtcod)