ADD_LIBRARY(Goblin.Task STATIC
        Job.cpp
        JobManager.cpp
        KuhnMunkres.cpp
        )
TARGET_INCLUDE_DIRECTORIES(Goblin.Task PUBLIC $<BUILD_INTERFACE:${GOBLIN_CAMP_ROOT_DIR}/Include/>)