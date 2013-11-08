project( marble-mobile )

include_directories(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_SOURCE_DIR}/..
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
 ${CMAKE_CURRENT_SOURCE_DIR}/../marble-ui
)
if( QT4_FOUND )
  include(${QT_USE_FILE})
endif()

set( marblemobile_SRCS
  main.cpp
  MobileMainWindow.cpp
  StackableWindow.cpp
)

add_definitions(-DMARBLE_MOBILE_VERSION_STRING="1.6.20")

marble_qt4_automoc( ${marblemobile_SRCS} )
add_executable( marble-mobile ${marblemobile_SRCS} )
target_link_libraries( marble-mobile ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTMAIN_LIBRARY} marbleui marblewidget )
install( TARGETS marble-mobile RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )

install(PROGRAMS marble-mobile.desktop DESTINATION ${APPS_INSTALL_DIR})
