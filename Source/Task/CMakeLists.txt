ADD_LIBRARY(Goblin.Task STATIC
        Job.cpp
        JobManager.cpp
        KuhnMunkres.cpp
        Task.cpp
        )
SET_TARGET_PROPERTIES(Goblin.Task PROPERTIES CXX_STANDARD 17)
TARGET_INCLUDE_DIRECTORIES(Goblin.Task PUBLIC $<BUILD_INTERFACE:${GOBLIN_CAMP_ROOT_DIR}/Include/>)