SET (TARGET marble-game)
PROJECT (${TARGET})

include_directories(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set(${TARGET}_HDRS
    GameMainWindow.h
    CountryByShape.h
    CountryByFlag.h
    ClickOnThat.h
)

set(${TARGET}_SRCS
    gamemain.cpp 
    GameMainWindow.cpp
    CountryByShape.cpp
    CountryByFlag.cpp
    ClickOnThat.cpp
)

FILE (GLOB QML_FILES *.qml)
marble_add_project_resources(${QML_FILES})
qt_add_resources(${TARGET}_SRCS marbleGame.qrc)

qt_wrap_ui(${TARGET}_SRCS game.ui)

add_executable( ${TARGET} ${${TARGET}_SRCS} )

target_link_libraries(
    ${TARGET}
    marblewidget
    marbleui
    Qt5::Quick
)

install( TARGETS ${TARGET} RUNTIME DESTINATION ${MARBLE_EXAMPLES_INSTALL_DIR}/${TARGET} )
# TODO: add to qrc and get from there
install( FILES MapTackRoundHeadRed.png MapTackRoundHeadGreen.png DESTINATION ${MARBLE_EXAMPLES_INSTALL_DIR}/${TARGET} )

return()
# TODO: qrc refers to lots of flags in marble sources
# needs a custom variant of the qrc file (-> configure_file) and reading the flags via alias
# TODO: do not install sources for now, MarblePlacemarkModel.h is not ready/good for public usages

install( FILES
    ${${TARGET}_HDRS}
    ${${TARGET}_SRCS}
    ${QML_FILES}
    game.ui
    marbleGame.qrc
    DESTINATION ${MARBLE_EXAMPLES_INSTALL_DIR}/${TARGET}
)
install( FILES CMakeLists.txt.external DESTINATION ${MARBLE_EXAMPLES_INSTALL_DIR}/${TARGET} RENAME CMakeLists.txt )
