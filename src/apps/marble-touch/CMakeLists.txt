PROJECT( marble-touch )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
IF (QT4BUILD)
  INCLUDE(${QT_USE_FILE})
else()
INCLUDE_DIRECTORIES(
  ${Qt5Qml_INCLUDE_DIRS}
  ${Qt5Quick_INCLUDE_DIRS}
)
ENDIF()

set( my_SRCS
  main.cpp
 )

FILE (GLOB QML_FILES *.qml activities/*.qml)
marble_add_project_resources(${QML_FILES})

qt_add_resources(my_SRCS meego.qrc)
add_executable( marble-touch ${my_SRCS} )
target_link_libraries (
    marble-touch
    ${Qt5Core_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
    ${Qt5Network_LIBRARIES}
    ${Qt5WebKitWidgets_LIBRARIES}
    ${Qt5WebKit_LIBRARIES}
    ${MARBLEWIDGET}
    marbledeclarative
)

install( TARGETS marble-touch RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
if(APPS_INSTALL_DIR)
  install(PROGRAMS marble-touch.desktop DESTINATION ${APPS_INSTALL_DIR})
endif(APPS_INSTALL_DIR)
