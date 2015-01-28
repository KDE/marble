SET (TARGET marble-game)
PROJECT (${TARGET})

include_directories(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)

if( QT4_FOUND )
  include( ${QT_USE_FILE} )
endif()

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
marble_qt4_automoc(${${TARGET}_SRCS})

add_executable( ${TARGET} ${${TARGET}_SRCS} )

target_link_libraries(
    ${TARGET}
    marblewidget
    marbleui)

if (QT4_FOUND)
  target_link_libraries( ${TARGET} ${QT_QTCORE_LIBRARY} ${QT_QTMAIN_LIBRARY} )
else()
  target_link_libraries( ${TARGET} ${Qt5Core_LIBRARIES} )
endif()

install( TARGETS ${TARGET} RUNTIME DESTINATION ${MARBLE_EXAMPLES_INSTALL_DIR}/${TARGET} )
install( FILES ${${TARGET}_SRCS} DESTINATION ${MARBLE_EXAMPLES_INSTALL_DIR}/${TARGET} )
install( FILES CMakeLists.txt.external DESTINATION ${MARBLE_EXAMPLES_INSTALL_DIR}/${TARGET} RENAME CMakeLists.txt )
install( FILES MapTackRoundHeadRed.png MapTackRoundHeadGreen.png DESTINATION ${MARBLE_EXAMPLES_INSTALL_DIR}/${TARGET} )
