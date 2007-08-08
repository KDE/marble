FILE (GLOB ICONS *.icns)
INSTALL (FILES ${ICONS}
         DESTINATION ${CMAKE_INSTALL_PREFIX}/Marble.app/Contents/Resources)
