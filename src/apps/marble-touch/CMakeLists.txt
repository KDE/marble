PROJECT( marble-touch )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
)

set( my_SRCS
  main.cpp
 )

FILE (GLOB QML_FILES *.qml activities/*.qml)
marble_add_project_resources(${QML_FILES})

qt_add_resources(my_SRCS meego.qrc)
add_executable( marble-touch ${my_SRCS} )
target_link_libraries (
    marble-touch
    Qt5::Widgets
    Qt5::Network
    Qt5::WebKitWidgets
    ${MARBLEWIDGET}
    marbledeclarative
)
