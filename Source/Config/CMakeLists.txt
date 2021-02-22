ADD_LIBRARY(Goblin.Configuration STATIC WindowConfig.cpp)
ADD_LIBRARY(Goblin::Configuration ALIAS Goblin.Configuration)
TARGET_INCLUDE_DIRECTORIES(Goblin.Configuration PUBLIC $<BUILD_INTERFACE:${GOBLIN_CAMP_ROOT_DIR}/Include/>)