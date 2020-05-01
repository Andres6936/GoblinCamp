ADD_SUBDIRECTORY("lib")

INSTALL(FILES terminal.png DESTINATION ${GOBLIN_CAMP_SHAREDIR})

# https://specifications.freedesktop.org/menu-spec/menu-spec-latest.html#paths
# FIXME may be made it dependant on $XDG_DATA_DIRS later
INSTALL(FILES goblin-camp.desktop  DESTINATION "${GOBLIN_CAMP_DATA_BASE_DIR}/applications")

FOREACH(ICON_SIZE 16x16 32x32 48x48 128x128 256x256 )
  # Installing app icons to defaul "hicolor" theme
  # https://specifications.freedesktop.org/icon-theme-spec/icon-theme-spec-latest.html#install_icons
  INSTALL(FILES "goblin-camp-${ICON_SIZE}.png" DESTINATION "${GOBLIN_CAMP_DATA_BASE_DIR}/icons/hicolor/${ICON_SIZE}/apps" RENAME "goblin-camp.png")
ENDFOREACH(ICON_SIZE)

# A fallback icon for Desktop Environments that ignores /usr/share/icons
# https://specifications.freedesktop.org/icon-theme-spec/icon-theme-spec-latest.html#directory_layout
INSTALL(FILES "goblin-camp-128x128.png" DESTINATION "/usr/share/pixmaps" RENAME "goblin-camp.png")



