ADD_LIBRARY (Goblin.User.Interface
        Interface/Announce.cpp
        Interface/AnnounceDialog.cpp
        Interface/Button.cpp
        Interface/ConstructionDialog.cpp
        Interface/Dialog.cpp
        Interface/Frame.cpp
        Interface/Grid.cpp
        Interface/JobDialog.cpp
        Interface/Label.cpp
        Interface/Menu.cpp
        Interface/MessageBox.cpp
        Interface/NPCDialog.cpp
        Interface/Panel.cpp
        Interface/ScrollPanel.cpp
        Interface/SideBar.cpp
        Interface/Spinner.cpp
        Interface/SquadsDialog.cpp
        Interface/StockManagerDialog.cpp
        Interface/TextBox.cpp
        Interface/Tooltip.cpp
        Interface/UIContainer.cpp
        )

ADD_LIBRARY(Goblin::User::Interface ALIAS Goblin.User.Interface)
SET_TARGET_PROPERTIES(Goblin.User.Interface PROPERTIES CXX_STANDARD 17)
TARGET_LINK_LIBRARIES(Goblin.User.Interface PUBLIC Goblin::Libtcod)
TARGET_INCLUDE_DIRECTORIES(Goblin.User.Interface PUBLIC $<BUILD_INTERFACE:${GOBLIN_CAMP_ROOT_DIR}/Include/>)
