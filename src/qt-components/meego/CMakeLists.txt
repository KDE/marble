PROJECT( Meego )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/qt-components/meego
 ${CMAKE_BINARY_DIR}/src/qt-components/meego
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( my_SRCS
  main.cpp
  MainWindow.cpp
 )

qt4_add_resources(my_SRCS meego.qrc)
qt4_automoc( ${my_SRCS} )
add_executable( meego ${my_SRCS} )
target_link_libraries( meego ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTMAIN_LIBRARY} ${QT_QTDECLARATIVE_LIBRARY} )
install( TARGETS meego RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
