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