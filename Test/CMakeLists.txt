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
