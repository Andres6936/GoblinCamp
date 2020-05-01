
# TODO compile python modules into bitecode before (.pyc) before installing

INSTALL(FILES __gcdevconsole__.py DESTINATION ${GOBLIN_CAMP_SHAREDIR}/lib)
INSTALL(DIRECTORY gcamp gcamp_core tilesets_core DESTINATION ${GOBLIN_CAMP_SHAREDIR}/lib)


