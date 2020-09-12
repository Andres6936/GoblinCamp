ADD_LIBRARY (GoblinCamp.UI
    AnnounceDialog.cpp
    ConstructionDialog.cpp
    DevConsole.cpp
    JobDialog.cpp
    Menu.cpp
    MessageBox.cpp
    NPCDialog.cpp
    SideBar.cpp
    SquadsDialog.cpp
    StockManagerDialog.cpp
    Tooltip.cpp
    UIComponents.cpp
)

ADD_LIBRARY(GoblinCamp::UI ALIAS GoblinCamp.UI)
TARGET_INCLUDE_DIRECTORIES(GoblinCamp.UI
        PUBLIC
        $<BUILD_INTERFACE:${GOBLIN_CAMP_ROOT_DIR}/Include/>
        $<INSTALL_INTERFACE:${GOBLIN_CAMP_ROOT_DIR}/Include/>
        )

TARGET_LINK_LIBRARIES(GoblinCamp.UI PUBLIC GoblinCamp::Libtcod)