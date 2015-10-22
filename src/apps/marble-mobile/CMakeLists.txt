project( marble-mobile )

include_directories(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_SOURCE_DIR}/..
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
 ${CMAKE_CURRENT_SOURCE_DIR}/../marble-ui
)

INCLUDE_DIRECTORIES(
${Qt5WebKitWidgets_INCLUDE_DIRS}
)

set( marblemobile_SRCS
  main.cpp
  MobileMainWindow.cpp
  StackableWindow.cpp
)

add_definitions(-DMARBLE_MOBILE_VERSION_STRING="1.7.20")

add_executable( marble-mobile ${marblemobile_SRCS} )
target_link_libraries( marble-mobile ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTMAIN_LIBRARY} marbleui ${MARBLEWIDGET})
