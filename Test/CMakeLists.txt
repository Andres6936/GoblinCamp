# Added Doctest as a Dependency
CPMADDPACKAGE(
        NAME DOCTEST
        GITHUB_REPOSITORY onqtam/doctest
        GIT_TAG 2.4.4
)

# Added Mocxx as a Dependency
CPMADDPACKAGE(
        NAME MOCXX
        GITHUB_REPOSITORY Guardsquare/mocxx
        # Use a recent commit, not released published
        GIT_TAG ae0d1428d55b7176fcca8b1008ec37acb18b4114
)

ENABLE_TESTING()

MACRO(GOBLIN_TEST_UNIT _NAME_TARGET _NAME_FILE)

    ADD_EXECUTABLE(${_NAME_TARGET} ${_NAME_FILE})
    SET_TARGET_PROPERTIES(${_NAME_TARGET} PROPERTIES CXX_STANDARD 17)
    TARGET_LINK_LIBRARIES(${_NAME_TARGET} PRIVATE doctest)
    ADD_TEST(NAME ${_NAME_TARGET} COMMAND ${_NAME_TARGET})

ENDMACRO()

# Execute the test in the directories
ADD_SUBDIRECTORY(Unit/)