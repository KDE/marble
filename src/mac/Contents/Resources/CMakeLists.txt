FILE (GLOB ICONS *.icns)
IF(QTONLY)
    INSTALL (FILES ${ICONS}
         DESTINATION ${CMAKE_INSTALL_PREFIX}/Marble.app/Contents/Resources)
ELSE(QTONLY)
    # KDE4/Mac
    INSTALL (FILES ${ICONS}
         DESTINATION ${BUNDLE_INSTALL_DIR}/marble.app/Contents/Resources)
ENDIF(QTONLY)
